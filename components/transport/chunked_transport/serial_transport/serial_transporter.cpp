#include <cstdint>

#include "chunked_sender.hpp"
#include "chunked_transporter.hpp"
#include "logger.hpp"
#include "result.hpp"
#include "serial_transporter.hpp"

namespace Transport {
SerialTransporter::SerialTransporter(uint16_t mtu,
                                     Serial::ISerialTransport &serial_transport)
    : ChunkedTransporter(mtu), serial_transport(serial_transport) {
        serial_transport.on_receive([this](std::span<const uint8_t> data) {
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
SerialTransporter::concrete_send(std::span<const uint8_t> data) {
        return this->serial_transport.send(data);
}
} // namespace Transport
