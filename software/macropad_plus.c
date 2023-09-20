// ===================================================================================
// Project:   MacroPad Plus for CH551, CH552 and CH554
// Version:   v1.0
// Year:      2023
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Firmware example implementation for the MacroPad Plus.
//
// References:
// -----------
// - Blinkinlabs: https://github.com/Blinkinlabs/ch554_sdcc
// - Deqing Sun: https://github.com/DeqingSun/ch55xduino
// - Ralph Doncaster: https://github.com/nerdralph/ch554_sdcc
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Chip:  CH551, CH552 or CH554
// - Clock: 16 MHz internal
// - Adjust the firmware parameters in src/config.h if necessary.
// - Customize the macro functions in the corresponding section below.
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Run 'make flash' immediatly afterwards.
// - To compile the firmware using the Arduino IDE, follow the instructions in the 
//   .ino file.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a HID device with
//   keyboard, mouse and joystick interface.
// - Press a macro key or turn the knob and see what happens.
// - To enter bootloader hold down rotary encoder switch while connecting the 
//   MacroPad to USB. All NeoPixels will light up white as long as the device is in 
//   bootloader mode (about 10 seconds).


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================

// Libraries
#include "src/config.h"                     // user configurations
#include "src/system.h"                     // system functions
#include "src/delay.h"                      // delay functions
#include "src/neo.h"                        // NeoPixel functions
#include "src/usb_composite.h"              // USB HID composite functions

// Prototypes for used interrupts
void USB_interrupt(void);
void USB_ISR(void) __interrupt(INT_NO_USB) {
  USB_interrupt();
}

#pragma disable_warning 110                 // Keep calm, EVELYN!

// ===================================================================================
// Macro Functions which associate Actions with Events (Customize your MacroPad here!)
// ===================================================================================
/*
// The list of available USB HID functions can be found in src/usb_composite.h
// The keys are enumerated the following way:
// +---+---+---+    -----
// | 3 | 2 | 1 |  /       \
// +---+---+---+  |encoder|
// | 4 | 5 | 6 |  \       /
// +---+---+---+    -----
*/

// Define action(s) if key1 was pressed
inline void KEY1_PRESSED() {
  CON_press(CON_MEDIA_STOP);
}

// Define action(s) if key1 was released
inline void KEY1_RELEASED() {
  CON_release();
}

// Define action(s) when key1 is held
inline void KEY1_HOLD() {
}

// Define action(s) if key2 was pressed
inline void KEY2_PRESSED() {
  CON_press(CON_MEDIA_PLAY);
}

// Define action(s) if key2 was released
inline void KEY2_RELEASED() {
  CON_release();
}

// Define action(s) when key2 is held
inline void KEY2_HOLD() {
}

// Define action(s) if key3 was pressed
inline void KEY3_PRESSED() {
  CON_press(CON_MEDIA_PAUSE);
}

// Define action(s) if key3 was released
inline void KEY3_RELEASED() {
  CON_release();
}

// Define action(s) when key3 is held
inline void KEY3_HOLD() {
}

// Define action(s) if key4 was pressed
inline void KEY4_PRESSED() {
  KBD_press('d');
}

// Define action(s) if key4 was released
inline void KEY4_RELEASED() {
  KBD_release('d');
}

// Define action(s) when key4 is held
inline void KEY4_HOLD() {
}

// Define action(s) if key5 was pressed
inline void KEY5_PRESSED() {
  KBD_press('e');
}

// Define action(s) if key5 was released
inline void KEY5_RELEASED() {
  KBD_release('e');
}

// Define action(s) when key5 is held
inline void KEY5_HOLD() {
}

// Define action(s) if key6 was pressed
inline void KEY6_PRESSED() {
  KBD_press('f');
}

// Define action(s) if key6 was released
inline void KEY6_RELEASED() {
  KBD_release('f');
}

// Define action(s) when key6 is held
inline void KEY6_HOLD() {
}

// Rotary encoder example -> volume control knob
// ---------------------------------------------

// Define action(s) if encoder was rotated clockwise
inline void ENC_CW_ACTION() {
  CON_press(CON_VOL_UP);                              // press VOLUME UP key
}

// Define action(s) after encoder was rotated clockwise
inline void ENC_CW_RELEASED() {
  CON_release();                                      // release VOLUME UP KEY
}

// Define action(s) if encoder was rotated counter-clockwise
inline void ENC_CCW_ACTION() {
  CON_press(CON_VOL_DOWN);                            // press VOLUME DOWN key
}

