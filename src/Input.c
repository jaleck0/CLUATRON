#include "Input.h"
#include "bsp/board.h"
#include "tusb.h"

uint8_t controllerInputs[2];
uint8_t controllerConnected[2];
uint8_t keyboardConected = 0;


extern void cdc_app_task(void);

void ReadInputs()
{
    tuh_task();
    cdc_app_task();
}

uint8_t KeyboardGetHold(char input)
{

}

uint8_t KeyboardGetPressed(char input)
{

}

uint8_t KeyboardGetReleased(char input)
{

}

uint8_t KeyboardGetConnected()
{

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