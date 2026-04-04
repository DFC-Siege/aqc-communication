#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>
#include <thread>
#include <unordered_map>

#include "chunked_transporter.hpp"
#include "console_logger.hpp"
#include "dispatcher.hpp"
#include "i_logger.hpp"
#include "logger.hpp"
#include "multiplexer.hpp"
#include "result.hpp"
#include "serial_hal.hpp"
#include "serial_transporter.hpp"

static constexpr auto TAG = "main";

enum Command : transport::CommandId {
        Ping,
};

int main(int argc, char *argv[]) {
        logging::Logger::set(std::make_unique<logging::ConsoleLogger>());
        if (argc < 2) {
                logging::logger().println(logging::LogLevel::Error, TAG,
                                          "usage: communication <device>");
                return 1;
        }
        const auto DEVICE = argv[1];
        static constexpr uint16_t MTU = 16;
        static constexpr uint16_t MAX_TRIES = 1;

        serial::SerialHal serial_hal(DEVICE);
        transport::SerialTransporter serial_base(serial_hal, MTU);

        using SerialChunked =
            transport::ChunkedTransporter<transport::SerialTransporter>;
        SerialChunked chunked_serial(serial_base, MAX_TRIES);

        std::unordered_map<transport::TransporterId,
                           std::reference_wrapper<transport::BaseTransporter>>
            transporters{{0x00, chunked_serial}};

        transport::Multiplexer<transport::SerialTransporter> multiplexer(
            serial_base, std::move(transporters));
        auto channel = multiplexer.get_channel(0x00).value();
        transport::Dispatcher dispatcher(channel);
        static constexpr std::string_view msg =
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
            "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut "
            "enim ad minim veniam, quis nostrud exercitation ullamco laboris "
            "nisi ut aliquip ex ea commodo consequat.";
        dispatcher.send(Command::Ping, transport::Data(msg.begin(), msg.end()));
        dispatcher.register_handler(
            Command::Ping, [](result::Result<transport::Data> result) {
                    if (result.failed()) {
                            logging::logger().println(logging::LogLevel::Error,
                                                      TAG, result.error());
                            return;
                    }
                    const auto data = std::move(result).value();
                    const auto str = std::string(data.begin(), data.end());
                    logging::logger().println(logging::LogLevel::Info, TAG,
                                              str);
            });

        while (true) {
                serial_hal.loop();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
}
