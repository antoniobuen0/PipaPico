# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13)

# Initialize the SDK
include(pico_sdk_import.cmake)

# Set cross-compilation before project()
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

# Initialize project
project(PipaPico C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the Pico SDK
pico_sdk_init()

# Create executable
add_executable(PipaPico
    main.c
    median_filter.c
    detection.c
    symmetry.c
    debug_comm.c
)

# Include header directories
target_include_directories(PipaPico PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# Link necessary libraries
target_link_libraries(PipaPico
    pico_stdlib
    hardware_uart
    hardware_i2c
    hardware_pwm
    hardware_gpio
    hardware_sync
    pico_multicore
)

# Enable USB output, disable UART output
pico_enable_stdio_usb(PipaPico 1)
pico_enable_stdio_uart(PipaPico 0)

# Create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(PipaPico)

# Optional: configure for debugging
pico_set_program_name(PipaPico "PipaPico")
pico_set_program_version(PipaPico "1.0")

# Add configurations for testing and validation steps
enable_testing()
add_subdirectory(tests)
