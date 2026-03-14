#include <cstdint>
#include <esp_crc.h>
#include <vector>

#include "chunked_receiver.hpp"
#include "transport/i_transport.hpp"

namespace Transport {
ChunkedReceiver::ChunkedReceiver(uint16_t mtu, uint8_t max_attempts)
    : mtu(mtu), max_attempts(max_attempts) {
}

Result::Result<bool> ChunkedReceiver::start(uint8_t session_id, uint8_t command,
                                            std::span<const uint8_t> payload,
                                            SendCallback sender) {
        if (payload.size() + Chunk::HEADER_SIZE > mtu) {
                return Result::err("payload is too large");
        }

        this->session_id = session_id;
        this->command = command;
        this->sender = sender;

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
                return nack();
        }
        const auto chunk = chunk_result.value();
        received_chunks.push_back(chunk);

        return Result::ok();
}

Result::Result<std::vector<uint8_t>>
ChunkedReceiver::reconstruct_data(std::span<const Chunk> chunks) const {
        return Result::ok(std::vector<uint8_t>{});
}
} // namespace Transport
