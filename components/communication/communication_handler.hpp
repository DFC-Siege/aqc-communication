#pragma once

#include <memory>

#include "ble_transporter.hpp"
#include "i_ble_hal.hpp"
#include "i_serial_hal.hpp"
#include "serial_transporter.hpp"

namespace communication {
class CommunicationHandler {
      public:
        transport::BleTransporter ble_transporter;
        transport::SerialTransporter serial_transporter;

        CommunicationHandler(ble::IBleHal &ble_hal,
                             serial::ISerialHal &serial_hal);

      private:
        static constexpr auto BLE_MTU = 512;
        static constexpr auto SERIAL_MTU = 512;
};
} // namespace communication
