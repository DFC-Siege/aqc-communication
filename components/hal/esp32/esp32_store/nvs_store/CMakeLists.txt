if(ESP_PLATFORM)
idf_component_register(
    SRCS "nvs_store.cpp"
    INCLUDE_DIRS "."
    REQUIRES result logger nvs_flash store
)
else()

endif()
