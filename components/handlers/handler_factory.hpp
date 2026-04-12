#pragma once

#include "base_transporter.hpp"
#include "commands.hpp"
#include "dispatcher.hpp"
#include "scd40_handler.hpp"

namespace handlers {
class HandlerFactory {
      public:
        HandlerFactory() {
        }

        void register_handlers(
            transport::Dispatcher<transport::BaseTransporter> &dispatcher) {
                dispatcher.register_handler(models::Command::SCD, handle_scd);
        }
};
} // namespace handlers
