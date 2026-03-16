idf_component_register(SRCS "main.cpp"
                       PRIV_REQUIRES communication logger console_logger result nvs_store async esp32_ble_hal esp32_serial_hal
                       INCLUDE_DIRS ".")
