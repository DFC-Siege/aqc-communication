if(ESP_PLATFORM)
    idf_component_register(
        INCLUDE_DIRS "."
        REQUIRES serializer result dispatcher
    )
else()
    add_library(models INTERFACE)
    target_include_directories(models INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(models INTERFACE serializer result dispatcher)
endif()
