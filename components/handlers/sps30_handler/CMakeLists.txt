if(ESP_PLATFORM)
    idf_component_register(
        INCLUDE_DIRS "."
        REQUIRES models
    )
else()
    add_library(sps30_handler INTERFACE)
    target_include_directories(sps30_handler INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(sps30_handler INTERFACE models)
endif()