// Define action(s) after encoder was rotated counter-clockwise
inline void ENC_CCW_RELEASED() {
  CON_release();                                      // release VOLUME DOWN KEY
}

// Define action(s) if encoder switch was pressed
inline void ENC_SW_PRESSED() {
  CON_press(CON_VOL_MUTE);                            // press VOLUME MUTE key
}

// Define action(s) if encoder switch was released
inline void ENC_SW_RELEASED() {
  CON_release();                                      // release VOLUME MUTE key
}

// ===================================================================================
// NeoPixel Configuration
// ===================================================================================

// Global NeoPixel brightness
#define NEO_BRIGHT_KEYS   2         // NeoPixel brightness for keys (0..2)
#define NEO_BRIGHT_ENC    0         // NeoPixel brightness for encoder ring (0..2)

// Key colors (hue value: 0..191)
#define NEO_KEY1          0         // red
#define NEO_KEY2          32        // yellow
#define NEO_KEY3          64        // green
#define NEO_KEY4          96        // cyan
#define NEO_KEY5          128       // blue
#define NEO_KEY6          160       // magenta

// ===================================================================================
// NeoPixel Functions
// ===================================================================================

uint8_t neoencoder = 0;                           // state of NeoPixel ring rotation

// Update NeoPixel ring colors
void NEO_encoder_update(void) {
  uint8_t i, j;
  j = neoencoder;
  for(i=6; i<18; i++) {
    NEO_writeHue(i, j, NEO_BRIGHT_ENC);
    j += 16;
    if(j >= 192) j -= 192;
  }
  NEO_update();
}

// Rotate NeoPixel ring clockwise
void NEO_encoder_cw(void) {
  neoencoder += 8;
  if(neoencoder >= 192) neoencoder -= 192;
  NEO_encoder_update();
}

