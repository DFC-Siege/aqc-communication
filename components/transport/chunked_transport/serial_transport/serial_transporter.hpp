#pragma once

#include <cstdint>
#include <memory>

#include "chunked_transporter.hpp"
#include "i_serial_hal.hpp"
#include "i_transport.hpp"
#include "result.hpp"

namespace Transport {
class SerialTransporter : public ChunkedTransporter {
      public:
        SerialTransporter(uint16_t mtu, Serial::ISerialHal &serial_hal);

        Result::Result<bool>
        concrete_send(std::span<const uint8_t> data) override;

      private:
        static constexpr auto TAG = "SerialTransporter";
        Serial::ISerialHal &serial_hal;
};
} // namespace Transport
