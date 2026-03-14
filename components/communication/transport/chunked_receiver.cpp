#include <cstdint>
#include <esp_crc.h>
#include <vector>

#include "chunked_receiver.hpp"
#include "result.hpp"
#include "transport/i_transport.hpp"

namespace Transport {
ChunkedReceiver::ChunkedReceiver(uint16_t mtu, uint8_t max_attempts)
    : mtu(mtu), max_attempts(max_attempts) {
}

Result::Result<bool>
ChunkedReceiver::start(uint8_t session_id, uint8_t command,
                       std::span<const uint8_t> payload, SendCallback sender,
                       IReceiver::CompleteCallback on_complete) {
        if (payload.size() + Chunk::HEADER_SIZE > mtu) {
                return Result::err("payload is too large");
        }

        this->session_id = session_id;
        this->command = command;
        this->sender = sender;
        this->on_complete = on_complete;
        received_chunks.clear();

        const auto checksum = esp_crc16_le(0, payload.data(), payload.size());
        const auto chunk =
            Chunk{std::vector<uint8_t>{payload.begin(), payload.end()},
                  0,
                  1,
                  checksum,
                  this->session_id,
                  this->command};

        return this->sender(chunk.to_buf());
}

Result::Result<bool> ChunkedReceiver::receive(std::span<const uint8_t> data) {
        const auto chunk_result = Chunk::from_buf(data);
        if (chunk_result.failed()) {
                return ack(false);
        }
        const auto chunk = chunk_result.value();
        received_chunks.push_back(chunk);

        if (chunk.index >= chunk.total_chunks - 1) {
                on_complete(command, reconstruct_data(received_chunks));
        }

        return ack(true);
}

std::vector<uint8_t>
ChunkedReceiver::reconstruct_data(std::vector<Chunk> chunks) const {
        std::sort(
            chunks.begin(), chunks.end(),
            [](const Chunk &a, const Chunk &b) { return a.index < b.index; });

        std::vector<uint8_t> reconstruct_data;
        for (const auto &chunk : chunks) {
                reconstruct_data.insert(reconstruct_data.end(),
                                        chunk.payload.begin(),
                                        chunk.payload.end());
        }

        return reconstruct_data;
}

Result::Result<bool> ChunkedReceiver::ack(bool success) {
        if (!success && ++current_attempt > max_attempts) {
                return Result::err("max attempts reached");
        } else if (success) {
                current_attempt = 0;
        }

        const uint16_t index =
            received_chunks.empty()
                ? 0
                : static_cast<uint16_t>(received_chunks.size() - 1);
        const auto ack = Ack{
            index,
            session_id,
            success,
        };
        return this->sender(ack.to_buf());
}
} // namespace Transport
