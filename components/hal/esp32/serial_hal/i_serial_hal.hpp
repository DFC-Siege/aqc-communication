#pragma once

#include "result.hpp"
#include <cstdint>
#include <functional>
#include <span>

namespace Serial {

using ReceiveCallback = std::function<void(std::span<const uint8_t>)>;

class ISerialHal {
      public:
        virtual ~ISerialHal() = default;
        virtual Result::Result<bool> send(std::span<const uint8_t> data) = 0;
        virtual void on_receive(ReceiveCallback cb) = 0;
        virtual Result::Result<bool> loop() = 0;

      protected:
        ReceiveCallback receive_callback;
};

} // namespace Serial
