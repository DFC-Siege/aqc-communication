#pragma once

#include <cstdint>
#include <string>
#include <utility>

#include "result.hpp"
#include "serializer.hpp"

namespace models {
struct SCD40 {
        uint16_t co2 = 0;
        float temperature = 0.0f;
        float humidity = 0.0f;
        std::string error;

        serializer::Data serialize() const {
                serializer::Writer w;
                w.write(co2);
                w.write(temperature);
                w.write(humidity);
                w.write(error);
                return std::move(w.buf);
        }

        static result::Result<SCD40> deserialize(serializer::DataView buf) {
                serializer::Reader r{buf};
                return result::ok(SCD40{
                    .co2 = TRY(r.read<uint16_t>()),
                    .temperature = TRY(r.read<float>()),
                    .humidity = TRY(r.read<float>()),
                    .error = TRY(r.read_string()),
                });
        }
};
} // namespace models
