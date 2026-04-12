cmake_minimum_required(VERSION 3.16)
if(ESP_PLATFORM)
    set(PONS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../pons)
    set(EXTRA_COMPONENT_DIRS
        ${PONS_DIR}/core/result
        ${PONS_DIR}/core/logger
        ${PONS_DIR}/core/logger/console_logger
        ${PONS_DIR}/core/serializer
        ${PONS_DIR}/core/mutex
        ${PONS_DIR}/core/semaphore
        ${PONS_DIR}/platform
        ${PONS_DIR}/hal/store
        ${PONS_DIR}/hal/esp32/esp32_store/nvs_store
        ${PONS_DIR}/hal/esp32/esp32_ble_hal
        ${PONS_DIR}/hal/esp32/esp32_serial_hal
        ${PONS_DIR}/hal/esp32/esp32_http_client
        ${PONS_DIR}/hal/ble_hal
        ${PONS_DIR}/hal/serial_hal
        ${PONS_DIR}/hal/http_client
        ${PONS_DIR}/transport
        ${PONS_DIR}/transport/dispatcher
        ${PONS_DIR}/transport/requester
        ${PONS_DIR}/transport/transporter
        ${PONS_DIR}/transport/transporter/chunked_transporter
        ${PONS_DIR}/transport/transporter/direct_transporter
        ${PONS_DIR}/transport/transporter/serial_transporter
        ${CMAKE_CURRENT_SOURCE_DIR}/models
        ${CMAKE_CURRENT_SOURCE_DIR}/components/handlers
        ${CMAKE_CURRENT_SOURCE_DIR}/components/handlers/scd40_handler
        ${CMAKE_CURRENT_SOURCE_DIR}/components/communication
    )
    add_compile_definitions(PLATFORM_FREERTOS)
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    project(communication)
else()
    project(communication CXX)
    add_subdirectory(../../pons ${CMAKE_BINARY_DIR}/pons)
    add_subdirectory(models)
    add_subdirectory(components/handlers)
    add_subdirectory(components/handlers/scd)
    add_subdirectory(components/handlers/sps)
    add_subdirectory(components/communication)
    add_subdirectory(main)
endif()
