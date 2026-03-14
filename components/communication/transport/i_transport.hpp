#pragma once

#include "result.hpp"
#include <cstdint>
#include <functional>
#include <memory>
#include <span>
#include <string_view>
#include <vector>

namespace Transport {
using ReceiveCallback =
    std::function<Result::Result<bool>(std::span<const uint8_t>)>;
using SendCallback =
    std::function<Result::Result<bool>(std::span<const uint8_t>)>;

struct Ack {
        uint16_t index;
        uint8_t session_id;
        bool success;

        std::vector<uint8_t> to_buf() const {
                return {static_cast<uint8_t>(index & 0xFF),
                        static_cast<uint8_t>((index >> 8) & 0xFF), session_id,
                        static_cast<uint8_t>(success)};
        }

        static Result::Result<Ack> from_buf(std::span<const uint8_t> buf) {
                if (buf.size() < 4)
                        return Result::err("buffer too small");
                return Result::ok(
                    Ack{static_cast<uint16_t>(buf[0] | (buf[1] << 8)), buf[2],
                        static_cast<bool>(buf[3])});
        }
};

struct Chunk {
        static constexpr auto HEADER_SIZE = 8;
        std::vector<uint8_t> payload;
        uint16_t index;
        uint16_t total_chunks;
        uint16_t checksum;
        uint8_t session_id;
        uint8_t command;

        std::vector<uint8_t> to_buf() const {
                std::vector<uint8_t> buf;
                buf.reserve(sizeof(index) + sizeof(total_chunks) +
                            sizeof(checksum) + sizeof(session_id) +
                            sizeof(command) + payload.size());
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
                if (buf.size() < HEADER_SIZE) {
                        return Result::err("buffer too small");
                }

                auto pull16 = [&](size_t offset) -> uint16_t {
                        return static_cast<uint16_t>(buf[offset] |
                                                     (buf[offset + 1] << 8));
                };
                Chunk chunk;
                chunk.index = pull16(0);
                chunk.total_chunks = pull16(2);
                chunk.checksum = pull16(4);
                chunk.session_id = buf[6];
                chunk.command = buf[7];
                chunk.payload =
                    std::vector<uint8_t>(buf.begin() + HEADER_SIZE, buf.end());
                return Result::ok(std::move(chunk));
        }
};

class ISender {
      public:
        virtual ~ISender() = default;

        virtual Result::Result<bool> send(uint8_t session_id, uint8_t command,
                                          std::span<const uint8_t> data,
                                          SendCallback send_chunk,
                                          ReceiveCallback on_ack) = 0;
        virtual Result::Result<bool> receive(std::span<const uint8_t> data) = 0;

      protected:
        uint8_t session_id;
        uint8_t command;
};

class IReceiver {
      public:
        virtual ~IReceiver() = default;
};

class ITransporter {
      public:
        ~ITransporter() = default;

      protected:
        std::unique_ptr<ISender> sender;
        std::unique_ptr<IReceiver> receiver;
};
} // namespace Transport
