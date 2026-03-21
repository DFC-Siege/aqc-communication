if(ESP_PLATFORM)
idf_component_register(
    INCLUDE_DIRS "."
    REQUIRES core_async esp32_async
)
else()
    add_library(platform_async INTERFACE)
    target_include_directories(platform_async INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_features(platform_async INTERFACE cxx_std_23)
endif()
