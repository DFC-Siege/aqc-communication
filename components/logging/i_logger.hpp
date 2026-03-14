#pragma once

#include <format>
#include <string_view>

namespace Logging {
enum class LogLevel { Verbose, Debug, Info, Warning, Error, Fatal, None };

static constexpr std::string_view level_to_string(LogLevel level) {
        switch (level) {
        case LogLevel::Verbose:
                return "V";
        case LogLevel::Debug:
                return "D";
        case LogLevel::Info:
                return "I";
        case LogLevel::Warning:
                return "W";
        case LogLevel::Error:
                return "E";
        case LogLevel::Fatal:
                return "F";
        default:
                return "?";
        }
}

class ILogger {
      public:
        virtual ~ILogger() = default;

        virtual void print(LogLevel level, std::string_view tag,
                           std::string_view value) = 0;
        virtual void println(LogLevel level, std::string_view tag,
                             std::string_view value) = 0;
        virtual void set_level(LogLevel level) = 0;
        virtual LogLevel get_level() const = 0;

        void print(std::string_view value) {
                print(LogLevel::Info, "", value);
        }

        void print(std::string_view tag, std::string_view value) {
                print(LogLevel::Info, tag, value);
        }

        void print(LogLevel level, std::string_view value) {
                print(level, "", value);
        }

        template <typename... Args>
        void print(LogLevel level, std::string_view tag,
                   std::format_string<Args...> fmt, Args &&...args) {
                print(level, tag,
                      std::format(fmt, std::forward<Args>(args)...));
        }

        template <typename... Args>
        void print(std::string_view tag, std::format_string<Args...> fmt,
                   Args &&...args) {
                print(tag, std::format(fmt, std::forward<Args>(args)...));
        }

        template <typename... Args>
        void print(LogLevel level, std::format_string<Args...> fmt,
                   Args &&...args) {
                print(level, std::format(fmt, std::forward<Args>(args)...));
        }

        template <typename... Args>
        void print(std::format_string<Args...> fmt, Args &&...args) {
                print(std::format(fmt, std::forward<Args>(args)...));
        }

        void println(std::string_view value) {
                println(LogLevel::Info, "", value);
        }

        void println(std::string_view tag, std::string_view value) {
                println(LogLevel::Info, tag, value);
        }

        void println(LogLevel level, std::string_view value) {
                println(level, "", value);
        }

        template <typename... Args>
        void println(LogLevel level, std::string_view tag,
                     std::format_string<Args...> fmt, Args &&...args) {
                println(level, tag,
                        std::format(fmt, std::forward<Args>(args)...));
        }

        template <typename... Args>
        void println(std::string_view tag, std::format_string<Args...> fmt,
                     Args &&...args) {
                println(tag, std::format(fmt, std::forward<Args>(args)...));
        }

        template <typename... Args>
        void println(LogLevel level, std::format_string<Args...> fmt,
                     Args &&...args) {
                println(level, std::format(fmt, std::forward<Args>(args)...));
        }

        template <typename... Args>
        void println(std::format_string<Args...> fmt, Args &&...args) {
                println(std::format(fmt, std::forward<Args>(args)...));
        }
};
} // namespace Logging
