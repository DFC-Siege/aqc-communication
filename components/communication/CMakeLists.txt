idf_component_register(
        SRCS "communication_handler.cpp"
        INCLUDE_DIRS "."
        REQUIRES logger ble_hal ble_transport platform_async serial_hal serial_transport
)
