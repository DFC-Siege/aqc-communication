cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS
    components/core/result
    components/core/async
    components/core/logger
    components/core/logger/idf_logger
    components/core/logger/serial_logger
    components/hal/storage/nvs_store
    components/hal/ble_hal
    components/hal/serial_hal
    components/hal/storage/nvs_store
    components/transport
    components/transport/chunked_transport
    components/transport/chunked_transport/ble_transport
    components/communication
)

set(PROJECT_NAME communication)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(${PROJECT_NAME})
