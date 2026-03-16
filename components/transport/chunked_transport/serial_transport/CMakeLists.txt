idf_component_register(
    SRCS "serial_transporter.cpp"
    INCLUDE_DIRS "."
    REQUIRES result logger serial_hal chunked_transport transport
)
