idf_component_register(SRCS 
        "communication_handler.cpp"
        "serial/serial_manager.cpp"
        "ble/ble_manager.cpp"
        "transport/chunked_sender.cpp"
        "transport/chunked_receiver.cpp"
        "transport/ble/ble_transporter.cpp"
        "transport/chunked_transporter.cpp"
        INCLUDE_DIRS "."
        REQUIRES esp_wifi nvs_flash esp_driver_uart bt esp_rom result logging
        async)
