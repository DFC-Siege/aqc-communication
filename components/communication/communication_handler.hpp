#pragma once

#include <memory>

#include "ble/i_ble_transport.hpp"
#include "serial/i_serial_transport.hpp"
#include "transport/ble_transporter.hpp"

namespace Communication {
class CommunicationHandler {
      public:
        Transport::BLETransporter ble_transporter;
        Serial::ISerialTransport &serial_transport;

        CommunicationHandler(BLE::IBLETransport &ble_transport,
                             Serial::ISerialTransport &serial_transport);

      private:
        static constexpr auto BLE_MTU = 517;
};
} // namespace Communication
