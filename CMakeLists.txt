cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS
    components/core/result
    components/core/async
    components/core/logger
    components/core/logger/console_logger
    components/core/store
    components/hal/esp32/logger/idf_logger
    components/hal/esp32/ble_hal
    components/hal/esp32/serial_hal
    components/hal/esp32/store/nvs_store
    components/transport
    components/transport/chunked_transport
    components/transport/chunked_transport/ble_transport
    components/transport/chunked_transport/serial_transport
    components/communication
)

set(PROJECT_NAME communication)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(${PROJECT_NAME})
