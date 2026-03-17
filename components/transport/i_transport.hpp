#pragma once

#include <cstdint>
#include <esp_crc.h>
#include <functional>
#include <future>
#include <memory>
#include <set>
#include <span>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "future.hpp"
#include "result.hpp"

namespace transport {
using SendCallback =
    std::function<result::Result<bool>(std::span<const uint8_t>)>;

class ISender {
      public:
        using CompleteCallback = std::function<void()>;

        virtual ~ISender() = default;
        virtual result::Result<bool> send(uint8_t session_id, uint8_t command,
                                          std::span<const uint8_t> data,
                                          SendCallback sender,
                                          CompleteCallback on_complete) = 0;
        virtual result::Result<bool> receive(std::span<const uint8_t> data) = 0;

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
        virtual result::Result<bool> start(uint8_t session_id, uint8_t command,
                                           std::span<const uint8_t> payload,
                                           SendCallback sender,
                                           CompleteCallback on_complete) = 0;
        virtual result::Result<bool> receive(std::span<const uint8_t> data) = 0;

      protected:
        uint8_t session_id;
        uint8_t command;
        SendCallback sender;
        CompleteCallback on_complete;
};

class ITransporter {
      public:
        using FeedResult = std::pair<uint8_t, result::Result<bool>>;
        using ErrorCallback = std::function<void(std::string_view error)>;

        virtual ~ITransporter() = default;
        virtual result::Result<bool> send(uint8_t command,
                                          std::span<const uint8_t> data,
                                          ISender::CompleteCallback on_complete,
                                          ErrorCallback on_error) = 0;

        virtual std::shared_ptr<Future<result::Result<bool>>>
        send_async(uint8_t command, std::span<const uint8_t> data) = 0;

        virtual result::Result<bool>
        request(uint8_t command, std::span<const uint8_t> payload,
                IReceiver::CompleteCallback on_complete,
                ErrorCallback on_error) = 0;

        virtual std::shared_ptr<Future<result::Result<std::vector<uint8_t>>>>
        request_async(uint8_t command, std::span<const uint8_t> payload) = 0;

        virtual result::Result<FeedResult>
        feed(std::span<const uint8_t> raw) = 0;

      protected:
        std::unordered_map<uint8_t, std::unique_ptr<ISender>> senders;
        std::unordered_map<uint8_t, std::unique_ptr<IReceiver>> receivers;
        std::set<uint8_t> available_sender_sessions;
        std::set<uint8_t> available_receiver_sessions;
        uint8_t next_session_id = 0;
};
} // namespace transport
