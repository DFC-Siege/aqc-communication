if(ESP_PLATFORM)
    idf_component_register(
        SRCS "main_esp32.cpp"
        INCLUDE_DIRS "."
        PRIV_REQUIRES 
                logger
                console_logger
                result
                serializer
                esp32_serial_hal
                dispatcher
                serial_transporter
                direct_transporter
                chunked_transporter
                transporter
                esp_driver_uart
                requester
    )
else()
    add_executable(communication main_posix.cpp)
    target_link_libraries(communication PRIVATE
        logger
        console_logger
        result
        serializer
        posix_serial_hal
        dispatcher
        serial_transporter
        direct_transporter
        chunked_transporter
        transporter
        requester
    )
    target_compile_features(communication PRIVATE cxx_std_23)
endif()
