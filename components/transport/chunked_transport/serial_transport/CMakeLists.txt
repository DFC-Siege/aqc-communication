if(ESP_PLATFORM)
idf_component_register(
    SRCS "serial_transporter.cpp"
    INCLUDE_DIRS "."
    REQUIRES result logger serial_hal chunked_transport transport
)
else()
    add_library(serial_transport serial_transporter.cpp)
    target_include_directories(serial_transport PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_features(serial_transport PUBLIC cxx_std_23)
endif()
