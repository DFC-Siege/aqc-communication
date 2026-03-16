#pragma once

#include <cstdint>
#include <esp_crc.h>
#include <functional>
#include <memory>
#include <set>
#include <span>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "result.hpp"

namespace Transport {
using SendCallback =
    std::function<Result::Result<bool>(std::span<const uint8_t>)>;

enum class PacketType : uint8_t {
        chunk = 0x01,
        ack = 0x02,
};

struct Ack {
        static constexpr uint8_t SESSION_ID_OFFSET = 3;
        uint16_t index;
        uint8_t session_id;
        bool success;

        std::vector<uint8_t> to_buf() const {
                return {static_cast<uint8_t>(PacketType::ack),
                        static_cast<uint8_t>(index & 0xFF),
                        static_cast<uint8_t>((index >> 8) & 0xFF), session_id,
                        static_cast<uint8_t>(success)};
        }

        static Result::Result<Ack> from_buf(std::span<const uint8_t> buf) {
                if (buf.size() < 5)
                        return Result::err("buffer too small");
                if (static_cast<PacketType>(buf[0]) != PacketType::ack)
                        return Result::err("invalid packet type");
                return Result::ok(
                    Ack{static_cast<uint16_t>(buf[1] | (buf[2] << 8)), buf[3],
                        static_cast<bool>(buf[4])});
        }
};

struct Chunk {
        static constexpr auto HEADER_SIZE = 9;
        static constexpr uint8_t SESSION_ID_OFFSET = 7;
        std::vector<uint8_t> payload;
        uint16_t index;
        uint16_t total_chunks;
        uint16_t checksum;
        uint8_t session_id;
        uint8_t command;

        std::vector<uint8_t> to_buf() const {
                std::vector<uint8_t> buf;
                buf.reserve(1 + sizeof(index) + sizeof(total_chunks) +
                            sizeof(checksum) + sizeof(session_id) +
                            sizeof(command) + payload.size());
                buf.push_back(static_cast<uint8_t>(PacketType::chunk));
                auto push16 = [&](uint16_t val) {
                        buf.push_back(val & 0xFF);
                        buf.push_back((val >> 8) & 0xFF);
                };
                push16(index);
                push16(total_chunks);
                push16(checksum);
                buf.push_back(session_id);
                buf.push_back(command);
                buf.insert(buf.end(), payload.begin(), payload.end());
                return buf;
        }

        static Result::Result<Chunk> from_buf(std::span<const uint8_t> buf) {
                if (buf.size() < HEADER_SIZE)
                        return Result::err("buffer too small");
                if (static_cast<PacketType>(buf[0]) != PacketType::chunk)
                        return Result::err("invalid packet type");

                auto pull16 = [&](size_t offset) -> uint16_t {
                        return static_cast<uint16_t>(buf[offset] |
                                                     (buf[offset + 1] << 8));
                };

                Chunk chunk;
                chunk.index = pull16(1);
                chunk.total_chunks = pull16(3);
                chunk.checksum = pull16(5);
                chunk.session_id = buf[7];
                chunk.command = buf[8];
                chunk.payload =
                    std::vector<uint8_t>(buf.begin() + HEADER_SIZE, buf.end());

                const auto computed =
                    esp_crc16_le(0, chunk.payload.data(), chunk.payload.size());
                if (computed != chunk.checksum)
                        return Result::err("checksum mismatch");

                return Result::ok(std::move(chunk));
        }
};

class ISender {
      public:
        using CompleteCallback = std::function<void()>;

        virtual ~ISender() = default;
        virtual Result::Result<bool> send(uint8_t session_id, uint8_t command,
                                          std::span<const uint8_t> data,
                                          SendCallback sender,
                                          CompleteCallback on_complete) = 0;
        virtual Result::Result<bool> receive(std::span<const uint8_t> data) = 0;

      protected:
        uint8_t session_id;
        uint8_t command;
        SendCallback sender;
        CompleteCallback on_complete;
};

class IReceiver {
      public:
        using CompleteCallback =
            std::function<void(std::vector<uint8_t> result)>;

        virtual ~IReceiver() = default;
        virtual Result::Result<bool> start(uint8_t session_id, uint8_t command,
                                           std::span<const uint8_t> payload,
                                           SendCallback sender,
                                           CompleteCallback on_complete) = 0;
        virtual Result::Result<bool> receive(std::span<const uint8_t> data) = 0;

      protected:
        uint8_t session_id;
        uint8_t command;
        SendCallback sender;
        CompleteCallback on_complete;
};

class ITransporter {
      public:
        using FeedResult = std::pair<uint8_t, Result::Result<bool>>;
        using ErrorCallback = std::function<void(std::string_view error)>;

        virtual ~ITransporter() = default;
        virtual Result::Result<bool> send(uint8_t command,
                                          std::span<const uint8_t> data,
                                          ISender::CompleteCallback on_complete,
                                          ErrorCallback on_error) = 0;
        virtual Result::Result<bool>
        request(uint8_t command, std::span<const uint8_t> payload,
                IReceiver::CompleteCallback on_complete,
                ErrorCallback on_error) = 0;

        virtual Result::Result<FeedResult>
        feed(std::span<const uint8_t> raw) = 0;

      protected:
        std::unordered_map<uint8_t, std::unique_ptr<ISender>> senders;
        std::unordered_map<uint8_t, std::unique_ptr<IReceiver>> receivers;
        std::set<uint8_t> available_sender_sessions;
        std::set<uint8_t> available_receiver_sessions;
        uint8_t next_session_id = 0;
};
} // namespace Transport
