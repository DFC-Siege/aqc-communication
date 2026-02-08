#include <algorithm>
#include <driver/uart.h>
#include <hal/uart_types.h>

#include "serial_manager.hpp"

namespace Communication {
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

void SerialManager::send(std::string value) {
        uart_write_bytes(UART_NUM_1, value.data(), value.size());
}

uint32_t
SerialManager::add_listener(std::function<void(std::string)> function) {
        listeners.emplace_back(Listener{++last_id, function});
        return last_id;
}

void SerialManager::remove_listener(uint32_t id) {
        auto it =
            std::remove_if(listeners.begin(), listeners.end(),
                           [id](const Listener &l) { return l.id == id; });
        listeners.erase(it, listeners.end());
}

void SerialManager::update() {
        uint8_t data[BUF_SIZE];
        int length = uart_read_bytes(UART_NUM_1, data, BUF_SIZE, 0);
        if (length > 0) {
                std::string received_str(reinterpret_cast<char *>(data),
                                         length);
                for (const auto &listener : listeners) {
                        listener.function(received_str);
                }
        }
}
} // namespace Communication
