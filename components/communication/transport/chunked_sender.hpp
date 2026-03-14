#pragma once

#include <cstdint>
#include <vector>

#include "i_transport.hpp"
#include "result.hpp"

namespace Transport {
class ChunkedSender : public ISender {
      public:
        ChunkedSender(uint16_t mtu);
        Result::Result<bool> send(uint8_t command,
                                  std::span<const uint8_t> data,
                                  SendCallback sender,
                                  ReceiveCallback receiver) override;
        Result::Result<bool> receive(std::span<const uint8_t> data) override;

      private:
        std::vector<Chunk> chunked_data;
        uint16_t mtu;
        uint16_t current_index = 0;
        SendCallback sender;
        ReceiveCallback receiver;

        Result::Result<std::vector<Chunk>>
        create_chunks(uint8_t command, std::span<const uint8_t> data) const;
        Result::Result<Chunk> get_next();
        Result::Result<Chunk> repeat() const;
};
} // namespace Transport
