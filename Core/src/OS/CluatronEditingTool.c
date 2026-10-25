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

            uint32_t probePos = filePos;

            while(file[probePos] != '\n' && probePos > 0)
            {
                probePos--;
            }

            currentHPos = filePos - probePos;

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

        uint32_t probePos = filePos;

        while(file[probePos] != '\n' && probePos > 0)
        {
            probePos--;
        }

        currentHPos = filePos - probePos;

        TerminalMove(-1);
    }
    else if (KeyboardGetPressed(USB_RIGHT) && filePos < strlen(file))
    {
        filePos++;

        uint32_t probePos = filePos;

        while(file[probePos] != '\n' && probePos > 0)
        {
            probePos--;
        }

        currentHPos = filePos - probePos;

        TerminalMove(1);
    }
    else if (KeyboardGetPressed(USB_UP))
    {
        uint32_t probePos = filePos;

        while(file[probePos] != '\n' && probePos > 0)
        {
            probePos--;
        }

        currentHPos = filePos - probePos;
        
        if (probePos == 0)
        {
            filePos = 0;
        }
        else
        {
            uint32_t prePropbePos = probePos-1;

            while(file[prePropbePos] != '\n' && prePropbePos > 0)
            {
                prePropbePos--;
            }

            uint32_t newHpos = currentHPos;

            if (newHpos > (probePos - prePropbePos))
            {
                filePos = prePropbePos + (probePos - prePropbePos);
            }
            else
            {
                filePos = prePropbePos + newHpos;
            }
        }
    }
    else if (KeyboardGetPressed(USB_DOWN))
    {
        uint32_t probePos = filePos;

        while(file[probePos] != '\n' && probePos > 0)
        {
            probePos--;
        }

        currentHPos = filePos - probePos;

        uint32_t postProbePos = filePos;

        while(file[postProbePos] != '\n' && postProbePos < strlen(file)-1)
        {
            postProbePos++;
        }

        if (postProbePos == strlen(file) - 1)
        {
            filePos = strlen(file) - 1;
        } 
        else
        {
            uint32_t nextProbePos = postProbePos + 1;

            while(file[nextProbePos] != '\n' && nextProbePos < strlen(file)-1)
            {
                nextProbePos++;
            }
            
            if (currentHPos > (nextProbePos - postProbePos))
            {
                filePos = postProbePos + (nextProbePos - postProbePos);
            }
            else
            {
                filePos = postProbePos + currentHPos;
            }
        }
        
    }
    else if ((ch >= 0x20 && ch < 0x7F) || ch == '\n')
    {
        if (strlen(file) < 32767)
        {
            memmove(file + filePos + 1, file + filePos, strlen(file + filePos) + 1);
            file[filePos] = ch;
            filePos++;

            uint32_t probePos = filePos;

            while(file[probePos] != '\n' && probePos > 0)
            {
                probePos--;
            }

            currentHPos = filePos - probePos;

            TerminalClear();
            TerminalPutString(file);
            TerminalMove(filePos - strlen(file));
        }
    }

    SwitchProgram();
}