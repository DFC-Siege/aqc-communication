#include <cstdint>
#include <cstdio>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <memory>
#include <nvs_flash.h>

#include "ble/ble_manager.hpp"
#include "communication_handler.hpp"
#include "i_logger.hpp"
#include "logger.hpp"
#include "nvs/nvs_store.hpp"
#include "serial/serial_manager.hpp"
#include "serial_logger.hpp"

extern "C" {
void app_main(void) {
        Logging::Logger::set(std::make_unique<Logging::SerialLogger>());

        auto store_result = Storage::KV::NVSStore::init("default");
        if (store_result.failed()) {
                Logging::logger().print_fmt("error initializing store: {}",
                                            store_result.error());
                return;
        }

        auto &ble = BLE::BLEManager::instance();
        ble.on_connection_changed([](bool connected) {
                Logging::logger().print_fmt(
                    "BLE {}", connected ? "connected" : "disconnected");
        });
        static constexpr auto BLE_NAME = "aqc";
        ble.begin(BLE_NAME);

        Serial::SerialManager serial_manager;
        serial_manager.on_receive([](std::span<const uint8_t> data) {
                std::string str(reinterpret_cast<const char *>(data.data()),
                                data.size());
                Logging::logger().println("serial", str);
        });

        Communication::CommunicationHandler communication_handler(
            ble, serial_manager);

        const std::string str = "yee";
        const auto send_result = communication_handler.ble_transporter.send(
            0x01,
            std::span<const uint8_t>(
                reinterpret_cast<const uint8_t *>(str.data()), str.size()),
            []() { Logging::logger().println("Success!!!!!"); },
            [](std::string_view error) {
                    Logging::logger().println_fmt(
                        Logging::LogLevel::Error,
                        "on_error: error sending command: {}", error);
            });
        if (send_result.failed()) {
                Logging::logger().println_fmt(
                    Logging::LogLevel::Error,
                    "result: error sending command: {}", send_result.error());
        }

        uint32_t counter = 0;
        TickType_t last_wake_time = xTaskGetTickCount();
        const TickType_t interval = pdMS_TO_TICKS(1000);

        while (true) {
                serial_manager.loop();

                TickType_t current_time = xTaskGetTickCount();
                if (current_time - last_wake_time >= interval) {
                        last_wake_time = current_time;
                        const auto str =
                            "Count: " + std::to_string(counter++) + "\n";
                        Logging::logger().print_fmt("sending: {}", str);
                        serial_manager.send(std::span<const uint8_t>(
                            reinterpret_cast<const uint8_t *>(str.data()),
                            str.size()));

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
