#include <memory>

#include "ble_transporter.hpp"
#include "communication_handler.hpp"

namespace Communication {
CommunicationHandler::CommunicationHandler(
    BLE::IBLETransport &ble_transport,
    Serial::ISerialTransport &serial_transport)
    : ble_transporter(Transport::BLETransporter{BLE_MTU, ble_transport}),
      serial_transport(serial_transport) {
}
} // namespace Communication
