if(ESP_PLATFORM)
idf_component_register(
    SRCS "http_client.cpp"
    INCLUDE_DIRS "."
    REQUIRES esp_http_client result http_client
)
else()
    add_library(esp_http_client http_client.cpp)
    target_include_directories(esp_http_client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(esp_http_client PUBLIC logger)
    target_compile_features(esp_http_client PUBLIC cxx_std_23)
endif()
