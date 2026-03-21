cmake_minimum_required(VERSION 3.16)

set(PLATFORM "esp32" CACHE STRING "Target platform (esp32, stm32, posix)")

if(ESP_PLATFORM)
    set(EXTRA_COMPONENT_DIRS
        components/core/core_async
        components/core/result
        components/core/logger
        components/core/logger/console_logger
        components/core/store
        components/hal/esp32/esp32_logger
        components/hal/esp32/esp32_store/nvs_store
        components/hal/esp32/esp32_async
        components/hal/esp32/esp32_ble_hal
        components/hal/esp32/esp32_serial_hal
        components/hal/esp32/esp32_http_client
        components/hal/ble_hal
        components/hal/serial_hal
        components/hal/http_client
        components/platform/${PLATFORM}/platform_async
        components/transport
        components/transport/chunked_transport
        components/transport/chunked_transport/ble_transport
        components/transport/chunked_transport/serial_transport
        components/communication
    )
    add_compile_definitions(PLATFORM_FREERTOS)
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    project(communication)
else()
    project(communication CXX)
    add_subdirectory(components/core/core_async)
    add_subdirectory(components/core/result)
    add_subdirectory(components/core/logger)
    add_subdirectory(components/core/logger/console_logger)
    add_subdirectory(components/hal/${PLATFORM}/${PLATFORM}_async)
    add_subdirectory(components/platform/${PLATFORM}/platform_async)
    add_subdirectory(components/transport)
    add_subdirectory(components/transport/chunked_transport)
    add_subdirectory(main)
endif()
