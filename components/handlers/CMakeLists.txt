if(ESP_PLATFORM)
    idf_component_register(
        INCLUDE_DIRS "."
        REQUIRES scd40_handler sps30_handler
    )
else()
    add_library(handlers INTERFACE)
    target_include_directories(handlers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
endif()
