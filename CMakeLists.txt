cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(KOS_BASE /opt/toolchains/dc/kos)
set(KOS_CC_BASE /opt/toolchains/dc/sh-elf)
set(KOS_SUBARCH pristine)
set(KOS_PORTS ${KOS_BASE}/../kos-ports)
include_directories(${KOS_PORTS}/include/raylib)
include(${KOS_BASE}/utils/cmake/dreamcast.toolchain.cmake)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(ChaoGen C)

set(CMAKE_C_STANDARD 23)

SET (SRCS main.c

)

add_executable(ChaoGen ${SRCS})
target_link_libraries(ChaoGen raylib GL m z ADX)

kos_add_romdisk(ChaoGen romdisk)