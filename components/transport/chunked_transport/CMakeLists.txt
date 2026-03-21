if(ESP_PLATFORM)
    idf_component_register(
        SRCS
            "chunked_receiver.cpp"
            "chunked_sender.cpp"
            "chunked_transporter.cpp"
        INCLUDE_DIRS "."
        REQUIRES result logger transport platform_async core_async
    )
else()
    add_library(chunked_transport chunked_receiver.cpp chunked_sender.cpp chunked_transporter.cpp)
    target_include_directories(chunked_transport PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(chunked_transport PUBLIC transport result platform_async core_async)
    target_compile_features(chunked_transport PUBLIC cxx_std_23)
endif()
