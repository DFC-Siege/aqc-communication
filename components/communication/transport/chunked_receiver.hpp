#pragma once

#include <cstdint>
#include <vector>

#include "i_transport.hpp"
#include "result.hpp"

namespace Transport {
class ChunkedReceiver : public IReceiver {
      public:
        ChunkedReceiver(uint16_t mtu, uint8_t max_attempts);
        Result::Result<bool> start(uint8_t session_id, uint8_t command,
                                   SendCallback sender) override;
        Result::Result<bool> receive(std::span<const uint8_t> data) override;

      private:
        std::vector<Chunk> received_chunks;
        uint16_t mtu;
        uint16_t current_index = 0;
        uint8_t max_attempts;
        uint8_t current_attempt = 0;
        SendCallback sender;

        Result::Result<std::vector<uint8_t>>
        reconstruct_data(std::span<const Chunk> chunks) const;
};
} // namespace Transport
