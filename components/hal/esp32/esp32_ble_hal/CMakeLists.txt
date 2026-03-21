if(ESP_PLATFORM)
idf_component_register(
    SRCS "ble_hal.cpp"
    INCLUDE_DIRS "."
    REQUIRES result logger bt ble_hal
)
else()
    add_library(esp32_ble_hal ble_hal.cpp)
    target_include_directories(esp32_ble_hal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(esp32_ble_hal PUBLIC logger)
    target_compile_features(esp32_ble_hal PUBLIC cxx_std_23)
endif()
