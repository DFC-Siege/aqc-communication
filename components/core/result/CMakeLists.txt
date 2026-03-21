if(ESP_PLATFORM)
idf_component_register(
        INCLUDE_DIRS "."
)
else()
    add_library(result INTERFACE)
    target_include_directories(result INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_features(result INTERFACE cxx_std_23)
endif()
