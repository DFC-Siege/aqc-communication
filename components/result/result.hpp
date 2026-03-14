#pragma once

#include <cstddef>
#include <string>

namespace Result {
template <typename T> struct Result {
        bool failed;
        std::string err;
        T value;
};

template <typename T> struct RefResult {
        bool failed;
        std::string err;
        T *value;
};

static Result<bool> ok() {
        return Result<bool>{false, "", true};
}

template <typename T> Result<T> ok(T value) {
        return Result<T>{false, "", value};
}

template <typename T> Result<T> err(std::string err) {
        return Result<T>{true, err, T{}};
}

template <typename T> RefResult<T> ok_ref(T value) {
        return Result<T>{false, "", value};
}

template <typename T> RefResult<T> err_ref(std::string err) {
        return Result<T>{true, err, nullptr};
}
} // namespace Result
