#include "chunked_receiver.hpp"
#include <vector>

namespace Transport {
ChunkedReceiver::ChunkedReceiver(uint16_t mtu, uint8_t max_attempts)
    : mtu(mtu), max_attempts(max_attempts) {
}

Result::Result<bool> ChunkedReceiver::start(uint8_t session_id, uint8_t command,
                                            SendCallback sender) {
        return Result::ok();
}

Result::Result<bool> ChunkedReceiver::receive(std::span<const uint8_t> data) {
        return Result::ok();
}

Result::Result<std::vector<uint8_t>>
ChunkedReceiver::reconstruct_data(std::span<const Chunk> chunks) const {
        return Result::ok(std::vector<uint8_t>{});
}
} // namespace Transport
