#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "i_serial_transport.hpp"

namespace Serial {
class SerialManager : public ISerialTransport {
      public:
        SerialManager();
        void send(std::span<const uint8_t> data) override;
        void on_receive(ReceiveCallback cb) override {
                receive_callback = std::move(cb);
        }
        void loop();

      private:
        static constexpr auto BAUDRATE = 115200;
        static constexpr auto TX_PIN = 7;
        static constexpr auto RX_PIN = 6;
        static constexpr auto BUF_SIZE = 1024;
        ReceiveCallback receive_callback;
};
} // namespace Serial
