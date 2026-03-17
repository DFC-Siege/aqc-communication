#pragma once

#include <cstdint>
#include <memory>

#include "semaphore.hpp"

namespace async {
template <typename T> class Future {
        Semaphore sem;
        T value;

      public:
        Future() = default;
        Future(const Future &) = delete;
        Future &operator=(const Future &) = delete;

        void set_value(T val) {
                value = std::move(val);
                sem.give();
        }

        T get() {
                sem.take();
                return std::move(value);
        }

        bool wait_for(uint32_t timeout_ms) {
                return sem.take(timeout_ms);
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
} // namespace async
