#include <cstdio>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string>

#include "serial/serial_manager.hpp"

extern "C" {
void app_main(void) {
        Communication::SerialManager serial_manager;

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
                }

                vTaskDelay(1);
        }
}
}
