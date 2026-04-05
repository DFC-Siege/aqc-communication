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
#include "dispatcher.hpp"
#include "i_logger.hpp"
#include "logger.hpp"
#include "multiplexer.hpp"
#include "result.hpp"
#include "serial_hal.hpp"
#include "serial_transporter.hpp"

static constexpr auto TAG = "main";

enum Channel : transport::TransporterId {
        Chunked,
        Direct,
};

enum Command : transport::CommandId {
        Ping,
};

// int main(int argc, char *argv[]) {
//         auto logger = std::make_unique<logging::ConsoleLogger>();
//         logger->set_level(logging::LogLevel::Debug);
//         logging::Logger::set(std::move(logger));
//
//         if (argc < 2) {
//                 logging::logger().println(logging::LogLevel::Error, TAG,
//                                           "usage: communication <device>");
//                 return 1;
//         }
//
//         const auto DEVICE = argv[1];
//         static constexpr uint16_t MTU = 24;
//         static constexpr uint16_t MAX_TRIES = 1;
//
//         serial::SerialHal serial_hal(DEVICE);
//         transport::SerialTransporter serial_transporter(serial_hal, MTU);
//         transport::Multiplexer multiplexer(serial_transporter);
//
//         using MuxChannel =
//             transport::Multiplexer<transport::SerialTransporter>::InnerChannel;
//         using ChunkedMuxChannel = transport::ChunkedTransporter<MuxChannel>;
//
//         auto &inner_channel =
//             multiplexer.create_inner_channel(Channel::Chunked);
//         auto chunked =
//             std::make_unique<ChunkedMuxChannel>(inner_channel, MAX_TRIES);
//         auto &channel =
//             multiplexer.register_channel(Channel::Chunked,
//             std::move(chunked));
//
//         transport::Dispatcher dispatcher(channel);
//
//         dispatcher.register_handler(
//             Command::Ping, [](result::Result<transport::Data> result) {
//                     if (result.failed()) {
//                             logging::logger().println(logging::LogLevel::Error,
//                                                       TAG, result.error());
//                             return;
//                     }
//                     const auto data = std::move(result).value();
//                     logging::logger().println(
//                         logging::LogLevel::Info, TAG,
//                         std::string(data.begin(), data.end()));
//             });
//
//         static constexpr std::string_view msg =
//             "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do
//             " "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut
//             " "enim ad minim veniam, quis nostrud exercitation ullamco
//             laboris " "nisi ut aliquip ex ea commodo consequat.";
//         const auto send_result = dispatcher.send(
//             Command::Ping, transport::Data(msg.begin(), msg.end()));
//         if (send_result.failed()) {
//                 logging::logger().println(logging::LogLevel::Error, TAG,
//                                           send_result.error());
//         }
//
//         while (true) {
//                 serial_hal.loop();
//                 std::this_thread::sleep_for(std::chrono::milliseconds(1));
//         }
// }

int main(int argc, char *argv[]) {
        auto logger = std::make_unique<logging::ConsoleLogger>();
        logger->set_level(logging::LogLevel::Debug);
        logging::Logger::set(std::move(logger));

        if (argc < 2) {
                logging::logger().println(logging::LogLevel::Error, TAG,
                                          "usage: communication <device>");
                return 1;
        }

        const auto DEVICE = argv[1];
        static constexpr uint16_t MTU = 24;
        static constexpr uint16_t MAX_TRIES = 1;

        serial::SerialHal serial_hal(DEVICE);
        transport::SerialTransporter serial_transporter(serial_hal, MTU);
        transport::ChunkedTransporter chunked_transporter(serial_transporter,
                                                          MAX_TRIES);

        transport::Dispatcher dispatcher(chunked_transporter);

        dispatcher.register_handler(
            Command::Ping, [](result::Result<transport::Data> result) {
                    if (result.failed()) {
                            logging::logger().println(logging::LogLevel::Error,
                                                      TAG, result.error());
                            return;
                    }
                    const auto data = std::move(result).value();
                    logging::logger().println(
                        logging::LogLevel::Info, TAG,
                        std::string(data.begin(), data.end()));
            });

        static constexpr std::string_view msg =
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
            "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut "
            "enim ad minim veniam, quis nostrud exercitation ullamco laboris "
            "nisi ut aliquip ex ea commodo consequat.";
        const auto send_result = dispatcher.send(
            Command::Ping, transport::Data(msg.begin(), msg.end()));
        if (send_result.failed()) {
                logging::logger().println(logging::LogLevel::Error, TAG,
                                          send_result.error());
        }

        while (true) {
                serial_hal.loop();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
}
