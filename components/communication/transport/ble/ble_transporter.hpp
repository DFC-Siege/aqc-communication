#pragma once

#include <cstdint>
#include <memory>

#include "ble/i_ble_transport.hpp"
#include "result.hpp"
#include "transport/chunked/chunked_transporter.hpp"
#include "transport/i_transport.hpp"

namespace Transport {
class BLETransporter : public ChunkedTransporter {
      public:
        BLETransporter(uint16_t mtu, BLE::IBLETransport &ble_transport);

        Result::Result<bool>
        concrete_send(std::span<const uint8_t> data) override;

        Result::Result<bool>
        request(uint8_t command, std::span<const uint8_t> payload,
                IReceiver::CompleteCallback on_complete,
                ITransporter::ErrorCallback on_error) override;

      private:
        static constexpr auto TAG = "BLETransporter";
        BLE::IBLETransport &ble_transport;
};
} // namespace Transport
