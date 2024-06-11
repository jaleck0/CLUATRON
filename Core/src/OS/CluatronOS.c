#include "CluatronOS.h"
#include "Commands.h"
#include "../Framebuffer/FramebufferFunctions.h"
#include "../Graphics/GraphicsFunctions.h"
#include "../Graphics/font4x6.h"
#include "../Terminal/Terminal.h"
#include "../USBInput/Input.h"

#include <string.h>
#include <stdlib.h>

#include "hardware/watchdog.h"
#include "pico/stdlib.h"
#include "pico/bootrom.h"

#include "../lua/lua.h" 
#include "../lua/lualib.h"
#include "../lua/lauxlib.h"
#include "../lua/lualib.h"

#define PROMPT "> "

char* startCommand = "function start() read_input() if keyh(41) == 1 then error(\"stopped\",2) end end debug.sethook(start,\"l\")";

typedef enum
{
    SHELL,
    CET
} Program;

Program currentProgram = SHELL;


uint16_t pico8pal[16] =
{
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(  0u,   0u,   0u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(29u,   43u,   83u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(126u, 37u,   83u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(0u, 135u,   81u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(  171u,   82u, 54u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8( 95u,  87u,  64u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8( 194u, 195u, 199u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(255u, 241u, 232u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8( 255u,  0u,  77u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(255u,   163u,   0u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(  255u, 236u,   39u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(0u, 228u,   54u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(  41u,   173u, 255u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(131u,   118u, 156u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(255u, 119u, 168u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(255u, 204u, 170u)
};

uint16_t cgapal[16] =
{
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(  0u,   0u,   0u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(0u,   0u,   170u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(0u, 170u,   0u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(0u, 170u,   170u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(  170u,   0u, 0u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8( 170u,  0u,  170u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8( 170u, 85u, 0u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(170u, 170u, 170u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8( 85u,  85u,  85u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(85u,   85u,   255u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(  85u, 255u,   85u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(85u, 255u,   255u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(  255u,   85u, 85u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(255u,   85u, 255u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(255u, 255u, 85u),
    PICO_SCANVIDEO_PIXEL_FROM_RGB8(255u, 255u, 255u)
};

void core1_entry() 
{
    SetFont(font4x6);
    board_init();
    cls(1);
    SetTextColor(11);
    TerminalSetBackCol(1);
    TerminalPutString("                        ---CLUATRON OS ~ Jalecko 2024---\n");

    InitInputs();

    lua_State* L;
    luaL_Buffer buf;
    int status;
    size_t len;
    char ch;

    stdio_init_all();

    L = luaL_newstate();
    luaL_openlibs(L);
    
    luaL_buffinit(L, &buf);

    RegisterCommands(L);
    //RegisterConstants(L);
    /*
    type help() for more information

    ctrl-c      stop program 
    ctrl-l      clear screen
    ctrl-enter  run current code
    enter       run command
    esc         pause program loop
    */
    TerminalPutString("\n ctrl-e  execute file\n ctrl-l  clear screen\n enter   execute buffer\n esc     stop program\n\n");
    TerminalPutString(PROMPT);

    uint8_t vis = 0;
    uint32_t mes = 0;
    uint32_t t = 0;

    luaL_addlstring(&buf, startCommand, strlen(startCommand));
    luaL_pushresult(&buf);
    const char *start = lua_tolstring(L, -1, &len);
    status = luaL_loadbuffer(L, start, len, "startup");
    status = lua_pcall(L, 0, 0, 0);
    luaL_buffinit(L, &buf);

    char command[78] = "";
    uint32_t commandPos = 0;

    char commandHistory[10][78] = {"","","","","","","","","",""};
    int32_t commandWriteIndex = 0;
    int32_t commandSelectIndex = 0;

    char file[32768] = "";
    uint32_t filePos = 0;

    for(;;)
    {
        ReadInputs();

        ch = (char)KeyboardGiveLetter();

        if(ch == '\r') 
        {
            ch = '\n';
        }

        switch (currentProgram)
        {
        case SHELL:
            if (KeyboardGetPressed(USB_BACKSPACE))
            {
                if (strlen(command) > 0 && commandPos > 0)
                {
                    strcpy(command+commandPos-1,command+commandPos);
                    commandPos--;
                    TerminalPutCommand(command);
                    TerminalMove(commandPos-strlen(command));
                }
            }
            if (KeyboardGetPressed(USB_DELETE))
            {
                if (command[commandPos] != '\0')
                {
                    memmove(command+commandPos,command+commandPos+1,strlen(command+commandPos)+1);
                    TerminalPutCommand(command);
                    TerminalMove(commandPos-strlen(command));
                }
            }
            else if (KeyboardGetPressed(USB_L) && KeyboardGetCtrl())
            {
                TerminalClear();
                TerminalPutString(PROMPT);
            }
            else if (KeyboardGetPressed(USB_E) && KeyboardGetCtrl())
            {
                TerminalPutCharacter('\n');
                buf.b = file;
                buf.n = strlen(file);
                luaL_pushresult(&buf);
                const char *s = lua_tolstring(L, -1, &len);
                status = luaL_loadbuffer(L, s, len, "command");

                
                filePos=0;

                if(status != LUA_OK) 
                {
                    const char *msg = lua_tostring(L, -1);
                    TerminalPutString("parse error: ");
                    TerminalPutString(msg);
                    TerminalPutString("\n");
                }
                else
                {
                    status = lua_pcall(L, 0, 0, 0);

                    if(status != LUA_OK) 
                    {
                        const char *msg = lua_tostring(L, -1);
                        TerminalPutString("execute error: ");
                        TerminalPutString(msg);
                        TerminalPutString("\n");
                        //lua_writestringerror("execute error: %s\n", msg);
                    }
                }

                lua_pop(L, 1);
                luaL_buffinit(L, &buf);
                TerminalPutString(PROMPT);
            }
            else if (KeyboardGetPressed(USB_LEFT) && commandPos > 0)
            {
                commandPos--;
                TerminalMove(-1);
            }
            else if (KeyboardGetPressed(USB_RIGHT) && commandPos < strlen(command))
            {
                commandPos++;
                TerminalMove(1);
            }
            else if (KeyboardGetPressed(USB_UP))
            {
                commandSelectIndex--;

                if (commandSelectIndex < 0) 
                {
                    commandSelectIndex = 9;
                }

                if (commandHistory[commandSelectIndex][0] == '\0')
                {
                    commandSelectIndex++;
                    commandSelectIndex %= 10;
                }

                strcpy(command, commandHistory[commandSelectIndex]);
                TerminalPutCommand(command);
                commandPos = strlen(command);
            }
            else if (KeyboardGetPressed(USB_DOWN))
            {
                if (commandSelectIndex != commandWriteIndex)
                {            
                    commandSelectIndex++;
                }

                if (commandSelectIndex > 9) 
                {
                    commandSelectIndex = 0;
                }            

                strcpy(command, commandHistory[commandSelectIndex]);
                TerminalPutCommand(command);
                commandPos = strlen(command);
            }
            else if (KeyboardGetPressed(USB_ENTER))
            {
                TerminalPutCharacter('\n');
                
                strcpy( commandHistory[commandWriteIndex], command);
                commandWriteIndex++;
                commandWriteIndex %= 10;
                commandSelectIndex = commandWriteIndex;

                buf.b = command;
                buf.n = strlen(command);
                luaL_pushresult(&buf);
                const char *s = lua_tolstring(L, -1, &len);
                status = luaL_loadbuffer(L, s, len, "command");

                command[0]='\0';
                commandPos=0;

                if(status != LUA_OK) 
                {
                    const char *msg = lua_tostring(L, -1);
                    TerminalPutString("parse error: ");
                    TerminalPutString(msg);
                    TerminalPutString("\n");
                }
                else
                {
                    status = lua_pcall(L, 0, 0, 0);

                    if(status != LUA_OK) 
                    {
                        const char *msg = lua_tostring(L, -1);
                        TerminalPutString("execute error: ");
                        TerminalPutString(msg);
                        TerminalPutString("\n");
                        //lua_writestringerror("execute error: %s\n", msg);
                    }
                }

                lua_pop(L, 1);
                luaL_buffinit(L, &buf);
                TerminalPutString(PROMPT);
            }
            else if((ch >= 0x20 && ch < 0x7F)) 
            { 
                if (strlen(command) < 77)
                {   
                    memmove(command+commandPos+1,command+commandPos,strlen(command+commandPos)+1);
                    command[commandPos] = ch;
                    commandPos++;
                    TerminalPutCommand(command);
                    TerminalMove(commandPos-strlen(command));
                }
            }
            break;
        case CET:
            
            if (KeyboardGetPressed(USB_BACKSPACE))
            {
                if (strlen(file) > 0 && filePos > 0)
                {
                    strcpy(file+filePos-1,file+filePos);
                    filePos--;
                    TerminalClear();
                    TerminalPutString(file);
                    TerminalMove(filePos-strlen(file));
                }
            }
            if (KeyboardGetPressed(USB_DELETE))
            {
                if (file[filePos] != '\0')
                {
                    memmove(file+filePos,file+filePos+1,strlen(file+filePos)+1);
                    TerminalClear();
                    TerminalPutString(file);
                    TerminalMove(filePos-strlen(file));
                }
            }
            else if (KeyboardGetPressed(USB_LEFT) && filePos > 0)
            {
                filePos--;
                TerminalMove(-1);
            }
            else if (KeyboardGetPressed(USB_RIGHT) && filePos < strlen(file))
            {
                filePos++;
                TerminalMove(1);
            }
            else if (KeyboardGetPressed(USB_UP))
            {
               
            }
            else if (KeyboardGetPressed(USB_DOWN))
            {
                
            }
            else if (KeyboardGetPressed(USB_ENTER))
            {
            
            }
            else if((ch >= 0x20 && ch < 0x7F)) 
            { 
                if (strlen(file) < 32767)
                {   
                    memmove(file+filePos+1,file+filePos,strlen(file+filePos)+1);
                    file[filePos] = ch;
                    filePos++;
                    TerminalClear();
                    TerminalPutString(file);
                    TerminalMove(filePos-strlen(file));
                }
            }
            break;
        default:
            break;
        }

        

        if (KeyboardGetPressed(USB_F1))
        {
            for(uint8_t i = 0; i<16; i++)
            {
                ChangeColor( i, pico8pal[i]);
            }
            set_colours();
            cls(1);
            currentProgram = SHELL;
            TerminalSetPosition(0,0);
            TerminalPutCommand(command);
            commandPos=strlen(command);
            SetTextColor(11);
            
        }

        if (KeyboardGetPressed(USB_F2))
        {
            for(uint8_t i = 0; i<16; i++)
            {
                ChangeColor( i, cgapal[i]);
            }
            set_colours();
            cls(1);
            currentProgram = CET;
            TerminalSetPosition(0,0);
            TerminalClear();
            TerminalPutString(file);
            filePos=strlen(file);
            SetTextColor(15);
            
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
