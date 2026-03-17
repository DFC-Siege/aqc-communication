idf_component_register(SRCS "main.cpp"
                       PRIV_REQUIRES communication logger console_logger result nvs_store async esp32_ble_hal esp32_serial_hal http_client esp32_http_client
                       INCLUDE_DIRS ".")
