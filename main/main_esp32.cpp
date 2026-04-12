#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>
#include <unordered_map>

#include <esp_task_wdt.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <hal/uart_types.h>

#include "communication_manager.hpp"
#include "console_logger.hpp"
#include "handler_factory.hpp"
#include "i_logger.hpp"
#include "logger.hpp"
#include "result.hpp"
#include "serial_hal.hpp"

extern "C" void app_main() {
        auto logger = std::make_unique<logging::ConsoleLogger>();
        logger->set_level(logging::LogLevel::Info);
        logging::set_logger(std::move(logger));

        static constexpr auto BAUDRATE = 115200;
        static constexpr auto BUFFER_SIZE = 1024;
        static constexpr auto UART = UART_NUM_1;
        static constexpr auto TX_PIN = 7;
        static constexpr auto RX_PIN = 6;
        serial::SerialHal serial_hal(UART, TX_PIN, RX_PIN, BAUDRATE,
                                     BUFFER_SIZE);
        handlers::HandlerFactory handler_factory;
        communication::CommunicationManager communication_manager(
            serial_hal, std::move(handler_factory));

        xTaskCreate(
            [](void *arg) {
                    auto *hal = static_cast<serial::SerialHal *>(arg);
                    while (true) {
                            hal->loop();
                    }
            },
            "serial", 8192, &serial_hal, 5, nullptr);

        while (true) {
                vTaskDelay(portMAX_DELAY);
        }
}
