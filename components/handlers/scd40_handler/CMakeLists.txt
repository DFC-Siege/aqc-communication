if(ESP_PLATFORM)
    idf_component_register(
        INCLUDE_DIRS "."
        REQUIRES models
    )
else()
    add_library(scd40_handler INTERFACE)
    target_include_directories(scd40_handler INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(scd40_handler INTERFACE models)
endif()
