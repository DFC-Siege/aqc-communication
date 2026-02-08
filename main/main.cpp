#include <stdio.h>
#include <string.h>

#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_MASTER_SCL_IO 9
#define I2C_MASTER_SDA_IO 8
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000
#define SCD40_SENSOR_ADDR 0x62

#define UART_NUM UART_NUM_1
#define TX_PIN 7
#define RX_PIN 6

void init_hardware() {
        uart_config_t uart_conf = {.baud_rate = 115200,
                                   .data_bits = UART_DATA_8_BITS,
                                   .parity = UART_PARITY_DISABLE,
                                   .stop_bits = UART_STOP_BITS_1,
                                   .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
                                   .rx_flow_ctrl_thresh = 122,
                                   .source_clk = UART_SCLK_DEFAULT,
                                   .flags = {}};
        uart_driver_install(UART_NUM, 256, 0, 0, NULL, 0);
        uart_param_config(UART_NUM, &uart_conf);
        uart_set_pin(UART_NUM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE,
                     UART_PIN_NO_CHANGE);
}

extern "C" void app_main(void) {
        init_hardware();

        while (1) {
        }
}
