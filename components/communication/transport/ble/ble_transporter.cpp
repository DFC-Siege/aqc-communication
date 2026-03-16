#include <cstdint>

#include "logger.hpp"
#include "result.hpp"
#include "transport/ble/ble_transporter.hpp"
#include "transport/chunked/chunked_sender.hpp"
#include "transport/chunked/chunked_transporter.hpp"

namespace Transport {
BLETransporter::BLETransporter(uint16_t mtu, BLE::IBLETransport &ble_transport)
    : ChunkedTransporter(mtu), ble_transport(ble_transport) {
        ble_transport.on_receive([this](std::span<const uint8_t> data) {
                const auto result = feed(data);
                if (result.failed()) {
                        Logging::logger().println(Logging::LogLevel::Error, TAG,
                                                  result.error());
                        return;
                }
                const auto feed_result = result.value();
                if (feed_result.second.failed() &&
                    error_callbacks.find(feed_result.first) !=
                        error_callbacks.end()) {
                        error_callbacks[feed_result.first](
                            feed_result.second.error());
                }
        });
}

Result::Result<bool>
BLETransporter::concrete_send(std::span<const uint8_t> data) {
        return this->ble_transport.send(data);
}

Result::Result<bool>
BLETransporter::request(uint8_t command, std::span<const uint8_t> payload,
                        IReceiver::CompleteCallback on_complete,
                        ITransporter::ErrorCallback on_error) {
        return Result::err("not implemented");
}
} // namespace Transport
