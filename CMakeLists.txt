cmake_minimum_required(VERSION 3.13)
set(PICO_BOARD vgaboard)

set(FAMILY rp2040)
set(BOARD pico_sdk)
set(TINYUSB_FAMILY_PROJECT_NAME_PREFIX "tinyusb_host_")

include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)

project(cluatron_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(cluatron
	src/main.c
	src/FramebufferFunctions.c
	src/GraphicsFunctions.c
	src/Terminal.c
	src/hid_app.c
	src/cdc_app.c
	src/Input.c
	src/CluatronOS.c
	src/font4x6.c
	src/lapi.c
    src/lauxlib.c
    src/lbaselib.c
    src/lcode.c
    src/lcorolib.c
    src/lctype.c
    src/ldblib.c
    src/ldebug.c
    src/ldo.c
    src/ldump.c
    src/lfunc.c
    src/lgc.c
    src/linit.c
    src/liolib.c
    src/llex.c
    src/lmathlib.c
    src/lmem.c
    src/loadlib.c
    src/lobject.c
    src/lopcodes.c
    src/loslib.c
    src/lparser.c
    src/lstate.c
    src/lstring.c
    src/lstrlib.c
    src/ltable.c
    src/ltablib.c
    src/ltm.c
    src/lundump.c
    src/lutf8lib.c
    src/lvm.c
    src/lzio.c
    src/Terminal.c
)

target_include_directories(cluatron PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

target_compile_definitions(cluatron PUBLIC -DPICO_SCANVIDEO_MAX_SCANLINE_BUFFER_WORDS=322
	-DPICO_SCANVIDEO_SCANLINE_BUFFER_COUNT=8)

pico_enable_stdio_uart(cluatron 0)
pico_enable_stdio_usb(cluatron 0)

pico_add_extra_outputs(cluatron)

target_link_libraries(cluatron PUBLIC
	pico_stdlib	
	pico_scanvideo_dpi
	pico_multicore
	hardware_interp
	tinyusb_host 
	tinyusb_board)
