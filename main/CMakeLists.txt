if(ESP_PLATFORM)
    idf_component_register(
        SRCS "main.cpp"
        INCLUDE_DIRS "."
        PRIV_REQUIRES communication logger console_logger result nvs_store platform_async esp32_ble_hal esp32_serial_hal http_client esp32_http_client
    )
else()
    add_executable(communication main_posix.cpp)
    target_link_libraries(communication PRIVATE
        logger
        console_logger
        result
        platform_async
        posix_serial_hal
        dispatcher
        serial_transporter
        chunked_transporter
        transporter
    )
    target_compile_features(communication PRIVATE cxx_std_23)
endif()
