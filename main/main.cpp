#include <cstdint>
#include <cstdio>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <memory>
#include <nvs_flash.h>
#include <string_view>
#include <thread>

#include "ble_hal.hpp"
#include "communication_handler.hpp"
#include "console_logger.hpp"
#include "http_client.hpp"
#include "i_http_client.hpp"
#include "logger.hpp"
#include "nvs_store.hpp"
#include "result.hpp"
#include "serial_hal.hpp"
#include "serial_transporter.hpp"

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
        transport::SerialTransporter serial_transporter(32, 3, serial_hal);
        serial_hal.on_receive([](std::span<const uint8_t> data) {
                logging::logger().println(
                    "serial", std::string_view(
                                  reinterpret_cast<const char *>(data.data()),
                                  data.size()));
        });

        std::thread([&] {
                while (true) {
                        serial_hal.loop();
                        vTaskDelay(1);
                }
        }).detach();

        while (true) {
                const auto result =
                    serial_transporter
                        .request_async(0x0, std::vector<uint8_t>())
                        ->get();
                if (result.failed()) {
                        logging::logger().print_fmt("error: {}",
                                                    result.error());
                        continue;
                }

                vTaskDelay(1);
        }
}
}
