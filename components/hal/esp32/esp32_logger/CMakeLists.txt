if(ESP_PLATFORM)
idf_component_register(
    SRCS "logger.cpp"
    INCLUDE_DIRS "."
    REQUIRES logger log esp_system
)
else()

endif()
