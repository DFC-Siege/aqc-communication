#pragma once

#include <cstdint>
#include <functional>
#include <span>

namespace BLE {

using ReceiveCallback = std::function<void(std::span<const uint8_t>)>;
using ConnectionCallback = std::function<void(bool connected)>;

class IBLETransport {
      public:
        virtual ~IBLETransport() = default;
        virtual void send(std::span<const uint8_t> data) = 0;
        virtual void on_receive(ReceiveCallback cb) = 0;
        virtual void on_connection_changed(ConnectionCallback cb) = 0;
        virtual bool is_connected() const = 0;
};

} // namespace BLE
