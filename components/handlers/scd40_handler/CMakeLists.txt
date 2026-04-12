if(ESP_PLATFORM)
    idf_component_register(
        INCLUDE_DIRS "."
    )
else()
    add_library(scd INTERFACE)
    target_include_directories(scd INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
endif()
