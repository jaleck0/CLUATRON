#include "pico.h"
#include "pico/stdlib.h"
#include "pico/scanvideo.h"
#include "pico/scanvideo/composable_scanline.h"
#include "pico/multicore.h"
#include "hardware/clocks.h"

#include <stdio.h>
#include <string.h>

#include "FramebufferFunctions.h"


int main (void)
{
    set_sys_clock_khz (200000, true);
    stdio_init_all();

    memset (fbuf, 0, sizeof(fbuf));
    set_colours(colours);

    setup_video();
    multicore_launch_core1(core1_entry);
    render_loop();
}
