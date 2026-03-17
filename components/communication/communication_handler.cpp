#include <memory>

#include "ble_transporter.hpp"
#include "communication_handler.hpp"
#include "serial_transport/serial_transporter.hpp"

namespace Communication {
CommunicationHandler::CommunicationHandler(Ble::IBleHal &ble_hal,
                                           Serial::ISerialHal &serial_hal)
    : ble_transporter(transport::BleTransporter{BLE_MTU, ble_hal}),
      serial_transporter(transport::SerialTransporter(SERIAL_MTU, serial_hal)) {
}
} // namespace Communication
