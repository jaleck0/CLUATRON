#include "CluatronEditingTool.h"
#include "../USBInput/Input.h"
#include "SharedOS.h"

void InitCET()
{
}

void StartCET()
{
    SetTextColor(TEXTCOL);
    TerminalSetBackCol(BACKCOL);
    currentProgram = CET;
    TerminalSetPosition(0, 0);
    TerminalClear();
    TerminalPutString(file);
    filePos = strlen(file);
    
}

void RunCET()
{
    ReadInputs();

    ch = (char)KeyboardGiveLetter();

    if (ch == '\r')
    {
        ch = '\n';
    }

    if (KeyboardGetPressed(USB_BACKSPACE))
    {
        if (strlen(file) > 0 && filePos > 0)
        {
            strcpy(file + filePos - 1, file + filePos);
            filePos--;
            TerminalClear();
            TerminalPutString(file);
            TerminalMove(filePos - strlen(file));
        }
    }
    if (KeyboardGetPressed(USB_DELETE))
    {
        if (file[filePos] != '\0')
        {
            memmove(file + filePos, file + filePos + 1, strlen(file + filePos) + 1);
            TerminalClear();
            TerminalPutString(file);
            TerminalMove(filePos - strlen(file));
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
    else if ((ch >= 0x20 && ch < 0x7F))
    {
        if (strlen(file) < 32767)
        {
            memmove(file + filePos + 1, file + filePos, strlen(file + filePos) + 1);
            file[filePos] = ch;
            filePos++;
            TerminalClear();
            TerminalPutString(file);
            TerminalMove(filePos - strlen(file));
        }
    }

    SwitchProgram();
}