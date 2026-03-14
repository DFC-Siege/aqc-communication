#include "serial_logger.hpp"
#include "i_logger.hpp"

namespace Logging {
void SerialLogger::print(LogLevel level, std::string_view tag,
                         std::string_view value) {
        if (!check_level(level)) {
                return;
        }

        auto level_str = level_to_string(level);
        ::printf("[%.*s] %.*s: %.*s", (int)level_str.size(), level_str.data(),
                 (int)tag.size(), tag.data(), (int)value.size(), value.data());
}

void SerialLogger::println(LogLevel level, std::string_view tag,
                           std::string_view value) {
        if (!check_level(level)) {
                return;
        }

        print(level, tag, value);
        ::printf("\n");
}

bool SerialLogger::check_level(LogLevel level) {
        return this->level >= level;
}
} // namespace Logging
