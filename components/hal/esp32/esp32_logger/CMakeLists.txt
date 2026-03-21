if(ESP_PLATFORM)
idf_component_register(
    SRCS "logger.cpp"
    INCLUDE_DIRS "."
    REQUIRES logger log esp_system
)
else()
    add_library(esp32_logger logger.cpp)
    target_include_directories(esp32_logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(esp32_logger PUBLIC logger)
    target_compile_features(esp32_logger PUBLIC cxx_std_23)
endif()
