#pragma once

#include <memory>
#include <utility>

#include "base_transporter.hpp"
#include "chunked_transporter.hpp"
#include "direct_transporter.hpp"
#include "dispatcher.hpp"
#include "handler_factory.hpp"
#include "i_serial_hal.hpp"
#include "multiplexer.hpp"
#include "serial_transporter.hpp"
#include "serialized_dispatcher.hpp"

namespace communication {
enum Channel : transport::TransporterId {
        Chunked,
        Direct,
};

class CommunicationManager {
      public:
        CommunicationManager(serial::ISerialHal &serial_hal,
                             handlers::HandlerFactory &&handler_factory)
            : handler_factory(std::move(handler_factory)) {
                static constexpr auto MTU = 17;
                static constexpr auto MAX_TRIES = 3;
                static constexpr auto TIMEOUT = std::chrono::milliseconds(1000);

                auto serial_transporter =
                    std::make_unique<transport::SerialTransporter>(serial_hal,
                                                                   MTU);
                transport::Multiplexer multiplexer(
                    std::move(serial_transporter));

                using MuxChannel = transport::Multiplexer<
                    transport::SerialTransporter>::InnerChannel;
                using ChunkedMuxChannel =
                    transport::ChunkedTransporter<MuxChannel>;
                using DirectMuxChannel =
                    transport::DirectTransporter<MuxChannel>;

                auto inner_chunked_channel =
                    multiplexer.create_inner_channel(Channel::Chunked);
                auto chunked = std::make_unique<ChunkedMuxChannel>(
                    std::move(inner_chunked_channel), MAX_TRIES, TIMEOUT);

                auto inner_direct_channel =
                    multiplexer.create_inner_channel(Channel::Direct);
                auto direct = std::make_unique<DirectMuxChannel>(
                    std::move(inner_direct_channel));

                auto dispatcher = std::make_unique<
                    transport::Dispatcher<transport::BaseTransporter>>();
                dispatcher->register_transporter(Channel::Chunked,
                                                 std::move(chunked));
                dispatcher->register_transporter(Channel::Direct,
                                                 std::move(direct));

                std::make_unique<transport::SerializedDispatcher<
                    transport::BaseTransporter>>(std::move(dispatcher));
                handler_factory.register_handlers(*serialized_dispatcher);
        }

        transport::SerializedDispatcher<transport::BaseTransporter> &
        get_dispatcher() {
                return *serialized_dispatcher;
        }

      private:
        handlers::HandlerFactory handler_factory;
        std::unique_ptr<
            transport::SerializedDispatcher<transport::BaseTransporter>>
            serialized_dispatcher;
};
} // namespace communication
