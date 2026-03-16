#pragma once

#include <string>
#include <string_view>

#include "result.hpp"

namespace Storage {
namespace KV {
class IStore {
      public:
        virtual ~IStore() = default;
        virtual Result::Result<bool> store(std::string_view key,
                                           std::string_view value) = 0;
        virtual Result::Result<std::string> get(std::string_view key) = 0;
};
} // namespace KV
} // namespace Storage
