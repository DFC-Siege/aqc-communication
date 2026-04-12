if(ESP_PLATFORM)
    idf_component_register(
        INCLUDE_DIRS "."
    )
else()
    add_library(sps INTERFACE)
    target_include_directories(sps INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
endif()
