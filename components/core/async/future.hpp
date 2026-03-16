#pragma once

#include <algorithm>
#include <cstdint>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <memory>

template <typename T> class Future {
        SemaphoreHandle_t sem;
        T value;
        bool has_value = false;

      public:
        Future() : sem(xSemaphoreCreateBinary()) {
        }
        ~Future() {
                vSemaphoreDelete(sem);
        }

        Future(const Future &) = delete;
        Future &operator=(const Future &) = delete;

        void set_value(T val) {
                value = std::move(val);
                has_value = true;
                xSemaphoreGive(sem);
        }

        T get() {
                xSemaphoreTake(sem, portMAX_DELAY);
                return std::move(value);
        }

        bool wait_for(uint32_t timeout_ms) {
                return xSemaphoreTake(sem, pdMS_TO_TICKS(timeout_ms)) == pdTRUE;
        }

        bool is_ready() {
                return wait_for(0);
        }
};

template <typename T> class Promise {
        std::shared_ptr<Future<T>> future;

      public:
        Promise() : future(std::make_shared<Future<T>>()) {
        }

        std::shared_ptr<Future<T>> get_future() {
                return future;
        }

        void set_value(T val) {
                future->set_value(std::move(val));
        }
};
