#pragma once

#include <nvs.h>
#include <string_view>

#include "i_store.hpp"
#include "result.hpp"

namespace Storage {
namespace KV {
class NVSStore : public IStore {
      public:
        static Result::Result<NVSStore> init(std::string_view ns);
        Result::Result<bool> store(std::string_view key,
                                   std::string_view value) override;
        Result::Result<std::string> get(std::string_view key) override;

      private:
        std::string_view ns;
        nvs_handle_t handle;
        static constexpr auto MAX_INIT_TRIES = 3;

        NVSStore(std::string_view ns);

        Result::Result<bool> try_open();
        static Result::Result<bool> try_init(int count = 0);
};
} // namespace KV
} // namespace Storage
