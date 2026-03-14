#pragma once

#include <cstdint>
#include <memory>

#include "ble/i_ble_transport.hpp"
#include "chunked_transporter.hpp"
#include "i_transport.hpp"
#include "result.hpp"

namespace Transport {
class BLETransporter : ChunkedTransporter {
      public:
        BLETransporter(uint16_t mtu,
                       std::unique_ptr<BLE::IBLETransport> ble_transport);
        Result::Result<bool>
        send(uint8_t command, std::span<const uint8_t> data,
             ISender::CompleteCallback on_complete,
             ITransporter::ErrorCallback on_error) override;

        Result::Result<bool>
        request(uint8_t command, std::span<const uint8_t> payload,
                IReceiver::CompleteCallback on_complete,
                ITransporter::ErrorCallback on_error) override;

      private:
        static constexpr auto TAG = "BLETransporter";
        std::unique_ptr<BLE::IBLETransport> ble_transport;
        uint16_t mtu;
};
} // namespace Transport
