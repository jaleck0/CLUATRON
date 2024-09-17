#ifndef INPUT_H
#define INPUT_H

#include "pico/stdlib.h"

typedef enum
{
    USB_NULL,
    USB_ERR0,
    USB_ERR1,
    USB_ERR2,
    USB_A,
    USB_B,
    USB_C,
    USB_D,
    USB_E,
    USB_F,
    USB_G,
    USB_H,
    USB_I,
    USB_J,
    USB_K,
    USB_L,
    USB_M,
    USB_N,
    USB_O,
    USB_P,
    USB_Q,
    USB_R,
    USB_S,
    USB_T,
    USB_U,
    USB_V,
    USB_W,
    USB_X,
    USB_Y,
    USB_Z,
    USB_1,
    USB_2,
    USB_3,
    USB_4,
    USB_5,
    USB_6,
    USB_7,
    USB_8,
    USB_9,
    USB_0,
    USB_ENTER,
    USB_ESC,
    USB_BACKSPACE,
    USB_TAB,
    USB_SPACE,
    USB_MIN,
    USB_EQUALS,
    USB_BRACKETOPEN,
    USB_BRACKETCLOSE,
    USB_BACKSLASH,
    USB_DOTS,
    USB_COLON,
    USB_QUOTES,
    USB_BACKQUATE,
    USB_KOMMA,
    USB_FULLSTOP,
    USB_SLASH,
    USB_CAPSLOCK,
    USB_F1,
    USB_F2,
    USB_F3,
    USB_F4,
    USB_F5,
    USB_F6,
    USB_F7,
    USB_F8,
    USB_F9,
    USB_F10,
    USB_F11,
    USB_F12,
    USB_PRINTSCREEN,
    USB_SCROLL_LOCK,
    USB_PAUSE,
    USB_INSERT,
    USB_HOME,
    USB_PAGEUP,
    USB_DELETE,
    USB_END,
    USB_PAGEDOWN,
    USB_RIGHT,
    USB_LEFT,
    USB_DOWN,
    USB_UP,
    USB_NUMLOCK,
    USB_KP_SLASH,
    USB_KP_POWER,
    USB_KP_MINUS,
    USB_KP_PLUS,
    USB_KP_ENTER,
    USB_KP_1,
    USB_KP_2,
    USB_KP_3,
    USB_KP_4,
    USB_KP_5,
    USB_KP_6,
    USB_KP_7,
    USB_KP_8,
    USB_KP_9,
    USB_KP_0,
    USB_KP_DEL,
    USB_KP_EXTRA,
    USB_KP_APP
} KeyboardInput;

typedef enum 
{
    USB_MOUSEB_LEFT,
    USB_MOUSEB_MIDDLE,
    USB_MOUSEB_RIGHT
} MouseInput;



void InitInputs();
void ReadInputs();

uint8_t KeyboardGetHold(KeyboardInput input);
uint8_t KeyboardGetPressed(KeyboardInput input);
uint8_t KeyboardGetReleased(KeyboardInput input);
uint8_t KeyboardGetConnected();
uint8_t KeyboardGetShift();
uint8_t KeyboardGetCtrl();
char KeyboardGiveLetter();
int8_t MouseGetXSpeed();
int8_t MouseGetYSpeed();
int8_t MouseGetSSpeed();
uint8_t MouseGetHold(MouseInput input);
uint8_t MouseGetPressed(MouseInput input);
uint8_t MouseGetReleased(MouseInput input);

void KeyboardSetConnected();
void KeyboardSetDisconnected();
void KeyboardSetKeyPressed(KeyboardInput keyCode);
void KeyboardSetKeyReleased(KeyboardInput keyCode);
void KeyboardSetModifiers(uint8_t setShift, uint8_t setCtrl);
void KeyboardSetInputChar(char keyCode);
void MouseSetMovement(int8_t x, int8_t y, int8_t wheel);
void MouseSetKeyPressed(MouseInput keyCode);
void MouseSetKeyReleased(MouseInput keyCode);

#endif