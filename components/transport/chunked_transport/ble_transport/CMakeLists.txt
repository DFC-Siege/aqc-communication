idf_component_register(
    SRCS "ble_transporter.cpp"
    INCLUDE_DIRS "."
    REQUIRES result logger ble_hal chunked_transport transport
)
