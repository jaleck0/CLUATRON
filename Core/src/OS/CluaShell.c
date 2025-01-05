#include "CluaShell.h"
#include "../USBInput/Input.h"
#include "SharedOS.h"
#include "Commands.h"

#include "../lua/lua.h"
#include "../lua/lualib.h"
#include "../lua/lauxlib.h"
#include "../lua/lualib.h"

#include <string.h>

char commandHistory[10][78] = {"", "", "", "", "", "", "", "", "", ""};
int32_t commandWriteIndex = 0;
int32_t commandSelectIndex = 0;

char *startCommand = "function start() read_input() if keyh(41) == 1 then error(\"stopped\",2) end end debug.sethook(start,\"l\")";

void InitShell()
{
    L = luaL_newstate();
    luaL_openlibs(L);
    luaL_buffinit(L, &buf);
    RegisterCommands(L);

    luaL_addlstring(&buf, startCommand, strlen(startCommand));
    luaL_pushresult(&buf);
    const char *start = lua_tolstring(L, -1, &len);
    status = luaL_loadbuffer(L, start, len, "startup");
    status = lua_pcall(L, 0, 0, 0);
    luaL_buffinit(L, &buf);

    // Initialize SD card
    if (!sd_init_driver()) 
    {
        TerminalPutString("ERROR: Could not initialize SD card\r\n");
        //while (true);
    }

    // Mount drive
    fr = f_mount(&fs, "0:", 1);
    if (fr != FR_OK) 
    {
        TerminalPutString("ERROR: Could not mount filesystem \r\n");
        //while (true);
    }
}

void StartShell()
{
    SetTextColor(TEXTCOL);
    TerminalSetBackCol(BACKCOL);
    currentProgram = SHELL;
    TerminalSetPosition(0, 0);
    TerminalClear(BACKCOL);
    TerminalPutCommand(command);
    commandPos = strlen(command);
}

void RunShell()
{
    ReadInputs();

    ch = (char)KeyboardGiveLetter();

    if (ch == '\r')
    {
        ch = '\n';
    }

    if (KeyboardGetPressed(USB_BACKSPACE))
    {
        if (strlen(command) > 0 && commandPos > 0)
        {
            strcpy(command + commandPos - 1, command + commandPos);
            commandPos--;
            TerminalPutCommand(command);
            TerminalMove(commandPos - strlen(command));
        }
    }
    if (KeyboardGetPressed(USB_DELETE))
    {
        if (command[commandPos] != '\0')
        {
            memmove(command + commandPos, command + commandPos + 1, strlen(command + commandPos) + 1);
            TerminalPutCommand(command);
            TerminalMove(commandPos - strlen(command));
        }
    }
    else if (KeyboardGetPressed(USB_T) && KeyboardGetCtrl())
    {
        TerminalPutString(file);
        TerminalPutString(PROMPT);
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
        status = luaL_loadbuffer(L, s, len, "program");

        filePos = 0;

        if (status != LUA_OK)
        {
            const char *msg = lua_tostring(L, -1);
            TerminalPutString("parse error: ");
            TerminalPutString(msg);
            TerminalPutString("\n");
        }
        else
        {
            status = lua_pcall(L, 0, 0, 0);

            if (status != LUA_OK)
            {
                const char *msg = lua_tostring(L, -1);
                TerminalPutString("execute error: ");
                TerminalPutString(msg);
                TerminalPutString("\n");
                // lua_writestringerror("execute error: %s\n", msg);
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

        strcpy(commandHistory[commandWriteIndex], command);
        commandWriteIndex++;
        commandWriteIndex %= 10;
        commandSelectIndex = commandWriteIndex;

        buf.b = command;
        buf.n = strlen(command);
        luaL_pushresult(&buf);
        const char *s = lua_tolstring(L, -1, &len);
        status = luaL_loadbuffer(L, s, len, "command");

        command[0] = '\0';
        commandPos = 0;

        if (status != LUA_OK)
        {
            const char *msg = lua_tostring(L, -1);
            TerminalPutString("parse error: ");
            TerminalPutString(msg);
            TerminalPutString("\n");
        }
        else
        {
            status = lua_pcall(L, 0, 0, 0);

            if (status != LUA_OK)
            {
                const char *msg = lua_tostring(L, -1);
                TerminalPutString("execute error: ");
                TerminalPutString(msg);
                TerminalPutString("\n");
                // lua_writestringerror("execute error: %s\n", msg);
            }
        }

        lua_pop(L, 1);
        luaL_buffinit(L, &buf);
        TerminalPutString(PROMPT);
    }
    else if ((ch >= 0x20 && ch < 0x7F))
    {
        if (strlen(command) < 77)
        {
            memmove(command + commandPos + 1, command + commandPos, strlen(command + commandPos) + 1);
            command[commandPos] = ch;
            commandPos++;
            TerminalPutCommand(command);
            TerminalMove(commandPos - strlen(command));
        }
    }

    SwitchProgram();
}