#pragma once

#include <cstdint>
#include <span>

#include "../i_transport.hpp"
#include "future.hpp"
#include "result.hpp"

namespace Transport {
class ChunkedTransporter : public ITransporter {
      public:
        ChunkedTransporter(uint16_t mtu);

        Result::Result<FeedResult> feed(std::span<const uint8_t> data) override;

        Result::Result<bool>
        send(uint8_t command, std::span<const uint8_t> data,
             ISender::CompleteCallback on_complete,
             ITransporter::ErrorCallback on_error) override;

        std::shared_ptr<Future<Result::Result<bool>>>
        send_async(uint8_t command, std::span<const uint8_t> data) override;

        Result::Result<bool> request(uint8_t command,
                                     std::span<const uint8_t> payload,
                                     IReceiver::CompleteCallback on_complete,
                                     ErrorCallback on_error) override;

        std::shared_ptr<Future<Result::Result<bool>>>
        request_async(uint8_t command,
                      std::span<const uint8_t> payload) override;

      protected:
        std::unordered_map<uint8_t, ErrorCallback> error_callbacks;
        uint16_t mtu;

        Result::Result<uint8_t> next_receiver_session();
        Result::Result<uint8_t> next_sender_session();
        void remove_sender(uint8_t session_id);
        void remove_receiver(uint8_t session_id);

        virtual Result::Result<bool>
        concrete_send(std::span<const uint8_t> data) = 0;
};
} // namespace Transport
