#include "i_logger.hpp"

namespace Logging {
void ILogger::set_level(LogLevel level) {
        this->level = level;
}

LogLevel ILogger::get_level() const {
        return level;
}
} // namespace Logging
