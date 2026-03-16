#include <cstdint>
#include <span>

#include "chunked_receiver.hpp"
#include "chunked_sender.hpp"
#include "chunked_transporter.hpp"
#include "packet.hpp"
#include "result.hpp"

namespace Transport {
ChunkedTransporter::ChunkedTransporter(uint16_t mtu) : mtu(mtu) {
        for (uint8_t i = 0; i < 255; i++) {
                available_sender_sessions.insert(i);
                available_receiver_sessions.insert(i);
        }
}

Result::Result<ChunkedTransporter::FeedResult>
ChunkedTransporter::feed(std::span<const uint8_t> data) {
        if (data.empty())
                return Result::err("data is empty");

        switch (static_cast<PacketType>(data[0])) {
        case PacketType::chunk: {
                if (data.size() < Chunk::HEADER_SIZE)
                        return Result::err("buffer too small");
                const uint8_t session_id = data[Chunk::SESSION_ID_OFFSET];
                auto it = receivers.find(session_id);
                if (it == receivers.end())
                        return Result::err("unknown session");
                return Result::ok(
                    std::make_pair(session_id, it->second->receive(data)));
        }
        case PacketType::ack: {
                if (data.size() < 5)
                        return Result::err("buffer too small");
                const uint8_t session_id = data[Ack::SESSION_ID_OFFSET];
                auto it = senders.find(session_id);
                if (it == senders.end())
                        return Result::err("unknown session");
                return Result::ok(
                    std::make_pair(session_id, it->second->receive(data)));
        }
        default:
                return Result::err("unknown packet type");
        }
}

Result::Result<bool>
ChunkedTransporter::send(uint8_t command, std::span<const uint8_t> data,
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
                    return this->concrete_send(data);
            },
            [on_complete, session, this]() {
                    remove_sender(session);
                    on_complete();
            });
        if (result.failed()) {
                return result;
        }

        return Result::ok();
}

std::shared_ptr<Future<Result::Result<bool>>>
ChunkedTransporter::send_async(uint8_t command, std::span<const uint8_t> data) {
        auto promise = std::make_shared<Promise<Result::Result<bool>>>();
        auto future = promise->get_future();

        send(
            command, data, [promise]() { promise->set_value(Result::ok()); },
            [promise](std::string_view err) {
                    promise->set_value(Result::err(err));
            });

        return future;
}

Result::Result<bool>
ChunkedTransporter::request(uint8_t command, std::span<const uint8_t> payload,
                            IReceiver::CompleteCallback on_complete,
                            ErrorCallback on_error) {
        const auto session_result = next_receiver_session();
        if (session_result.failed()) {
                return Result::err(session_result.error());
        }
        const auto session = session_result.value();

        error_callbacks[session] = on_error;
        receivers[session] = std::make_unique<ChunkedReceiver>(mtu, session);
        const auto &receiver = receivers[session];
        const auto result = receiver->start(
            session, command, payload,
            [this](std::span<const uint8_t> data) -> Result::Result<bool> {
                    return this->concrete_send(data);
            },
            [on_complete, session, this](std::vector<uint8_t> data) {
                    remove_receiver(session);
                    on_complete(data);
            });
        if (result.failed()) {
                return result;
        }

        return Result::ok();
}

std::shared_ptr<Future<Result::Result<std::vector<uint8_t>>>>
ChunkedTransporter::request_async(uint8_t command,
                                  std::span<const uint8_t> payload) {
        auto promise =
            std::make_shared<Promise<Result::Result<std::vector<uint8_t>>>>();
        auto future = promise->get_future();

        request(
            command, payload,
            [promise](std::vector<uint8_t> data) {
                    promise->set_value(Result::ok(data));
            },
            [promise](std::string_view err) {
                    promise->set_value(Result::err(err));
            });

        return future;
}

Result::Result<uint8_t> ChunkedTransporter::next_receiver_session() {
        if (available_receiver_sessions.empty()) {
                return Result::err("no sessions available for sending");
        }
        const auto id = *available_receiver_sessions.begin();
        available_receiver_sessions.erase(available_receiver_sessions.begin());
        return Result::ok(id);
}

Result::Result<uint8_t> ChunkedTransporter::next_sender_session() {
        if (available_sender_sessions.empty()) {
                return Result::err("no sessions available for sending");
        }
        const auto id = *available_sender_sessions.begin();
        available_sender_sessions.erase(available_sender_sessions.begin());
        return Result::ok(id);
}

void ChunkedTransporter::remove_sender(uint8_t session_id) {
        senders.erase(session_id);
        error_callbacks.erase(session_id);
        available_sender_sessions.insert(session_id);
}

void ChunkedTransporter::remove_receiver(uint8_t session_id) {
        receivers.erase(session_id);
        available_receiver_sessions.insert(session_id);
}
} // namespace Transport
