#include "pico.h"
#include "pico/stdlib.h"
#include "pico/scanvideo.h"
#include "pico/scanvideo/composable_scanline.h"
#include "pico/multicore.h"
#include "hardware/clocks.h"

#include <stdio.h>
#include <string.h>

#include "Framebuffer/FramebufferFunctions.h"
#include "OS/CluatronOS.h"

#include "tusb.h"

void core0Func()
{
    tuh_init(BOARD_TUH_RHPORT);
    tusb_init();

    RunCluatronOS();
}

void core1Func()
{
    memset(fbuf, 0, sizeof(fbuf));
    set_colours(colours);

    setup_video();
    render_loop();
}


int main (void)
{
    set_sys_clock_khz (250000, true);
    stdio_init_all();

    multicore_launch_core1(core1Func);    

    core0Func();
}
