cmake_minimum_required(VERSION 3.13)

set(PICO_SDK_PATH "/Users/anto/pico/pico-sdk")
include(${PICO_SDK_PATH}/pico_sdk_import.cmake)

project(PipaPico C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(PipaPico
    main.c
    median_filter.c
    detection.c
    symmetry.c
    debug_comm.c
)

target_include_directories(PipaPico PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(PipaPico 
    pico_stdlib        # stdio, sleep, etc.
    hardware_uart      # Para UART (si se usa)
    hardware_sync      # critical_section
    hardware_dma
    hardware_i2c
    hardware_pio
    pico_multicore     # multicore
)

pico_set_program_name(PipaPico "pipas_main")
pico_set_program_version(PipaPico "1.0")

pico_enable_stdio_usb(PipaPico 1)
pico_enable_stdio_uart(PipaPico 0)

pico_add_extra_outputs(PipaPico)