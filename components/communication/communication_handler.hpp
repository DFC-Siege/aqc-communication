#pragma once

#include <memory>

#include "ble_transporter.hpp"
#include "i_ble_hal.hpp"
#include "i_serial_hal.hpp"
#include "serial_transporter.hpp"

namespace Communication {
class CommunicationHandler {
      public:
        Transport::BleTransporter ble_transporter;
        Transport::SerialTransporter serial_transporter;

        CommunicationHandler(Ble::IBleHal &ble_hal,
                             Serial::ISerialHal &serial_hal);

      private:
        static constexpr auto BLE_MTU = 512;
        static constexpr auto SERIAL_MTU = 512;
};
} // namespace Communication
