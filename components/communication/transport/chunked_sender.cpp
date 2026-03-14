#include <cstdint>

#include "chunked_sender.hpp"
#include "result.hpp"
#include "transport/i_transport.hpp"

namespace Transport {
ChunkedSender::ChunkedSender(uint16_t mtu) : mtu(mtu) {
}

Result::Result<bool> ChunkedSender::send(uint8_t command,
                                         std::span<const uint8_t> data,
                                         SendCallback sender,
                                         ReceiveCallback receiver) {
        this->sender = sender;
        const auto result = create_chunks(command, data);
        if (result.failed()) {
                return Result::err(result.error());
        }
        chunked_data = result.value();

        const auto chunk_result = repeat();
        if (chunk_result.failed()) {
                return Result::err(chunk_result.error());
        }
        const auto chunk = chunk_result.value();
        return this->sender(chunk.to_buf());
}

Result::Result<bool> ChunkedSender::receive(std::span<const uint8_t> data) {
        assert(sender != nullptr && "sender should not be null");
        const auto result = Ack::from_buf(data);
        if (result.failed()) {
                return Result::err(result.error());
        }

        const auto ack = result.value();
        if (!ack.success) {
                const auto repeat_result = repeat();
                if (repeat_result.failed()) {
                        return Result::err(repeat_result.error());
                }
                const auto chunk = repeat_result.value();
                return sender(chunk.to_buf());
        }

        const auto next_result = get_next();
        if (next_result.failed()) {
                return Result::err(next_result.error());
        }

        const auto chunk = next_result.value();
        return sender(chunk.to_buf());
}

Result::Result<std::vector<Chunk>>
ChunkedSender::create_chunks(uint8_t command,
                             std::span<const uint8_t> data) const {
        return Result::ok(std::vector<Chunk>{});
}

Result::Result<Chunk> ChunkedSender::get_next() {
        if (++current_index >= chunked_data.size()) {
                return Result::err("index out of bounds getting next");
        }

        return Result::ok(chunked_data.at(current_index));
}

Result::Result<Chunk> ChunkedSender::repeat() const {
        if (current_index >= chunked_data.size()) {
                return Result::err("index out of bound repeating");
        }

        return Result::ok(chunked_data.at(current_index));
}
} // namespace Transport
