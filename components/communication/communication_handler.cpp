#include <memory>

#include "communication_handler.hpp"

namespace Communication {
CommunicationHandler::CommunicationHandler(
    std::unique_ptr<BLE::IBLETransport> ble_transport,
    std::unique_ptr<Serial::ISerialTransport> serial_transport)
    : ble_transport(std::move(ble_transport)),
      serial_transport(std::move(serial_transport)) {
}
} // namespace Communication
