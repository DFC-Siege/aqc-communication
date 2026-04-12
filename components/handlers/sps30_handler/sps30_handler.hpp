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

        auto &data = result.value();
        logging::logger().println_fmt(
            "SPS30 - PM1.0: {:.2f}, PM2.5: {:.2f}, "
            "PM4.0: {:.2f}, PM10.0: {:.2f}, Size: {:.2f}",
            data.pm1_0, data.pm2_5, data.pm4_0, data.pm10_0,
            data.typical_particle_size);
}
} // namespace sps30
} // namespace handlers
