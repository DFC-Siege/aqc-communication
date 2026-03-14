#include <cstdint>
#include <cstdio>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <memory>
#include <nvs_flash.h>

#include "ble/ble_manager.hpp"
#include "communication_handler.hpp"
#include "logger.hpp"
#include "serial/serial_manager.hpp"
#include "serial_logger.hpp"

extern "C" {
void app_main(void) {
        Logging::Logger::set(std::make_unique<Logging::SerialLogger>());

        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
            ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
                nvs_flash_erase();
                nvs_flash_init();
        }

        auto &ble = BLE::BLEManager::instance();
        ble.on_connection_changed([](bool connected) {
                Logging::logger().println_fmt(
                    "BLE {}", connected ? "connected" : "disconnected");
        });
        ble.begin("aqc");

        Serial::SerialManager serial_manager;
        serial_manager.on_receive([](std::span<const uint8_t> data) {
                Logging::logger().println(
                    "serial", std::string_view(
                                  reinterpret_cast<const char *>(data.data()),
                                  data.size()));
        });

        Communication::CommunicationHandler communication_handler(
            ble, serial_manager);

        uint32_t counter = 0;
        TickType_t last_wake_time = xTaskGetTickCount();
        const TickType_t interval = pdMS_TO_TICKS(1000);

        while (true) {
                serial_manager.loop();

                TickType_t current_time = xTaskGetTickCount();
                if (current_time - last_wake_time >= interval) {
                        last_wake_time = current_time;
                        const auto str = std::to_string(counter++);
                        Logging::logger().println_fmt("count: {}", str);

                        const auto span = std::span<const uint8_t>(
                            reinterpret_cast<const uint8_t *>(str.data()),
                            str.size());

                        communication_handler.ble_transporter.send(
                            0x01, span,
                            []() {
                                    Logging::logger().println("send complete");
                            },
                            [](std::string_view error) {
                                    Logging::logger().println_fmt(
                                        Logging::LogLevel::Error,
                                        "send error: {}", error);
                            });
                }

                vTaskDelay(1);
        }
}
}
