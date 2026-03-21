if(ESP_PLATFORM)
idf_component_register(
    SRCS "logger.cpp"
    INCLUDE_DIRS "."
)
else()

endif()
