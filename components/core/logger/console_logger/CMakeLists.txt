if(ESP_PLATFORM)
idf_component_register(
    SRCS "console_logger.cpp"
    INCLUDE_DIRS "."
    REQUIRES logger
)
else()

endif()
