#include <chrono>
#include <cstdint>
#include <memory>
#include <string_view>
#include <thread>

#include "console_logger.hpp"
#include "logger.hpp"
#include "result.hpp"

int main() {
        logging::Logger::set(std::make_unique<logging::ConsoleLogger>());

        uint32_t counter = 0;
        while (true) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                const auto str = std::to_string(counter++);
                logging::logger().println_fmt("count: {}", str);
        }

        return 0;
}
