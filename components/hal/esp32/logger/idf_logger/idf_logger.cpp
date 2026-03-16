#include <esp_log.h>
#include <esp_system.h>

#include "i_logger.hpp"
#include "idf_logger.hpp"

namespace Logging {

void IdfLogger::print(LogLevel level, std::string_view tag,
                      std::string_view value) {
        switch (level) {
        case LogLevel::Verbose:
                ESP_LOGV(tag.data(), "%.*s", (int)value.size(), value.data());
                break;
        case LogLevel::Debug:
                ESP_LOGD(tag.data(), "%.*s", (int)value.size(), value.data());
                break;
        case LogLevel::Info:
                ESP_LOGI(tag.data(), "%.*s", (int)value.size(), value.data());
                break;
        case LogLevel::Warning:
                ESP_LOGW(tag.data(), "%.*s", (int)value.size(), value.data());
                break;
        case LogLevel::Error:
                ESP_LOGE(tag.data(), "%.*s", (int)value.size(), value.data());
                break;
        case LogLevel::Fatal:
                ESP_LOGE(tag.data(), "%.*s", (int)value.size(), value.data());
                esp_restart();
                break;
        case LogLevel::None:
                break;
        }
}

void IdfLogger::println(LogLevel level, std::string_view tag,
                        std::string_view value) {
        print(level, tag, value);
}
} // namespace Logging
