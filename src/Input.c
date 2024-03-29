#include "Input.h"
#include "bsp/board.h"
#include "tusb.h"

uint8_t controllerInputs[2];
uint8_t controllerConnected[2];
uint8_t keyboardConected = 0;
uint8_t keyboardShift = 0;
uint8_t keyboardCtrl = 0;

uint8_t KeycodesPressed[256];
uint8_t KeycodesHold[256];
uint8_t KeycodesReleased[256];

extern void cdc_app_task(void);

void ReadInputs()
{
    for(uint8_t key = 0; key <= 127; key++)
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

    tuh_task();
    cdc_app_task();

    
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
    return 0;
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

uint8_t ControllerGetHold(uint8_t controllerNumber, ControlInput input)
{

}

uint8_t ControllerGetPressed(uint8_t controllerNumber, ControlInput input)
{

}

uint8_t ControllerGetReleased(uint8_t controllerNumber, ControlInput input)
{

}

uint8_t ControllerGetConnected(uint8_t controllerNumber)
{

}