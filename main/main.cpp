#include <cstdint>
#include <cstdio>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <memory>
#include <nvs_flash.h>
#include <string_view>

#include "ble_hal.hpp"
#include "communication_handler.hpp"
#include "console_logger.hpp"
#include "http_client.hpp"
#include "i_http_client.hpp"
#include "logger.hpp"
#include "nvs_store.hpp"
#include "result.hpp"
#include "serial_hal.hpp"

extern "C" {
void app_main(void) {
        logging::Logger::set(std::make_unique<logging::ConsoleLogger>());

        static constexpr auto DEFAULT_NAMESPACE = "default";
        auto store = store::kv::NvsStore::init(DEFAULT_NAMESPACE);

        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
            ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
                nvs_flash_erase();
                nvs_flash_init();
        }

        auto &ble = ble::BleHal::instance();
        ble.on_connection_changed([](bool connected) {
                logging::logger().println_fmt(
                    "Ble {}", connected ? "connected" : "disconnected");
        });
        ble.begin("aqc");

        serial::SerialHal serial_hal;
        serial_hal.on_receive([](std::span<const uint8_t> data) {
                logging::logger().println(
                    "serial", std::string_view(
                                  reinterpret_cast<const char *>(data.data()),
                                  data.size()));
        });

        uint32_t counter = 0;
        TickType_t last_wake_time = xTaskGetTickCount();
        const TickType_t interval = pdMS_TO_TICKS(1000);

        while (true) {
                serial_hal.loop();

                TickType_t current_time = xTaskGetTickCount();
                if (current_time - last_wake_time >= interval) {
                        last_wake_time = current_time;
                        const auto str = std::to_string(counter++);
                        logging::logger().println_fmt("count: {}", str);

                        const auto span = std::span<const uint8_t>(
                            reinterpret_cast<const uint8_t *>(str.data()),
                            str.size());
                }

                vTaskDelay(1);
        }
}
}
