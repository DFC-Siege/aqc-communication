#pragma once

#include "i_logger.hpp"
#include "logger.hpp"
#include "result.hpp"
#include "scd40.hpp"
#include "transport_data.hpp"

namespace handlers {
namespace scd40 {
static constexpr logging::Tag TAG = "scd40_handler";

inline static void handle(result::Result<models::SCD40> result) {
        if (result.failed()) {
                logging::logger().println(logging::LogLevel::Error, TAG,
                                          result.error());
                return;
        }

        auto &data = result.value();
        logging::logger().println_fmt(
            "SCD40 - CO2: {} ppm, Temp: {:.2f}°C, Humidity: {:.2f}%", data.co2,
            data.temperature, data.humidity);
}
} // namespace scd40
} // namespace handlers
