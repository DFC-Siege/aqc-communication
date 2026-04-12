#pragma once

#include "base_transporter.hpp"
#include "commands.hpp"
#include "scd40.hpp"
#include "scd40_handler.hpp"
#include "serialized_dispatcher.hpp"
#include "sps30.hpp"
#include "sps30_handler.hpp"

namespace handlers {
class HandlerFactory {
      public:
        HandlerFactory() {
        }

        void register_handlers(
            transport::SerializedDispatcher<transport::BaseTransporter>
                &dispatcher) {
                dispatcher.register_handler<models::SCD40>(models::Command::SCD,
                                                           scd40::handle);
                dispatcher.register_handler<models::SPS30>(models::Command::SPS,
                                                           sps30::handle);
        }
};
} // namespace handlers
