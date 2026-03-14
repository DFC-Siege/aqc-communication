#include "serial_logger.hpp"
#include "i_logger.hpp"

namespace Logging {
SerialLogger::SerialLogger(LogLevel level) : level(level) {
}

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

void SerialLogger::set_level(LogLevel level) {
        this->level = level;
}

LogLevel SerialLogger::get_level() const {
        return level;
}

bool SerialLogger::check_level(LogLevel level) {
        return this->level >= level;
}
} // namespace Logging
