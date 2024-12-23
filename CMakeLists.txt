cmake_minimum_required(VERSION 3.31)

project(psxe VERSION 1.0 LANGUAGES C)

add_executable(
    ${CMAKE_PROJECT_NAME} WIN32
    source/frontend/argparse.c
    source/frontend/config.c
    source/frontend/screen.c
    source/frontend/toml.c

    source/psx/bus.c
    source/psx/config.c
    source/psx/cpu.c
    source/psx/exe.c
    source/psx/log.c
    source/psx/psx.c

    source/psx/dev/bios.c
    source/psx/dev/dma.c
    source/psx/dev/exp1.c
    source/psx/dev/exp2.c
    source/psx/dev/gpu.c
    source/psx/dev/ic.c
    source/psx/dev/input.c
    source/psx/dev/mc1.c
    source/psx/dev/mc2.c
    source/psx/dev/mc3.c
    source/psx/dev/mcd.c
    source/psx/dev/mdec.c
    source/psx/dev/pad.c
    source/psx/dev/ram.c
    source/psx/dev/scratchpad.c
    source/psx/dev/spu.c
    source/psx/dev/timer.c
    source/psx/dev/xa.c

    source/psx/dev/cdrom/audio.c
    source/psx/dev/cdrom/cdrom.c
    source/psx/dev/cdrom/cue.c
    source/psx/dev/cdrom/disc.c
    source/psx/dev/cdrom/impl.c
    source/psx/dev/cdrom/list.c
    source/psx/dev/cdrom/queue.c

    source/psx/input/guncon.c
    source/psx/input/sda.c

    source/main.c
)

set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY C_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE")

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    include
    ${SDL3_INCLUDES}    
)
add_subdirectory(externals/SDL3 EXCLUDE_FROM_ALL)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE SDL3::SDL3)