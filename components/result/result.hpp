#pragma once

#include <optional>
#include <string_view>

namespace Result {
template <typename T> class Result {
      public:
        Result(bool fail, std::string_view err, std::optional<T> val)
            : fail(fail), err(err), val(std::move(val)) {
        }
        bool failed() const {
                return fail;
        }

        std::string_view error() const {
                return err;
        }

        const T &value() const {
                return val.value();
        }

      private:
        bool fail;
        std::string_view err;
        std::optional<T> val;
};

template <typename T> class RefResult {
      public:
        RefResult(bool fail, std::string_view err, T *val)
            : fail(fail), err(err), val(val) {
        }

        bool failed() const {
                return fail;
        }

        std::string_view error() const {
                return err;
        }

        const T &value() const {
                return *val;
        }

      private:
        bool fail;
        std::string_view err;
        T *val;
};

struct Error {
        std::string_view message;

        template <typename T> operator Result<T>() const {
                return Result<T>{true, message, std::nullopt};
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
        return Result<T>{false, "", std::move(value)};
}

template <typename T> RefResult<T> ok_ref(T &value) {
        return RefResult<T>{false, "", &value};
}

} // namespace Result
