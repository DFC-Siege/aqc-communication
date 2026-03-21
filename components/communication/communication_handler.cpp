#include <memory>

#include "ble_transporter.hpp"
#include "communication_handler.hpp"
#include "serial_transport/serial_transporter.hpp"

namespace communication {
CommunicationHandler::CommunicationHandler(ble::IBleHal &ble_hal,
                                           serial::ISerialHal &serial_hal)
    : ble_transporter(
          transport::BleTransporter{BLE_MTU, MAX_ATTEMPTS, ble_hal}),
      serial_transporter(
          transport::SerialTransporter(SERIAL_MTU, MAX_ATTEMPTS, serial_hal)) {
}
} // namespace communication
