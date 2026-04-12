#pragma once

#include <string>
#include <utility>

#include "result.hpp"
#include "serializer.hpp"

namespace models {
struct SPS30 {
        float pm1_0 = 0.0f;
        float pm2_5 = 0.0f;
        float pm4_0 = 0.0f;
        float pm10_0 = 0.0f;
        float typical_particle_size = 0.0f;
        std::string error;

        serializer::Data serialize() const {
                serializer::Writer w;
                w.write(pm1_0);
                w.write(pm2_5);
                w.write(pm4_0);
                w.write(pm10_0);
                w.write(typical_particle_size);
                w.write(error);
                return std::move(w.buf);
        }

        static result::Result<SPS30> deserialize(serializer::DataView buf) {
                serializer::Reader r{buf};
                return result::ok(SPS30{
                    .pm1_0 = TRY(r.read<float>()),
                    .pm2_5 = TRY(r.read<float>()),
                    .pm4_0 = TRY(r.read<float>()),
                    .pm10_0 = TRY(r.read<float>()),
                    .typical_particle_size = TRY(r.read<float>()),
                    .error = TRY(r.read_string()),
                });
        }
};
} // namespace models
