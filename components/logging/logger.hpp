#pragma once

#include <memory>

#include "i_logger.hpp"

namespace Logging {
class Logger {
      public:
        static ILogger &instance();

        static void set(std::unique_ptr<ILogger> l);

      private:
        static std::unique_ptr<ILogger> logger;
};
} // namespace Logging
