#include <cstdint>

#include "ble_transporter.hpp"
#include "logger.hpp"
#include "transport/chunked_sender.hpp"

namespace Transport {
BLETransporter::BLETransporter(
    uint16_t mtu, std::unique_ptr<BLE::IBLETransport> ble_transport)
    : ble_transport(std::move(ble_transport)), mtu(mtu) {
        ble_transport->on_receive([this](std::span<const uint8_t> data) {
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
BLETransporter::send(uint8_t command, std::span<const uint8_t> data,
                     ISender::CompleteCallback on_complete,
                     ITransporter::ErrorCallback on_error) {
        const auto session_result = next_sender_session();
        if (session_result.failed()) {
                return Result::err(session_result.error());
        }
        const auto session = session_result.value();

        error_callbacks[session] = on_error;
        senders[session] = std::make_unique<ChunkedSender>(mtu, session);
        const auto &sender = senders[session];
        const auto result = sender->send(
            session, command, data,
            [this](std::span<const uint8_t> data) -> Result::Result<bool> {
                    return this->ble_transport->send(data);
            },
            [on_complete, session, this](uint8_t command) {
                    remove_sender(session);
                    on_complete(command);
            });
        if (result.failed()) {
                return result;
        }

        return Result::ok();
}

Result::Result<bool>
BLETransporter::request(uint8_t command, std::span<const uint8_t> payload,
                        IReceiver::CompleteCallback on_complete,
                        ITransporter::ErrorCallback on_error) {
        return Result::ok();
}
} // namespace Transport
