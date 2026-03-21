#include <chrono>
#include <cstdint>
#include <memory>
#include <string_view>
#include <thread>

#include "console_logger.hpp"
#include "logger.hpp"
#include "result.hpp"
#include "serial_hal.hpp"

int main() {
        logging::Logger::set(std::make_unique<logging::ConsoleLogger>());

        serial::SerialHal serial_hal("/dev/ttyACM0");
        serial_hal.on_receive([](std::span<const uint8_t> data) {
                logging::logger().println(
                    "serial", std::string_view(
                                  reinterpret_cast<const char *>(data.data()),
                                  data.size()));
        });

        uint32_t counter = 0;
        while (true) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                serial_hal.loop();

                const auto str = std::to_string(counter++);
                logging::logger().println_fmt("count: {}", str);
        }

        return 0;
}
