#pragma once

#include "i_logger.hpp"

namespace Logging {
class SerialLogger : public ILogger {
      public:
        SerialLogger(LogLevel level);

        virtual void print(LogLevel level, std::string_view tag,
                           std::string_view value);
        virtual void println(LogLevel level, std::string_view tag,
                             std::string_view value);
        virtual void set_level(LogLevel level);
        virtual LogLevel get_level() const;

      private:
        LogLevel level;

        bool check_level(LogLevel level);
};
} // namespace Logging
