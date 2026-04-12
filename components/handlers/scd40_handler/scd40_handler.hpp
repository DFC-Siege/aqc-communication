#pragma once

#include "i_logger.hpp"
#include "logger.hpp"
#include "result.hpp"
#include "scd40.hpp"
#include "transport_data.hpp"

namespace handlers {
namespace scd40 {
static constexpr auto TAG = "scd40_handler";

inline static void handle(result::Result<models::SCD40> result) {
        if (result.failed()) {
                logging::logger().println(logging::LogLevel::Error, TAG,
                                          result.error());
                return;
        }

        logging::logger().println_fmt("todo");
}
} // namespace scd40
} // namespace handlers
