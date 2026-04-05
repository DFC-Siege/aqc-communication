if(ESP_PLATFORM)
    idf_component_register(
        SRCS "main_esp32.cpp"
        INCLUDE_DIRS "."
        PRIV_REQUIRES 
                logger
                console_logger
                result
                esp32_serial_hal
                dispatcher
                serial_transporter
                direct_transporter
                chunked_transporter
                transporter
    )
else()
    add_executable(communication main_posix.cpp)
    target_link_libraries(communication PRIVATE
        logger
        console_logger
        result
        posix_serial_hal
        dispatcher
        serial_transporter
        direct_transporter
        chunked_transporter
        transporter
    )
    target_compile_features(communication PRIVATE cxx_std_23)
endif()
