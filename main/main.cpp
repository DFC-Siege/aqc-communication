#include <cstdio>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <memory>
#include <nvs_flash.h>

#include "ble/ble_manager.hpp"
#include "logger.hpp"
#include "serial/serial_manager.hpp"
#include "serial_logger.hpp"

extern "C" {
void app_main(void) {
        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
            ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
                nvs_flash_erase();
                nvs_flash_init();
        }

        Logging::Logger::set(std::make_unique<Logging::SerialLogger>());
        Logging::logger().println("yee");

        auto &ble = BLE::BLEManager::instance();
        ble.on_connection_changed([](bool connected) {
                printf("BLE %s\n", connected ? "connected" : "disconnected");
        });
        ble.on_receive([](std::span<const uint8_t> data) {
                printf("BLE received: %.*s\n", static_cast<int>(data.size()),
                       data.data());
        });
        ble.begin("esp32");

        Communication::SerialManager serial_manager;
        serial_manager.add_listener(
            [](std::string value) { printf("received: %s\n", value.c_str()); });

        uint32_t counter = 0;
        TickType_t last_wake_time = xTaskGetTickCount();
        const TickType_t interval = pdMS_TO_TICKS(1000);

        while (true) {
                serial_manager.update();

                TickType_t current_time = xTaskGetTickCount();
                if (current_time - last_wake_time >= interval) {
                        last_wake_time = current_time;
                        const auto str =
                            "Count: " + std::to_string(counter++) + "\n";
                        printf("sending: %s", str.c_str());
                        serial_manager.send(str);

                        if (ble.is_connected()) {
                                auto bytes = std::span<const uint8_t>(
                                    reinterpret_cast<const uint8_t *>(
                                        str.data()),
                                    str.size());
                                ble.send(bytes);
                        }
                }

                vTaskDelay(1);
        }
}
}
