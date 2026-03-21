if(ESP_PLATFORM)
idf_component_register(
    INCLUDE_DIRS "."
    REQUIRES result
)
else()
    add_library(serial_hal INTERFACE)
    target_include_directories(serial_hal INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_features(serial_hal INTERFACE cxx_std_23)
endif()
