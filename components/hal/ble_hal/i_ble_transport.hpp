#pragma once

#include <cstdint>
#include <functional>
#include <span>

#include "result.hpp"

namespace Ble {

using ReceiveCallback = std::function<void(std::span<const uint8_t>)>;
using ConnectionCallback = std::function<void(bool connected)>;

class IBleTransport {
      public:
        virtual ~IBleTransport() = default;
        virtual Result::Result<bool> send(std::span<const uint8_t> data) = 0;
        virtual void on_receive(ReceiveCallback cb) = 0;
        virtual void on_connection_changed(ConnectionCallback cb) = 0;
        virtual bool is_connected() const = 0;

      protected:
        ReceiveCallback receive_callback;
        ConnectionCallback connection_callback;
};

} // namespace Ble
