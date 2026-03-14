#pragma once

#include <cstdint>

#include "chunked_transporter.hpp"

namespace Transport {
ChunkedTransporter::ChunkedTransporter() {
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
