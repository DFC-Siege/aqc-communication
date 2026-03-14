#include "logger.hpp"
#include "i_logger.hpp"

namespace Logging {
std::unique_ptr<ILogger> Logger::logger = std::make_unique<NullLogger>();

ILogger &Logger::instance() {
        assert(logger != nullptr && "No logger set");
        return *logger;
}

void Logger::set(std::unique_ptr<ILogger> l) {
        logger = std::move(l);
}
} // namespace Logging
