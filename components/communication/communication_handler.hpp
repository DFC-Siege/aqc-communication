#pragma once

#include <memory>

#include "ble_transporter.hpp"
#include "i_ble_transport.hpp"
#include "i_serial_transport.hpp"
#include "serial_transporter.hpp"

namespace Communication {
class CommunicationHandler {
      public:
        Transport::BleTransporter ble_transporter;
        Transport::SerialTransporter serial_transporter;

        CommunicationHandler(Ble::IBleTransport &ble_transport,
                             Serial::ISerialTransport &serial_transport);

      private:
        static constexpr auto BLE_MTU = 512;
        static constexpr auto SERIAL_MTU = 512;
};
} // namespace Communication
