/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021, Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "pico/stdlib.h"
#include "bsp/board.h"
#include "tusb.h"
#include "../Terminal/Terminal.h"
#include "Input.h"

#include <stdlib.h>

//--------------------------------------------------------------------+
// MACRO TYPEDEF CONSTANT ENUM DECLARATION
//--------------------------------------------------------------------+

// If your host terminal support ansi escape code such as TeraTerm
// it can be use to simulate mouse cursor movement within terminal
#define USE_ANSI_ESCAPE   0

#define MAX_REPORT  4

static uint8_t const keycode2ascii[128][2] =  { HID_KEYCODE_TO_ASCII };

// Each HID instance can has multiple reports
static struct
{
  uint8_t report_count;
  tuh_hid_report_info_t report_info[MAX_REPORT];
}hid_info[CFG_TUH_HID];

static void process_kbd_report(hid_keyboard_report_t const *report);
static void process_mouse_report(hid_mouse_report_t const * report);
static void process_generic_report(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len);

void hid_app_task(void)
{
  // nothing to do
}

//--------------------------------------------------------------------+
// TinyUSB Callbacks
//--------------------------------------------------------------------+

// Invoked when device with hid interface is mounted
// Report descriptor is also available for use. tuh_hid_parse_report_descriptor()
// can be used to parse common/simple enough descriptor.
// Note: if report descriptor length > CFG_TUH_ENUMERATION_BUFSIZE, it will be skipped
// therefore report_desc = NULL, desc_len = 0
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len)
{
  //printf("HID device address = %d, instance = %d is mounted\r\n", dev_addr, instance);
  /*
  TerminalPutString("HID device address = ");
  TerminalPutNumber(dev_addr);
  TerminalPutString(", instance = ");
  TerminalPutNumber(instance);
  TerminalPutString(" is mounted\r\n");
  */
  // Interface protocol (hid_interface_protocol_enum_t)
  char* protocol_str[] = { "None", "Keyboard", "Mouse" };
  uint8_t itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);
  KeyboardSetConnected();

  //printf("HID Interface Protocol = %s\r\n", protocol_str[itf_protocol]);
  /*
  TerminalPutString("HID Interface Protocol = ");
  TerminalPutString(protocol_str[itf_protocol]);
  TerminalPutString("\r\n");
  */

  // By default host stack will use activate boot protocol on supported interface.
  // Therefore for this simple example, we only need to parse generic report descriptor (with built-in parser)
  if ( itf_protocol == HID_ITF_PROTOCOL_NONE )
  {
    hid_info[instance].report_count = tuh_hid_parse_report_descriptor(hid_info[instance].report_info, MAX_REPORT, desc_report, desc_len);
    //TO printf("HID has %u reports \r\n", hid_info[instance].report_count);
  }

  // request to receive report
  // tuh_hid_report_received_cb() will be invoked when report is available
  if ( !tuh_hid_receive_report(dev_addr, instance) )
  {
    //printf("Error: cannot request to receive report\r\n");
    //TerminalPutString("Error: cannot request to receive report\r\n");
  }
}

// Invoked when device with hid interface is un-mounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance)
{
  //printf("HID device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);
  /*
  TerminalPutString("HID device address = ");
  TerminalPutNumber(dev_addr);
  TerminalPutString(", instance = ");
  TerminalPutNumber(instance);
  TerminalPutString(" is unmounted\r\n");
  */
  KeyboardSetDisconnected();
}

// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len)
{
  uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);

  switch (itf_protocol)
  {
    case HID_ITF_PROTOCOL_KEYBOARD:
      //TerminalPutString("HID receive boot keyboard report\r\n");
      process_kbd_report( (hid_keyboard_report_t const*) report );
    break;

    case HID_ITF_PROTOCOL_MOUSE:
      //TerminalPutString("HID receive boot mouse report\r\n");
      process_mouse_report( (hid_mouse_report_t const*) report );
    break;

    default:
      // Generic report requires matching ReportID and contents with previous parsed report info
      process_generic_report(dev_addr, instance, report, len);
    break;
  }

  // continue to request to receive report
  if ( !tuh_hid_receive_report(dev_addr, instance) )
  {
    //TerminalPutString("Error: cannot request to receive report\r\n");
  }
}

//--------------------------------------------------------------------+
// Keyboard
//--------------------------------------------------------------------+

