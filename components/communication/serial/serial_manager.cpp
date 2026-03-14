#include <algorithm>
#include <driver/uart.h>
#include <hal/uart_types.h>

#include "serial_manager.hpp"

namespace Serial {
SerialManager::SerialManager() {
        uart_config_t uart_config = {.baud_rate = BAUDRATE,
                                     .data_bits = UART_DATA_8_BITS,
                                     .parity = UART_PARITY_DISABLE,
                                     .stop_bits = UART_STOP_BITS_1,
                                     .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
                                     .rx_flow_ctrl_thresh =
                                         UART_HW_FLOWCTRL_DISABLE,
                                     .source_clk = UART_SCLK_DEFAULT,
                                     .flags = {}};

        uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);
        uart_param_config(UART_NUM_1, &uart_config);
        uart_set_pin(UART_NUM_1, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE,
                     UART_PIN_NO_CHANGE);
}

void SerialManager::send(std::span<const uint8_t> data) {
        uart_write_bytes(UART_NUM_1, data.data(), data.size());
}

void SerialManager::loop() {
        std::vector<uint8_t> data(BUF_SIZE);
        int length = uart_read_bytes(UART_NUM_1, data.data(), BUF_SIZE, 0);
        if (length <= 0)
                return;
        data.resize(length);

        if (receive_callback) {
                receive_callback(data);
        }
}
} // namespace Serial
