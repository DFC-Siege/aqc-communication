#include <memory>

#include "communication_handler.hpp"
#include "transport/ble/ble_transporter.hpp"

namespace Communication {
CommunicationHandler::CommunicationHandler(
    BLE::IBLETransport &ble_transport,
    Serial::ISerialTransport &serial_transport)
    : ble_transporter(Transport::BLETransporter{BLE_MTU, ble_transport}),
      serial_transport(serial_transport) {
}
} // namespace Communication
