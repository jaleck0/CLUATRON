#include "CluatronOS.h"
#include "../Framebuffer/FramebufferFunctions.h"
#include "../Graphics/GraphicsFunctions.h"
#include "../Graphics/font4x6.h"
#include "../Terminal/Terminal.h"
#include "../USBInput/Input.h"

#include "CluaShell.h"
#include "CluatronEditingTool.h"
#include "SharedOS.h"

#include <string.h>
#include <stdlib.h>

#include "hardware/watchdog.h"
#include "pico/stdlib.h"
#include "pico/bootrom.h"


void RunCluatronOS() 
{
    SetFont(font4x6);

    cls(BACKCOL);
    SetTextColor(TEXTCOL);
    TerminalSetBackCol(BACKCOL);

    

    uint8_t vis = 0;
    uint32_t mes = 0;
    uint32_t t = 0;

    InitShell();

    for(;;)
    {
        switch (currentProgram)
        {
        case SHELL:
            
            RunShell();
            
            break;
        case CET:

            RunCET();

            break;
        default:
            break;
        }

        
        if (to_ms_since_boot(get_absolute_time()) - mes >= 500)
        {
            mes = to_ms_since_boot(get_absolute_time());

            if(vis)
            {
                DrawWritePoint();
                vis = 0;
            }
            else
            {
                RemoveWritePoint();
                vis = 1;
            }
        }

    }
}
