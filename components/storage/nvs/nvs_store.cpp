#include <nvs.h>
#include <nvs_flash.h>
#include <string_view>

#include "nvs_store.hpp"
#include "result.hpp"

namespace Storage {
namespace KV {
NVSStore::NVSStore(std::string_view ns) : ns(ns) {
}

Result::Result<bool> NVSStore::try_init(int count) {
        auto err = nvs_flash_init();
        switch (err) {
        case ESP_OK:
                break;
        case ESP_ERR_NVS_NEW_VERSION_FOUND:
        case ESP_ERR_NVS_NO_FREE_PAGES:
                if (count > MAX_INIT_TRIES) {
                        return Result::err("failed to init nvs after " +
                                           std::to_string(count) + " tries");
                }
                try_init(count++);
        case ESP_ERR_NOT_FOUND:
                return Result::err("no partition with label \"nvs\" found in "
                                   "the partition table");
        case ESP_ERR_NO_MEM:
                return Result::err(
                    "esp doesn't have enough memory to init nvs");
        default:
                return Result::err("an unknown error has occured while "
                                   "trying to initialize the NVS");
        }

        return Result::ok();
}

Result::Result<bool> NVSStore::try_open() {
        auto ret = nvs_open(std::string(ns).c_str(), NVS_READWRITE, &handle);
        switch (ret) {
        case ESP_OK:
                break;
        case ESP_FAIL:
                return Result::err("nvs partition corrupted");
        case ESP_ERR_NVS_NOT_INITIALIZED:
                return Result::err("nvs not initialized");
        case ESP_ERR_NVS_PART_NOT_FOUND:
                return Result::err("nvs partition not found");
        case ESP_ERR_NVS_NOT_FOUND:
                return Result::err("namespace not found");
        case ESP_ERR_NVS_INVALID_NAME:
                return Result::err("invalid namespace name");
        case ESP_ERR_NO_MEM:
                return Result::err("out of memory");
        case ESP_ERR_NVS_NOT_ENOUGH_SPACE:
                return Result::err("not enough space");
        case ESP_ERR_NOT_ALLOWED:
                return Result::err("partition is read only");
        case ESP_ERR_INVALID_ARG:
                return Result::err("invalid argument");
        default:
                return Result::err("unknown error");
        }

        return Result::ok();
}

Result::Result<NVSStore> NVSStore::init(std::string_view ns) {
        auto result = try_init();
        if (result.failed) {
                return Result::err(result.err);
        }

        auto store = NVSStore(ns);
        result = store.try_open();
        if (result.failed) {
                return Result::err(result.err);
        }

        return Result::ok(store);
}

Result::Result<bool> NVSStore::store(std::string_view key,
                                     std::string_view value) {
        auto err = nvs_set_str(handle, std::string(key).c_str(),
                               std::string(value).c_str());
        switch (err) {
        case ESP_OK:
                break;
        case ESP_ERR_NVS_INVALID_HANDLE:
                return Result::err("invalid handle");
        case ESP_ERR_NVS_READ_ONLY:
                return Result::err("storage is opened as read only");
        case ESP_ERR_NVS_INVALID_NAME:
                return Result::err("key name doesn't satisfy constraints");
        case ESP_ERR_NVS_NOT_ENOUGH_SPACE:
                return Result::err("not enough space in the storage");
        case ESP_ERR_NVS_REMOVE_FAILED:
                return Result::err(
                    "value was written but storage failed to update flash. "
                    "update will finish after reinitialization");
        case ESP_ERR_NVS_VALUE_TOO_LONG:
                return Result::err("value is too long");
        default:
                return Result::err(
                    "an unknown error occured while setting string");
        }

        err = nvs_commit(handle);
        switch (err) {
        case ESP_OK:
                break;
        case ESP_ERR_NVS_INVALID_HANDLE:
                return Result::err("invalid handle");
        default:
                return Result::err(
                    "an unknown error occured while commiting to nvs");
        }

        return Result::ok();
}

Result::Result<std::string> NVSStore::get(std::string_view key) {
        const auto key_str = std::string(key);

        auto check = [](esp_err_t ret) -> Result::Result<bool> {
                switch (ret) {
                case ESP_OK:
                        return Result::ok();
                case ESP_ERR_NVS_NOT_FOUND:
                        return Result::err("key not found");
                case ESP_ERR_NVS_INVALID_HANDLE:
                        return Result::err("invalid handle");
                case ESP_ERR_NVS_INVALID_NAME:
                        return Result::err("invalid key name");
                case ESP_ERR_NVS_INVALID_LENGTH:
                        return Result::err("invalid length");
                case ESP_FAIL:
                        return Result::err("nvs corrupted");
                default:
                        return Result::err("unknown error");
                }
        };

        size_t len = 0;
        auto result =
            check(nvs_get_str(handle, key_str.c_str(), nullptr, &len));
        if (result.failed)
                return Result::err(result.err);

        std::string str(len, '\0');
        result = check(nvs_get_str(handle, key_str.c_str(), str.data(), &len));
        if (result.failed)
                return Result::err(result.err);

        return Result::ok(str);
}
} // namespace KV
} // namespace Storage
