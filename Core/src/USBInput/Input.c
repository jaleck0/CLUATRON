#include "Input.h"
#include "bsp/board.h"
#include "tusb.h"
#include <string.h>


#define KEYCOUNT 108
#define MBCOUNT 3

uint8_t controllerInputs[2];
uint8_t controllerConnected[2];
uint8_t keyboardConected = 0;
uint8_t keyboardShift = 0;
uint8_t keyboardCtrl = 0;

uint32_t keyboardT = 0;

uint8_t KeycodesPressed[KEYCOUNT];
uint8_t KeycodesHold[KEYCOUNT];
uint8_t KeycodesReleased[KEYCOUNT];

uint8_t MouseBPressed[MBCOUNT];
uint8_t MouseBHold[MBCOUNT];
uint8_t MouseBReleased[MBCOUNT];

int8_t MouseXSpeed = 0;
int8_t MouseYSpeed = 0;
int8_t MouseSSpeed = 0;

uint32_t mouseT = 0;

//extern void cdc_app_task(void);

char putKeyoardChar = 0;

void InitInputs()
{
    //tusb_init();
    tuh_init(BOARD_TUH_RHPORT);
    //
}

void ReadInputs()
{
    for(uint8_t mb = 0; mb < MBCOUNT; mb++)
    {
        if (MouseBPressed[mb] == 1)
        {
            MouseBPressed[mb] = 0;
        }

        if (MouseBReleased[mb] == 1)
        {
            MouseBHold[mb] = 0;
            MouseBReleased[mb] = 0;
        }
    }

    for(uint8_t key = 0; key < KEYCOUNT; key++)
    {
        if (KeycodesPressed[key] == 1)
        {
            KeycodesPressed[key] = 0;
        }

        if (KeycodesReleased[key] == 1)
        {
            KeycodesReleased[key] = 0;
            KeycodesHold[key] = 0;
        }
    }

    if (to_ms_since_boot(get_absolute_time()) > mouseT + 16)
    {
        MouseSetMovement(0,0,0);
    }

    tuh_task(); 
    //cdc_app_task(); 
}

uint8_t KeyboardGetHold(KeyboardInput input)
{
    return KeycodesHold[input];
}

uint8_t KeyboardGetPressed(KeyboardInput input)
{
    return KeycodesPressed[input];
}

uint8_t KeyboardGetReleased(KeyboardInput input)
{
    return KeycodesReleased[input];
}

uint8_t KeyboardGetConnected()
{
    return keyboardConected;
}

char KeyboardGiveLetter()
{
    char returnChar = putKeyoardChar;
    putKeyoardChar = 0;
    return returnChar;
}

int8_t MouseGetXSpeed()
{
    return MouseXSpeed;
}

int8_t MouseGetYSpeed()
{
    return MouseYSpeed;
}

int8_t MouseGetSSpeed()
{
    return MouseSSpeed;
}

uint8_t MouseGetHold(MouseInput input)
{
    return MouseBHold[input];
}

uint8_t MouseGetPressed(MouseInput input)
{
    return MouseBPressed[input]; 
}

uint8_t MouseGetReleased(MouseInput input)
{
    return MouseBReleased[input];
}

void KeyboardSetConnected()
{
    keyboardConected = 1;
}

void KeyboardSetDisconnected()
{
    keyboardConected = 0;
}

void KeyboardSetKeyPressed(KeyboardInput keyCode)
{
    KeycodesPressed[keyCode] = 1;
    KeycodesHold[keyCode] = 1;
}

void KeyboardSetKeyReleased(KeyboardInput keyCode)
{
    KeycodesReleased[keyCode] = 1;
    KeycodesHold[keyCode] = 0;
}

void KeyboardSetModifiers(uint8_t setShift, uint8_t setCtrl)
{
    keyboardShift = setShift;
    keyboardCtrl = setCtrl;
}

uint8_t KeyboardGetShift()
{
    return keyboardShift;
}

uint8_t KeyboardGetCtrl()
{
    return keyboardCtrl;
}

void KeyboardSetInputChar(char keyCode)
{
    if ( keyCode == '\r' ) 
    {
        keyCode = '\n';
    }
    putKeyoardChar = keyCode;
}

void MouseSetMovement(int8_t x, int8_t y, int8_t wheel)
{
    MouseXSpeed = x;
    MouseYSpeed = y;
    MouseSSpeed = wheel;
    mouseT = to_ms_since_boot(get_absolute_time());
}

void MouseSetButtons(uint8_t lb, uint8_t mb, uint8_t rb, uint8_t plb, uint8_t pmb, uint8_t prb)
{
    if (lb && !plb)
    {
        MouseBPressed[USB_MOUSEB_LEFT] = 1;
        MouseBHold[USB_MOUSEB_LEFT] = 1;
        
    } 
    if (!lb && plb)
    {
        MouseBReleased[USB_MOUSEB_LEFT] = 1;
        MouseBHold[USB_MOUSEB_LEFT] = 0;
    }

    if (mb && !pmb)
    {
        MouseBPressed[USB_MOUSEB_MIDDLE] = 1;
        MouseBHold[USB_MOUSEB_MIDDLE] = 1;
        
    } 
    if (!mb && pmb)
    {
        MouseBReleased[USB_MOUSEB_MIDDLE] = 1;
        MouseBHold[USB_MOUSEB_MIDDLE] = 0;
    }

    if (rb && !prb)
    {
        MouseBPressed[USB_MOUSEB_RIGHT] = 1;
        MouseBHold[USB_MOUSEB_RIGHT] = 1;
        
    } 
    if (!rb && prb)
    {
        MouseBReleased[USB_MOUSEB_RIGHT] = 1;
        MouseBHold[USB_MOUSEB_RIGHT] = 0;
    }

}