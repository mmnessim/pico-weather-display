cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_BOARD pico2_w CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(weather-display C CXX ASM)

pico_sdk_init()

file(GLOB_RECURSE SOURCES "src/*.c")
add_subdirectory(lib/Config)
add_subdirectory(lib/e-Paper)
add_subdirectory(lib/Fonts)
add_subdirectory(lib/GUI)

include_directories(./lib/Config)
include_directories(./lib/GUI)

add_executable(weather-display ${SOURCES})

pico_enable_stdio_uart(weather-display 0)
pico_enable_stdio_usb(weather-display 1)

target_include_directories(weather-display PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/lib/Config
    ${CMAKE_CURRENT_LIST_DIR}/lib/e-Paper
    ${CMAKE_CURRENT_LIST_DIR}/lib/Fonts
    ${CMAKE_CURRENT_LIST_DIR}/lib/GUI
)

target_link_libraries(weather-display
    pico_stdlib
    hardware_i2c
    pico_cyw43_arch_lwip_threadsafe_background
)

#Stuff for e-ink display

target_link_libraries(weather-display
    ePaper
    GUI
    Fonts
    Config
    hardware_spi)

pico_add_extra_outputs(weather-display)
