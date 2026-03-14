#include <cstdio>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <memory>
#include <nvs_flash.h>

#include "ble/ble_manager.hpp"
#include "logger.hpp"
#include "nvs/nvs_store.hpp"
#include "serial/serial_manager.hpp"
#include "serial_logger.hpp"

extern "C" {
void app_main(void) {
        Logging::Logger::set(std::make_unique<Logging::SerialLogger>());

        auto store_result = Storage::KV::NVSStore::init("default");
        if (store_result.failed) {
                Logging::logger().print_fmt("error initializing store: {}",
                                            store_result.err);
                return;
        }

        auto &ble = BLE::BLEManager::instance();
        ble.on_connection_changed([](bool connected) {
                Logging::logger().print_fmt(
                    "BLE {}", connected ? "connected" : "disconnected");
        });
        ble.on_receive([](std::span<const uint8_t> data) {
                Logging::logger().print_fmt(
                    "BLE received: {}",
                    std::string(reinterpret_cast<const char *>(data.data()),
                                data.size()));
        });
        ble.begin("esp32");

        Communication::SerialManager serial_manager;
        serial_manager.add_listener([](std::string value) {
                Logging::logger().print_fmt("recieved: {}", value);
        });

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
                        Logging::logger().print_fmt("sending: {}", str);
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
