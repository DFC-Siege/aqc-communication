idf_component_register(SRCS "serial/serial_manager.cpp"
                             "ble/ble_manager.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES esp_wifi nvs_flash esp_driver_uart bt result)
