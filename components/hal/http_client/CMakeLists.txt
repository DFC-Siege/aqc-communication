if(ESP_PLATFORM)
idf_component_register(
    INCLUDE_DIRS "."
)
else()
    add_library(http_client INTERFACE)
    target_include_directories(http_client INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_features(http_client INTERFACE cxx_std_23)
endif()
