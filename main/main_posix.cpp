#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>

#include "communication_manager.hpp"
#include "console_logger.hpp"
#include "handler_factory.hpp"
#include "i_logger.hpp"
#include "logger.hpp"
#include "result.hpp"
#include "serial_hal.hpp"

static constexpr auto TAG = "main";

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
        handlers::HandlerFactory handler_factory;
        communication::CommunicationManager communication_manager(
            serial_hal, std::move(handler_factory));

        std::thread serial_thread([&] {
                while (true) {
                        serial_hal.loop();
                        std::this_thread::sleep_for(
                            std::chrono::milliseconds(1));
                }
        });

        serial_thread.join();
}
