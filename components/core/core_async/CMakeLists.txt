if(ESP_PLATFORM)
    idf_component_register(
        INCLUDE_DIRS "."
    )
else()
    add_library(core_async INTERFACE)
    target_include_directories(core_async INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_features(core_async INTERFACE cxx_std_23)
endif()