// Rotate NeoPixel ring counter-clockwise
void NEO_encoder_ccw(void) {
  neoencoder -= 8;
  if(neoencoder >= 192) neoencoder -= 64;
  NEO_encoder_update();
}

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Variables
  __bit key1last = 0;                             // last state of key 1
  __bit key2last = 0;                             // last state of key 2
  __bit key3last = 0;                             // last state of key 3
  __bit key4last = 0;                             // last state of key 4
  __bit key5last = 0;                             // last state of key 5
  __bit key6last = 0;                             // last state of key 6
  __bit isSwitchPressed = 0;                      // state of rotary encoder switch
  __idata uint8_t i;                              // temp variable

  // Setup
  NEO_init();                                     // init NeoPixels
  CLK_config();                                   // configure system clock
  DLY_ms(10);                                     // wait for clock to settle
  NEO_clearAll();                                 // clear NeoPixels

  // Enter bootloader if rotary encoder switch is pressed
  if(!PIN_read(PIN_ENC_SW)) {                     // encoder switch pressed?
    for(i=3*NEO_COUNT; i; i--) NEO_sendByte(127); // light up all pixels
    BOOT_now();                                   // enter bootloader
  }

  // Init USB HID device
  HID_init();                                     // init USB HID device
  DLY_ms(500);                                    // wait for Windows
  WDT_start();                                    // start watchdog timer
  NEO_encoder_update();                           // set NeoPixel ring for encoder

  // Loop
  while(1) {

    // Handle key 1
    // ------------
    if(!PIN_read(PIN_KEY1) != key1last) {         // key state changed?
      key1last = !key1last;                       // update last state flag
      if(key1last) {                              // key was pressed?
        NEO_writeHue(0, NEO_KEY1, NEO_BRIGHT_KEYS);    // light up corresponding NeoPixel
        NEO_update();                             // update pixels
        KEY1_PRESSED();                           // take proper action
      }
      else {                                      // key was released?
        NEO_clearPixel(0);                        // light up corresponding NeoPixel
        NEO_update();                             // update pixels
        KEY1_RELEASED();                          // take proper action
      }
    }
    else if(key1last) {                           // key still being pressed?
      KEY1_HOLD();                                // take proper action
    }

    // Handle key 2
    // ------------
    if(!PIN_read(PIN_KEY2) != key2last) {         // key state changed?
      key2last = !key2last;                       // update last state flag
      if(key2last) {                              // key was pressed?
        NEO_writeHue(1, NEO_KEY2, NEO_BRIGHT_KEYS);    // light up corresponding NeoPixel
        NEO_update();                             // update pixels
        KEY2_PRESSED();                           // take proper action
      }
      else {                                      // key was released?
        NEO_clearPixel(1);                        // light up corresponding NeoPixel
        NEO_update();                             // update pixels
        KEY2_RELEASED();                          // take proper action
      }
    }
    else if(key2last) {                           // key still being pressed?
      KEY2_HOLD();                                // take proper action
    }

    // Handle key 3
    // ------------
    if(!PIN_read(PIN_KEY3) != key3last) {         // key state changed?
      key3last = !key3last;                       // update last state flag
      if(key3last) {                              // key was pressed?
        NEO_writeHue(2, NEO_KEY3, NEO_BRIGHT_KEYS);    // light up corresponding NeoPixel
        NEO_update();                             // update pixels
        KEY3_PRESSED();                           // take proper action
      }
      else {                                      // key was released?
        NEO_clearPixel(2);                        // light up corresponding NeoPixel
        NEO_update();                             // update pixels
        KEY3_RELEASED();                          // take proper action
      }
    }
    else if(key3last) {                           // key still being pressed?
      KEY3_HOLD();                                // take proper action
    }

    // Handle key 4
    // ------------
    if(!PIN_read(PIN_KEY4) != key4last) {         // key state changed?
      key4last = !key4last;                       // update last state flag
      if(key4last) {                              // key was pressed?
        NEO_writeHue(3, NEO_KEY4, NEO_BRIGHT_KEYS);    // light up corresponding NeoPixel
        NEO_update();                             // update pixels
        KEY4_PRESSED();                           // take proper action
      }
      else {                                      // key was released?
        NEO_clearPixel(3);                        // light up corresponding NeoPixel
        NEO_update();                             // update pixels
        KEY4_RELEASED();                          // take proper action
      }
    }
    else if(key4last) {                           // key still being pressed?
      KEY4_HOLD();                                // take proper action
    }

    // Handle key 5
    // ------------
    if(!PIN_read(PIN_KEY5) != key5last) {         // key state changed?
      key5last = !key5last;                       // update last state flag
      if(key5last) {                              // key was pressed?
        NEO_writeHue(4, NEO_KEY5, NEO_BRIGHT_KEYS);    // light up corresponding NeoPixel
        NEO_update();                             // update pixels
        KEY5_PRESSED();                           // take proper action
      }
      else {                                      // key was released?
        NEO_clearPixel(4);                        // light up corresponding NeoPixel
        NEO_update();                             // update pixels
        KEY5_RELEASED();                          // take proper action
      }
    }
    else if(key5last) {                           // key still being pressed?
      KEY5_HOLD();                                // take proper action
    }

    // Handle key 6
    // ------------
    if(!PIN_read(PIN_KEY6) != key6last) {         // key state changed?
      key6last = !key6last;                       // update last state flag
      if(key6last) {                              // key was pressed?
        NEO_writeHue(5, NEO_KEY6, NEO_BRIGHT_KEYS);    // light up corresponding NeoPixel
        NEO_update();                             // update pixels
        KEY6_PRESSED();                           // take proper action
      }
      else {                                      // key was released?
        NEO_clearPixel(5);                        // light up corresponding NeoPixel
        NEO_update();                             // update pixels
        KEY6_RELEASED();                          // take proper action
      }
    }
    else if(key6last) {                           // key still being pressed?
      KEY6_HOLD();                                // take proper action
    }

    // Handle rotary encoder
    // ---------------------
    if(!PIN_read(PIN_ENC_A)) {                    // encoder turned ?
      if(PIN_read(PIN_ENC_B)) {                   // clockwise ?
        ENC_CW_ACTION();                          // take proper action
        NEO_encoder_cw();                         // rotate NeoPixels
        DLY_ms(5);                                // debounce
        ENC_CW_RELEASED();                        // take proper action
      }
      else {                                      // counter-clockwise ?
        ENC_CCW_ACTION();                         // take proper action
        NEO_encoder_ccw();                        // rotate NeoPixels
        DLY_ms(5);                                // debounce
        ENC_CCW_RELEASED();                       // take proper action
      }
      while(!PIN_read(PIN_ENC_A));                // wait until next detent
    } 
    else {
      if(!isSwitchPressed && !PIN_read(PIN_ENC_SW)) {     // switch previously pressed?
        ENC_SW_PRESSED();                         // take proper action
        isSwitchPressed = 1;
      }
      else if(isSwitchPressed && PIN_read(PIN_ENC_SW)) {  // switch previously released?
        ENC_SW_RELEASED();                        // take proper action
        isSwitchPressed = 0;                      // update switch state
      }
    }

    DLY_ms(1);                                    // debounce
    WDT_reset();                                  // reset watchdog
  }
}
