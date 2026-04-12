if(ESP_PLATFORM)
    idf_component_register(
        INCLUDE_DIRS "."
        REQUIRES
            dispatcher
            serial_transporter
            chunked_transporter
            direct_transporter
            serial_hal
            handlers
    )
else()
    add_library(communication_lib INTERFACE)
    target_include_directories(communication_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(communication_lib INTERFACE
            dispatcher
            serial_transporter
            chunked_transporter
            direct_transporter
            serial_hal
            handlers
    )
endif()
