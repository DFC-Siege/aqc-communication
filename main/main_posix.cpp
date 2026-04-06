#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>
#include <thread>
#include <unordered_map>

#include "base_transporter.hpp"
#include "chunked_transporter.hpp"
#include "console_logger.hpp"
#include "direct_transporter.hpp"
#include "dispatcher.hpp"
#include "i_logger.hpp"
#include "logger.hpp"
#include "multiplexer.hpp"
#include "result.hpp"
#include "serial_hal.hpp"
#include "serial_transporter.hpp"
#include "serializer.hpp"

static constexpr auto TAG = "main";

enum Channel : transport::TransporterId {
        Chunked,
        Direct,
};

enum Command : transport::CommandId {
        Ping,
};

struct PingMessage {
        uint32_t sequence;
        uint32_t timestamp;

        serializer::Data serialize() const {
                serializer::Writer w;
                w.write(sequence);
                w.write(timestamp);
                return std::move(w.buf);
        }

        static result::Result<PingMessage>
        deserialize(serializer::DataView buf) {
                serializer::Reader r{buf};
                return result::ok(PingMessage{
                    .sequence = TRY(r.read<uint32_t>()),
                    .timestamp = TRY(r.read<uint32_t>()),
                });
        }
};

static_assert(serializer::Serializable<PingMessage>);

static result::Result<bool>
handle_ping(result::Result<transport::Data> result) {
        const auto data = TRY(result);
        const auto ping = TRY(PingMessage::deserialize(data));
        logging::logger().println(logging::LogLevel::Info, TAG,
                                  "ping seq=" + std::to_string(ping.sequence) +
                                      " ts=" + std::to_string(ping.timestamp));
        return result::ok();
}

int main(int argc, char *argv[]) {
        auto logger = std::make_unique<logging::ConsoleLogger>();
        logger->set_level(logging::LogLevel::Info);
        logging::Logger::set(std::move(logger));

        if (argc < 2) {
                logging::logger().println(logging::LogLevel::Error, TAG,
                                          "usage: communication <device>");
                return 1;
        }

        static constexpr auto MTU = 17;
        static constexpr auto MAX_TRIES = 1;
        static constexpr auto TIMEOUT = std::chrono::milliseconds(10000);

        static constexpr auto BAUDRATE = 115200;
        static constexpr auto BUFFER_SIZE = 1024;
        static constexpr auto UART = UART_NUM_1;
        static constexpr auto TX_PIN = 7;
        static constexpr auto RX_PIN = 6;
        serial::SerialHal serial_hal(UART, TX_PIN, RX_PIN, BAUDRATE,
                                     BUFFER_SIZE);
        transport::SerialTransporter serial_transporter(serial_hal, MTU);
        transport::Multiplexer multiplexer(serial_transporter);

        using MuxChannel =
            transport::Multiplexer<transport::SerialTransporter>::InnerChannel;
        using ChunkedMuxChannel = transport::ChunkedTransporter<MuxChannel>;
        using DirectMuxChannel = transport::DirectTransporter<MuxChannel>;

        auto &inner_chunked_channel =
            multiplexer.create_inner_channel(Channel::Chunked);
        auto chunked = std::make_unique<ChunkedMuxChannel>(
            inner_chunked_channel, MAX_TRIES, TIMEOUT);

        auto &inner_direct_channel =
            multiplexer.create_inner_channel(Channel::Direct);
        auto direct = std::make_unique<DirectMuxChannel>(inner_direct_channel);

        transport::Dispatcher<transport::BaseTransporter> dispatcher;
        dispatcher.register_transporter(Channel::Chunked, std::move(chunked));
        dispatcher.register_transporter(Channel::Direct, std::move(direct));

        dispatcher.register_handler(
            Command::Ping, [](result::Result<transport::Data> result) {
                    const auto r = handle_ping(std::move(result));
                    if (r.failed()) {
                            logging::logger().println(logging::LogLevel::Error,
                                                      TAG, r.error());
                    }
            });

        const PingMessage msg{.sequence = 1, .timestamp = 42};
        const auto send_result =
            dispatcher.send(Channel::Chunked, Command::Ping, msg.serialize());
        if (send_result.failed()) {
                logging::logger().println(logging::LogLevel::Error, TAG,
                                          send_result.error());
        }

        const PingMessage msg2{.sequence = 2, .timestamp = 43};
        const auto send_result2 =
            dispatcher.send(Channel::Direct, Command::Ping, msg2.serialize());
        if (send_result2.failed()) {
                logging::logger().println(logging::LogLevel::Error, TAG,
                                          send_result2.error());
        }

        while (true) {
                serial_hal.loop();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
}
