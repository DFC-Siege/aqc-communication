#pragma once

#include <nvs.h>
#include <string_view>

#include "i_store.hpp"
#include "result.hpp"

namespace Storage {
namespace KV {
class NVSStore : public IStore {
      public:
        NVSStore(std::string_view ns);
        Result::Result<bool> store(std::string_view key,
                                   std::string_view value) override;
        Result::Result<std::string> get(std::string_view key) override;

      private:
        std::string_view ns;
        nvs_handle_t handle;
};
} // namespace KV
} // namespace Storage
