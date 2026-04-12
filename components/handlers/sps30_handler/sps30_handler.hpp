#pragma once

#include "i_logger.hpp"
#include "logger.hpp"
#include "result.hpp"
#include "sps30.hpp"
#include "transport_data.hpp"

namespace handlers {
namespace sps30 {
static constexpr auto TAG = "sps30_handler";

inline static void handle(result::Result<models::SPS30> result) {
        if (result.failed()) {
                logging::logger().println(logging::LogLevel::Error, TAG,
                                          result.error());
                return;
        }

        logging::logger().println_fmt("todo");
}
} // namespace sps30
} // namespace handlers
