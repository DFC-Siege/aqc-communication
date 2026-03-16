cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS
    components/core/result
    components/core/async
    components/core/logger
    components/core/logger/console_logger
    components/core/store
    components/hal/esp32/logger/idf_logger
    components/hal/ble_hal
    components/hal/serial_hal
    components/hal/esp32/esp32_ble_hal
    components/hal/esp32/esp32_serial_hal
    components/hal/esp32/store/nvs_store
    components/transport
    components/transport/chunked_transport
    components/transport/chunked_transport/ble_transport
    components/transport/chunked_transport/serial_transport
    components/communication
)

if(ESP_PLATFORM)
    add_compile_definitions(PLATFORM_FREERTOS)
endif()

set(PROJECT_NAME communication)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(${PROJECT_NAME})
