if(ESP_PLATFORM)
idf_component_register(
    INCLUDE_DIRS "."
    REQUIRES freertos core_async
)
else()
    add_library(esp32_async INTERFACE)
    target_include_directories(esp32_async INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_features(esp32_async INTERFACE cxx_std_23)
endif()
