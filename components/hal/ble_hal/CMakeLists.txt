if(ESP_PLATFORM)
idf_component_register(
    INCLUDE_DIRS "."
    REQUIRES result
)
else()
    add_library(ble_hal INTERFACE)
    target_include_directories(ble_hal INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_features(ble_hal INTERFACE cxx_std_23)
endif()
