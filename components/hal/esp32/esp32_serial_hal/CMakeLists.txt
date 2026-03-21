if(ESP_PLATFORM)
idf_component_register(
    SRCS "serial_hal.cpp"
    INCLUDE_DIRS "."
    REQUIRES result logger driver serial_hal
)
else()
    add_library(esp32_serial_hal serial_hal.cpp)
    target_include_directories(esp32_serial_hal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(esp32_serial_hal PUBLIC esp32_serial_hal)
    target_compile_features(esp32_serial_hal PUBLIC cxx_std_23)
endif()
