#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
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
#include "requester.hpp"
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
        SCD,
        SCDRequest,
};

struct SCDData {
        uint16_t co2 = 0;
        float temperature = 0.0f;
        float humidity = 0.0f;
        std::string error;

        serializer::Data serialize() const {
                serializer::Writer w;
                w.write(co2);
                w.write(temperature);
                w.write(humidity);
                w.write(error);
                return std::move(w.buf);
        }

        static result::Result<SCDData> deserialize(serializer::DataView buf) {
                serializer::Reader r{buf};
                return result::ok(SCDData{
                    .co2 = TRY(r.read<uint16_t>()),
                    .temperature = TRY(r.read<float>()),
                    .humidity = TRY(r.read<float>()),
                    .error = TRY(r.read_string()),
                });
        }
};

static_assert(serializer::Serializable<SCDData>);

int main(int argc, char *argv[]) {
        auto logger = std::make_unique<logging::ConsoleLogger>();
        logger->set_level(logging::LogLevel::Info);
        logging::set_logger(std::move(logger));

        if (argc < 2) {
                logging::logger().println(logging::LogLevel::Error, TAG,
                                          "usage: communication <device>");
                return 1;
        }

        static constexpr auto MTU = 17;
        static constexpr auto MAX_TRIES = 1;
        static constexpr auto TIMEOUT = std::chrono::milliseconds(10000);
        serial::SerialHal serial_hal(argv[1]);
        auto serial_transporter =
            std::make_unique<transport::SerialTransporter>(serial_hal, MTU);
        transport::Multiplexer multiplexer(std::move(serial_transporter));

        using MuxChannel =
            transport::Multiplexer<transport::SerialTransporter>::InnerChannel;
        using ChunkedMuxChannel = transport::ChunkedTransporter<MuxChannel>;
        using DirectMuxChannel = transport::DirectTransporter<MuxChannel>;

        auto inner_chunked_channel =
            multiplexer.create_inner_channel(Channel::Chunked);
        auto chunked = std::make_unique<ChunkedMuxChannel>(
            std::move(inner_chunked_channel), MAX_TRIES, TIMEOUT);

        auto inner_direct_channel =
            multiplexer.create_inner_channel(Channel::Direct);
        auto direct =
            std::make_unique<DirectMuxChannel>(std::move(inner_direct_channel));

        transport::Dispatcher<transport::BaseTransporter> dispatcher;
        dispatcher.register_transporter(Channel::Chunked, std::move(chunked));
        dispatcher.register_transporter(Channel::Direct, std::move(direct));

        transport::Requester<transport::BaseTransporter> requester(dispatcher);

        requester.register_requestable<SCDData, SCDData>(
            Command::SCDRequest, Command::SCDRequest, Channel::Chunked,
            [](SCDData query) -> result::Result<SCDData> {
                    logging::logger().println(logging::LogLevel::Info, TAG,
                                              "received SCDRequest");
                    return result::ok(SCDData{
                        .co2 = 420,
                        .temperature = 23.5f,
                        .humidity = 55.0f,
                        .error = "",
                    });
            });

        std::thread serial_thread([&] {
                while (true) {
                        serial_hal.loop();
                        std::this_thread::sleep_for(
                            std::chrono::milliseconds(1));
                }
        });

        logging::logger().println(logging::LogLevel::Info, TAG,
                                  "listening for requests...");

        auto handle_result =
            requester.send_request(Channel::Chunked, Command::SCDRequest,
                                   Command::SCDRequest, SCDData{});
        if (handle_result.failed()) {
                logging::logger().println(logging::LogLevel::Error, TAG,
                                          handle_result.error());
        } else {
                logging::logger().println(
                    logging::LogLevel::Info, TAG,
                    "sent SCDRequest, awaiting response...");
                auto data_result = handle_result.value()->await<SCDData>(
                    std::chrono::milliseconds(5000));
                if (data_result.failed()) {
                        logging::logger().println(logging::LogLevel::Error, TAG,
                                                  data_result.error());
                } else {
                        const auto &data = data_result.value();
                        logging::logger().println_fmt(
                            logging::LogLevel::Info,
                            "SCDResponse co2: {} temp: {:.2f} humidity: {:.2f}",
                            data.co2, data.temperature, data.humidity);
                }
        }

        serial_thread.join();
}
