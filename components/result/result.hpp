#pragma once

#include <string_view>

namespace Result {

template <typename T> struct Result {
        bool failed;
        std::string_view err;
        T value;
};

template <typename T> struct RefResult {
        bool failed;
        std::string_view err;
        T *value;
};

struct Error {
        std::string_view message;

        template <typename T> operator Result<T>() const {
                return Result<T>{true, message, T{}};
        }

        template <typename T> operator RefResult<T>() const {
                return RefResult<T>{true, message, nullptr};
        }
};

inline Error err(std::string_view message) {
        return Error{message};
}

inline Result<bool> ok() {
        return Result<bool>{false, "", true};
}

template <typename T> Result<T> ok(T value) {
        return Result<T>{false, "", value};
}

template <typename T> RefResult<T> ok_ref(T &value) {
        return RefResult<T>{false, "", &value};
}

} // namespace Result
