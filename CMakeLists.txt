cmake_minimum_required(VERSION 3.16)

if(ESP_PLATFORM)
    set(PONS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../pons)
    set(EXTRA_COMPONENT_DIRS
        ${PONS_DIR}/core/core_async
        ${PONS_DIR}/core/result
        ${PONS_DIR}/core/logger
        ${PONS_DIR}/core/logger/console_logger
        ${PONS_DIR}/core/store
        ${PONS_DIR}/hal/esp32/esp32_async
        ${PONS_DIR}/hal/esp32/esp32_store/nvs_store
        ${PONS_DIR}/hal/esp32/esp32_ble_hal
        ${PONS_DIR}/hal/esp32/esp32_serial_hal
        ${PONS_DIR}/hal/esp32/esp32_http_client/
        ${PONS_DIR}/hal/ble_hal
        ${PONS_DIR}/hal/serial_hal
        ${PONS_DIR}/hal/http_client
        ${PONS_DIR}/platform/${PLATFORM}/platform_async
        ${PONS_DIR}/transport
        ${PONS_DIR}/transport/chunked_transport
        ${PONS_DIR}/transport/chunked_transport/ble_transport
        ${PONS_DIR}/transport/chunked_transport/serial_transport
        components/communication
    )
    add_compile_definitions(PLATFORM_FREERTOS)
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    project(communication)
else()
    project(communication CXX)
    add_subdirectory(../../pons ${CMAKE_BINARY_DIR}/pons)
    add_subdirectory(components/communication)
    add_subdirectory(main)
endif()
