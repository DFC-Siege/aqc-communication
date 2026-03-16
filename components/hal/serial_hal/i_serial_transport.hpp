#pragma once

#include <cstdint>
#include <functional>
#include <span>

namespace Serial {

using ReceiveCallback = std::function<void(std::span<const uint8_t>)>;

class ISerialTransport {
      public:
        virtual ~ISerialTransport() = default;
        virtual void send(std::span<const uint8_t> data) = 0;
        virtual void on_receive(ReceiveCallback cb) = 0;

      protected:
        ReceiveCallback receive_callback;
};

} // namespace Serial
