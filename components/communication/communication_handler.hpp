#pragma once

#include <memory>

#include "ble/i_ble_transport.hpp"
#include "serial/i_serial_transport.hpp"
#include "transport/ble_transporter.hpp"

namespace Communication {
class CommunicationHandler {
      public:
        CommunicationHandler(
            std::unique_ptr<BLE::IBLETransport> ble_transport,
            std::unique_ptr<Serial::ISerialTransport> serial_transport);

      private:
        Transport::BLETransporter ble_transporter;
        std::unique_ptr<Serial::ISerialTransport> serial_transport;
};
} // namespace Communication
