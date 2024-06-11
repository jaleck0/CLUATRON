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
	src/Framebuffer/FramebufferFunctions.c
	src/Graphics/GraphicsFunctions.c
	src/Terminal/Terminal.c
	src/USBInput/hid_app.c
	src/USBInput/cdc_app.c
	src/USBInput/Input.c
	src/OS/Commands.c
    src/OS/CluatronOS.c
	src/Graphics/font4x6.c
	src/lua/lapi.c
    src/lua/lauxlib.c
    src/lua/lbaselib.c
    src/lua/lcode.c
    src/lua/lcorolib.c
    src/lua/lctype.c
    src/lua/ldblib.c
    src/lua/ldebug.c
    src/lua/ldo.c
    src/lua/ldump.c
    src/lua/lfunc.c
    src/lua/lgc.c
    src/lua/linit.c
    src/lua/liolib.c
    src/lua/llex.c
    src/lua/lmathlib.c
    src/lua/lmem.c
    src/lua/loadlib.c
    src/lua/lobject.c
    src/lua/lopcodes.c
    src/lua/loslib.c
    src/lua/lparser.c
    src/lua/lstate.c
    src/lua/lstring.c
    src/lua/lstrlib.c
    src/lua/ltable.c
    src/lua/ltablib.c
    src/lua/ltm.c
    src/lua/lundump.c
    src/lua/lutf8lib.c
    src/lua/lvm.c
    src/lua/lzio.c
)

target_include_directories(cluatron PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

target_compile_definitions(cluatron PUBLIC -DPICO_SCANVIDEO_MAX_SCANLINE_BUFFER_WORDS=322
	-DPICO_SCANVIDEO_SCANLINE_BUFFER_COUNT=8)

pico_enable_stdio_uart(cluatron 0)
pico_enable_stdio_usb(cluatron 0)

pico_set_float_implementation(cluatron pico)
pico_set_double_implementation(cluatron compiler) #this seems to improve stuff so far

pico_add_extra_outputs(cluatron)

target_link_libraries(cluatron PUBLIC
	pico_stdlib	
	pico_scanvideo_dpi
	pico_multicore
	hardware_interp
	tinyusb_host 
	tinyusb_board)
