if(ESP_PLATFORM)
idf_component_register(
    INCLUDE_DIRS "."
    REQUIRES result
)
else()
    add_library(store INTERFACE)
    target_include_directories(store INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_features(store INTERFACE cxx_std_23)
endif()
