if(ESP_PLATFORM)
idf_component_register(
    SRCS "logger.cpp"
    INCLUDE_DIRS "."
)
else()
    add_library(logger logger.cpp)
    target_include_directories(logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(logger PUBLIC logger)
    target_compile_features(logger PUBLIC cxx_std_23)
endif()
