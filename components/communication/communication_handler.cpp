#include <memory>

#include "ble_transporter.hpp"
#include "communication_handler.hpp"
#include "serial_transport/serial_transporter.hpp"

namespace Communication {
CommunicationHandler::CommunicationHandler(
    Ble::IBleTransport &ble_transport,
    Serial::ISerialTransport &serial_transport)
    : ble_transporter(Transport::BleTransporter{BLE_MTU, ble_transport}),
      serial_transporter(
          Transport::SerialTransporter(SERIAL_MTU, serial_transport)) {
}
} // namespace Communication
