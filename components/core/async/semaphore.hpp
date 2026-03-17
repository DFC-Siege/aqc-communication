#pragma once

#ifdef PLATFORM_FREERTOS
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

namespace async {
class Semaphore {
        SemaphoreHandle_t sem;

      public:
        Semaphore() : sem(xSemaphoreCreateBinary()) {
        }
        ~Semaphore() {
                vSemaphoreDelete(sem);
        }
        Semaphore(const Semaphore &) = delete;
        Semaphore &operator=(const Semaphore &) = delete;

        void give() {
                xSemaphoreGive(sem);
        }
        bool take(uint32_t timeout_ms) {
                return xSemaphoreTake(sem, pdMS_TO_TICKS(timeout_ms)) == pdTRUE;
        }
        bool take() {
                return xSemaphoreTake(sem, portMAX_DELAY) == pdTRUE;
        }
};
} // namespace async

#elif defined(PLATFORM_POSIX)
#include <chrono>
#include <condition_variable>
#include <mutex>

namespace async {
class Semaphore {
        std::mutex mutex;
        std::condition_variable cv;
        bool ready = false;

      public:
        void give() {
                std::unique_lock lock(mutex);
                ready = true;
                cv.notify_one();
        }
        bool take(uint32_t timeout_ms) {
                std::unique_lock lock(mutex);
                const auto result =
                    cv.wait_for(lock, std::chrono::milliseconds(timeout_ms),
                                [this] { return ready; });
                ready = false;
                return result;
        }
        bool take() {
                std::unique_lock lock(mutex);
                cv.wait(lock, [this] { return ready; });
                ready = false;
                return true;
        }
};
} // namespace async
#else
#error "No platform selected, define PLATFORM_FREERTOS or PLATFORM_POSIX"
#endif