// look up new key in previous keys
static inline bool find_key_in_report(hid_keyboard_report_t const *report, uint8_t keycode)
{
  for(uint8_t i=0; i<6; i++)
  {
    if (report->keycode[i] == keycode) 
    {
      return true;
    } 
  }
  return false;
}
///////////////KEYBOARD INPUT IMPORTANT
static void process_kbd_report(hid_keyboard_report_t const *report)
{
  static hid_keyboard_report_t prev_report = { 0, 0, {0} }; // previous report to check key released
  uint8_t changed = 0;

  for(uint8_t i=0; i<6; i++)
  {
    if ( report->keycode[i] )
    {
      if ( find_key_in_report(&prev_report, report->keycode[i]) )
      {
        
      }
      else
      {
        // not existed in previous report means the current key is pressed
        bool const is_shift = report->modifier & (KEYBOARD_MODIFIER_LEFTSHIFT | KEYBOARD_MODIFIER_RIGHTSHIFT);
        bool const is_ctrl = report->modifier & (KEYBOARD_MODIFIER_LEFTCTRL | KEYBOARD_MODIFIER_RIGHTCTRL);
        KeyboardSetModifiers( (uint8_t)is_shift, (uint8_t)is_ctrl);
        
        KeyboardSetKeyPressed(report->keycode[i]);
        if (changed == 0)
        {
          uint8_t ch = keycode2ascii[report->keycode[i]][is_shift ? 1 : 0];
          KeyboardSetInputChar(ch);
          changed = 1;
        }
      }
    }
    for(uint8_t key = 0; key <= 107; key++)
    {
      if (KeyboardGetHold(key) == 1 && !(find_key_in_report(report, key)))
      {
        KeyboardSetKeyReleased(key);
      }
    }
  }
  prev_report = *report;
}

//--------------------------------------------------------------------+
// Mouse
//--------------------------------------------------------------------+
static void process_mouse_report(hid_mouse_report_t const * report)
{
  static hid_mouse_report_t prev_report = { 0 };

  //------------- button state  -------------//
  uint8_t button_changed_mask = report->buttons ^ prev_report.buttons;
  //if ( button_changed_mask & report->buttons)
  //{
  //  //ADD LATER printf(" %c%c%c ",
  //     report->buttons & MOUSE_BUTTON_LEFT   ? 'L' : '-',
  //     report->buttons & MOUSE_BUTTON_MIDDLE ? 'M' : '-',
  //     report->buttons & MOUSE_BUTTON_RIGHT  ? 'R' : '-');
  //}
  MouseSetButtons(report->buttons & MOUSE_BUTTON_LEFT
  , report->buttons & MOUSE_BUTTON_MIDDLE
  , report->buttons & MOUSE_BUTTON_RIGHT
  , prev_report.buttons & MOUSE_BUTTON_LEFT
  , prev_report.buttons & MOUSE_BUTTON_MIDDLE
  , prev_report.buttons & MOUSE_BUTTON_RIGHT);


  //------------- cursor movement -------------//
  //cursor_movement(report->x, report->y, report->wheel);
  MouseSetMovement(report->x, report->y, report->wheel);
  prev_report = *report;
}

//--------------------------------------------------------------------+
// Generic Report
//--------------------------------------------------------------------+
static void process_generic_report(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len)
{
  (void) dev_addr;

  uint8_t const rpt_count = hid_info[instance].report_count;
  tuh_hid_report_info_t* rpt_info_arr = hid_info[instance].report_info;
  tuh_hid_report_info_t* rpt_info = NULL;

  if ( rpt_count == 1 && rpt_info_arr[0].report_id == 0)
  {
    // Simple report without report ID as 1st byte
    rpt_info = &rpt_info_arr[0];
  }else
  {
    // Composite report, 1st byte is report ID, data starts from 2nd byte
    uint8_t const rpt_id = report[0];

    // Find report id in the array
    for(uint8_t i=0; i<rpt_count; i++)
    {
      if (rpt_id == rpt_info_arr[i].report_id )
      {
        rpt_info = &rpt_info_arr[i];
        break;
      }
    }

    report++;
    len--;
  }

  if (!rpt_info)
  {
    //TerminalPutString("Couldn't find the report info for this report !\r\n");
    return;
  }

  // For complete list of Usage Page & Usage checkout src/class/hid/hid.h. For examples:
  // - Keyboard                     : Desktop, Keyboard
  // - Mouse                        : Desktop, Mouse
  // - Gamepad                      : Desktop, Gamepad
  // - Consumer Control (Media Key) : Consumer, Consumer Control
  // - System Control (Power key)   : Desktop, System Control
  // - Generic (vendor)             : 0xFFxx, xx
  if ( rpt_info->usage_page == HID_USAGE_PAGE_DESKTOP )
  {
    switch (rpt_info->usage)
    {
      case HID_USAGE_DESKTOP_KEYBOARD:
        //TU_LOG1("HID receive keyboard report\r\n");
        // Assume keyboard follow boot report layout
        process_kbd_report( (hid_keyboard_report_t const*) report );
      break;

      case HID_USAGE_DESKTOP_MOUSE:
        //TU_LOG1("HID receive mouse report\r\n");
        // Assume mouse follow boot report layout
        process_mouse_report( (hid_mouse_report_t const*) report );
      break;

      default: break;
    }
  }
}
