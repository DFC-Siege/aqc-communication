#include <nvs.h>

#include "nvs_store.hpp"
#include "result.hpp"
#include "result/result.hpp"

namespace Storage {
namespace KV {
NVSStore::NVSStore(std::string_view ns) : ns(ns) {
        nvs_open(std::string(ns).c_str(), NVS_READWRITE, &handle);
}
//
// *-ESP_OK if value was set successfully *
//         -ESP_ERR_NVS_INVALID_HANDLE if handle has been closed or
//     is NULL * -ESP_ERR_NVS_READ_ONLY if storage handle was opened as read
//     only *
//             -ESP_ERR_NVS_INVALID_NAME if key
//                 name doesn't satisfy constraints * -
//         ESP_ERR_NVS_NOT_ENOUGH_SPACE if there is not enough space in the *
//             underlying storage to save the value *
//             -ESP_ERR_NVS_REMOVE_FAILED if the
//                  value wasn't updated because flash * write
//                      operation has failed.The value was written however,
//     and*update will be finished after re - initialization of nvs,
//     provided that *flash operation doesn't fail again. * -
//         ESP_ERR_NVS_VALUE_TOO_LONG if the string value is too long void
Result::Result<bool> NVSStore::store(std::string_view key,
                                     std::string_view value) {
        auto err = nvs_set_str(handle, std::string(key).c_str(),
                               std::string(value).c_str());
        // switch (err) {
        // case ESP_OK:
        //         break;
        // case ESP_ERR_NVS_INVALID_HANDLE:
        //         // TODO: Handle
        //         return;
        // }

        err = nvs_commit(handle);

        return Result::ok();
}

Result::Result<std::string> NVSStore::get(std::string_view key) {
        const auto key_str = std::string(key);
        size_t len = 0;
        nvs_get_str(handle, key_str.c_str(), nullptr, &len);
        std::string result(len, '\0');
        nvs_get_str(handle, key_str.c_str(), result.data(), &len);
        return Result::ok(result);
}
} // namespace KV
} // namespace Storage
