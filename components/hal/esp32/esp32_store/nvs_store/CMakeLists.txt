if(ESP_PLATFORM)
idf_component_register(
    SRCS "nvs_store.cpp"
    INCLUDE_DIRS "."
    REQUIRES result logger nvs_flash store
)
else()
    add_library(nvs_store nvs_store.cpp)
    target_include_directories(nvs_store PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(nvs_store PUBLIC nvs_store)
    target_compile_features(nvs_store PUBLIC cxx_std_23)
endif()
