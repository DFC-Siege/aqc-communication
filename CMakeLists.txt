cmake_minimum_required(VERSION 3.16)

set(PLATFORM "esp32" CACHE STRING "Target platform (esp32, stm32, posix)")

set(EXTRA_COMPONENT_DIRS
    # core — platform agnostic
    components/core/core_async
    components/core/result
    components/core/logger
    components/core/logger/console_logger
    components/core/store

    # hal — concrete implementations
    components/hal/esp32/esp32_logger
    components/hal/esp32/esp32_store/nvs_store
    components/hal/esp32/esp32_async
    components/hal/esp32/esp32_ble_hal
    components/hal/esp32/esp32_serial_hal
    components/hal/esp32/esp32_http_client
    components/hal/ble_hal
    components/hal/serial_hal
    components/hal/http_client

    # platform — maps core concepts to hal implementations
    components/platform/${PLATFORM}/platform_async

    # transport
    components/transport
    components/transport/chunked_transport
    components/transport/chunked_transport/ble_transport
    components/transport/chunked_transport/serial_transport

    # application
    components/communication
)

if(ESP_PLATFORM)
    add_compile_definitions(PLATFORM_FREERTOS)
endif()

set(PROJECT_NAME communication)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(${PROJECT_NAME})
