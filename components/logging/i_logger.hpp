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

        void set_level(LogLevel l) {
                level = l;
        }

        LogLevel get_level() const {
                return level;
        }

        void print(std::string_view value) {
                print(level, "", value);
        }
        void print(std::string_view tag, std::string_view value) {
                print(level, tag, value);
        }
        void print(LogLevel l, std::string_view value) {
                print(l, "", value);
        }

        void println(std::string_view value) {
                println(level, "", value);
        }
        void println(std::string_view tag, std::string_view value) {
                println(level, tag, value);
        }
        void println(LogLevel l, std::string_view value) {
                println(l, "", value);
        }

        template <typename... Args>
        void print_fmt(LogLevel l, std::string_view tag,
                       std::format_string<Args...> fmt, Args &&...args) {
                print(l, tag, std::format(fmt, std::forward<Args>(args)...));
        }
        template <typename... Args>
        void print_fmt(std::string_view tag, std::format_string<Args...> fmt,
                       Args &&...args) {
                print(level, tag,
                      std::format(fmt, std::forward<Args>(args)...));
        }
        template <typename... Args>
        void print_fmt(LogLevel l, std::format_string<Args...> fmt,
                       Args &&...args) {
                print(l, "", std::format(fmt, std::forward<Args>(args)...));
        }
        template <typename... Args>
        void print_fmt(std::format_string<Args...> fmt, Args &&...args) {
                print(level, "", std::format(fmt, std::forward<Args>(args)...));
        }

        template <typename... Args>
        void println_fmt(LogLevel l, std::string_view tag,
                         std::format_string<Args...> fmt, Args &&...args) {
                println(l, tag, std::format(fmt, std::forward<Args>(args)...));
        }
        template <typename... Args>
        void println_fmt(std::string_view tag, std::format_string<Args...> fmt,
                         Args &&...args) {
                println(level, tag,
                        std::format(fmt, std::forward<Args>(args)...));
        }
        template <typename... Args>
        void println_fmt(LogLevel l, std::format_string<Args...> fmt,
                         Args &&...args) {
                println(l, "", std::format(fmt, std::forward<Args>(args)...));
        }
        template <typename... Args>
        void println_fmt(std::format_string<Args...> fmt, Args &&...args) {
                println(level, "",
                        std::format(fmt, std::forward<Args>(args)...));
        }

      protected:
        LogLevel level = LogLevel::Info;
};

} // namespace Logging
