idf_component_register(
    SRCS
        "chunked_receiver.cpp"
        "chunked_sender.cpp"
        "chunked_transporter.cpp"
    INCLUDE_DIRS "."
    REQUIRES result logger transport platform_async core_async
)
