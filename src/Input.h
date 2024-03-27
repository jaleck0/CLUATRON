#ifndef INPUT_H
#define INPUT_H

#include "pico/stdlib.h"

typedef enum 
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    START,
    SELECT,
    A_BUTTON,
    B_BUTTON
} ControlInput;

void ReadInputs();

uint8_t KeyboardGetHold(char input);
uint8_t KeyboardGetPressed(char input);
uint8_t KeyboardGetReleased(char input);
uint8_t KeyboardGetConnected();

uint8_t ControllerGetHold(uint8_t controllerNumber, ControlInput input);
uint8_t ControllerGetPressed(uint8_t controllerNumber, ControlInput input);
uint8_t ControllerGetReleased(uint8_t controllerNumber, ControlInput input);
uint8_t ControllerGetConnected(uint8_t controllerNumber);

#endif