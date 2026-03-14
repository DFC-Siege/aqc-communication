#pragma once

#include "i_transport.hpp"
#include "result.hpp"
#include <cstdint>

namespace Transport {
class ChunkedTransporter : public ITransporter {
      public:
        ChunkedTransporter();

        Result::Result<FeedResult> feed(std::span<const uint8_t> data) override;

      protected:
        std::unordered_map<uint8_t, ErrorCallback> error_callbacks;

        Result::Result<uint8_t> next_receiver_session();
        Result::Result<uint8_t> next_sender_session();
        void remove_sender(uint8_t session_id);
        void remove_receiver(uint8_t session_id);
};
} // namespace Transport
