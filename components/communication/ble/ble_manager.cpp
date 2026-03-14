#include <cstring>
#include <vector>

#include "esp_log.h"
#include "host/ble_hs.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "ble_manager.hpp"

namespace BLE {

static constexpr char TAG[] = "BLEManager";

uint16_t BLEManager::txAttrHandle = 0;

const ble_gatt_svc_def BLEManager::gattServices[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &serviceUuid.u,
        .characteristics =
            (ble_gatt_chr_def[]){
                {
                    .uuid = &rxUuid.u,
                    .access_cb = BLEManager::onGattAccess,
                    .arg = nullptr,
                    .descriptors = nullptr,
                    .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_NO_RSP,
                },
                {
                    .uuid = &txUuid.u,
                    .access_cb = BLEManager::onGattAccess,
                    .arg = nullptr,
                    .descriptors = nullptr,
                    .flags = BLE_GATT_CHR_F_NOTIFY,
                    .val_handle = &txAttrHandle,
                },
                {0},
            },
    },
    {0},
};

void BLEManager::begin(std::string_view deviceName) {
        nimble_port_init();

        ble_hs_cfg.sync_cb = []() { instance().startAdvertising(); };

        ble_svc_gap_init();
        ble_svc_gatt_init();
        ble_gatts_count_cfg(gattServices);
        ble_gatts_add_svcs(gattServices);
        ble_svc_gap_device_name_set(deviceName.data());

        nimble_port_freertos_init([](void *) {
                nimble_port_run();
                nimble_port_freertos_deinit();
        });
}

void BLEManager::send(std::span<const uint8_t> data) {
        if (!isConnected())
                return;

        os_mbuf *om = ble_hs_mbuf_from_flat(data.data(), data.size());
        if (!om)
                return;

        ble_gatts_notify_custom(connHandle, txAttrHandle, om);
}

void BLEManager::startAdvertising() {
        ble_hs_adv_fields fields = {};
        fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
        fields.tx_pwr_lvl_is_present = 1;
        fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

        const char *name = ble_svc_gap_device_name();
        fields.name = reinterpret_cast<const uint8_t *>(name);
        fields.name_len = strlen(name);
        fields.name_is_complete = 1;

        ble_gap_adv_set_fields(&fields);

        ble_gap_adv_params adv_params = {};
        adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
        adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

        ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, nullptr, BLE_HS_FOREVER,
                          &adv_params, onGapEvent, nullptr);
}

int BLEManager::onGapEvent(ble_gap_event *event, void *) {
        auto &self = instance();

        switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
                if (event->connect.status == 0) {
                        self.connHandle = event->connect.conn_handle;
                        if (self.connectionCallback)
                                self.connectionCallback(true);
                } else {
                        self.startAdvertising();
                }
                break;

        case BLE_GAP_EVENT_DISCONNECT:
                self.connHandle = BLE_HS_CONN_HANDLE_NONE;
                if (self.connectionCallback)
                        self.connectionCallback(false);
                self.startAdvertising();
                break;

        case BLE_GAP_EVENT_MTU:
                ESP_LOGI(TAG, "MTU updated: %d", event->mtu.value);
                break;
        }

        return 0;
}

int BLEManager::onGattAccess(uint16_t, uint16_t, ble_gatt_access_ctxt *ctxt,
                             void *) {
        auto &self = instance();

        if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
                uint16_t len = OS_MBUF_PKTLEN(ctxt->om);
                std::vector<uint8_t> buf(len);
                ble_hs_mbuf_to_flat(ctxt->om, buf.data(), len, nullptr);
                if (self.receiveCallback)
                        self.receiveCallback(buf);
        }

        return 0;
}

} // namespace BLE
