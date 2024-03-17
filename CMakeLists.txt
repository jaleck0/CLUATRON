cmake_minimum_required(VERSION 3.12)

#Pull in PICO SDK (must be before project)
include(pico_sdk_import.cmake)

project(blink C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

add_executable(cluatron
		src/main.c
		)
		
# pull in our pico_stdlib
target_link_libraries(cluatron pico_stdlib)

#create map/bin/hex file etc.
pico_add_extra_outputs(cluatron)
