cmake_minimum_required(VERSION 3.12)
project(portaudio-examples LANGUAGES C)

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PORTAUDIO portaudio-2.0 REQUIRED)

include_directories(${PORTAUDIO_INCLUDE_DIRS})

add_executable(basic_io_connection basic_io_connection.c)
target_link_libraries(basic_io_connection ${PORTAUDIO_LIBRARIES})

add_executable(devices_info devices_info.c)
target_link_libraries(devices_info ${PORTAUDIO_LIBRARIES})

enable_testing()
add_test(NAME test_device_info COMMAND $<TARGET_NAME:devices_info>)
