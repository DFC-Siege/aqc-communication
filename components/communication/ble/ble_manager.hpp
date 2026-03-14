#pragma once

#include <cstdint>
#include <string_view>

#include "host/ble_gap.h"
#include "host/ble_hs.h"
#include "services/gatt/ble_svc_gatt.h"

#include "i_ble_transport.hpp"

namespace BLE {

class BLEManager : public IBLETransport {
      public:
        static BLEManager &instance() {
                static BLEManager inst;
                return inst;
        }

        void begin(std::string_view deviceName);

        void send(std::span<const uint8_t> data) override;
        void onReceive(ReceiveCallback cb) override {
                receiveCallback = std::move(cb);
        }
        void onConnectionChanged(ConnectionCallback cb) override {
                connectionCallback = std::move(cb);
        }
        bool isConnected() const override {
                return connHandle != BLE_HS_CONN_HANDLE_NONE;
        }

        BLEManager(const BLEManager &) = delete;
        BLEManager &operator=(const BLEManager &) = delete;

        static int onGapEvent(ble_gap_event *event, void *arg);
        static int onGattAccess(uint16_t connHandle, uint16_t attrHandle,
                                ble_gatt_access_ctxt *ctxt, void *arg);

      private:
        BLEManager() = default;

        void startAdvertising();

        ReceiveCallback receiveCallback;
        ConnectionCallback connectionCallback;

        uint16_t connHandle = BLE_HS_CONN_HANDLE_NONE;
        static uint16_t txAttrHandle;

        static constexpr ble_uuid128_t serviceUuid =
            BLE_UUID128_INIT(0x6e, 0x40, 0x00, 0x01, 0xb5, 0xa3, 0xf3, 0x93,
                             0xe0, 0xa9, 0xe5, 0x0e, 0x24, 0xdc, 0xca, 0x9e);
        static constexpr ble_uuid128_t rxUuid =
            BLE_UUID128_INIT(0x6e, 0x40, 0x00, 0x02, 0xb5, 0xa3, 0xf3, 0x93,
                             0xe0, 0xa9, 0xe5, 0x0e, 0x24, 0xdc, 0xca, 0x9e);
        static constexpr ble_uuid128_t txUuid =
            BLE_UUID128_INIT(0x6e, 0x40, 0x00, 0x03, 0xb5, 0xa3, 0xf3, 0x93,
                             0xe0, 0xa9, 0xe5, 0x0e, 0x24, 0xdc, 0xca, 0x9e);

        static const ble_gatt_svc_def gattServices[];
};

} // namespace BLE
