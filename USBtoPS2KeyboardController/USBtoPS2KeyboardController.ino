/*
  USB to PS/2 Keyboard Controller
*/

// Includes
//Emulate a PS/2 device
#include "src/PS2/ps2dev.h"
// Require keyboard control library
#include <KeyboardController.h>


// On SAMD boards where the native USB port is also the serial console, use
// Serial1 for the serial console. This applies to all SAMD boards except for
// Arduino Zero and M0 boards.
// Pins are 0 & 1 on my Sparkfun SAMD21 for Serial1, works great

//#if (USB_VID==0x2341 && defined(ARDUINO_SAMD_ZERO)) || (USB_VID==0x2a03 && defined(ARDUINO_SAM_ZERO))
//#define SerialDebug SERIAL_PORT_MONITOR
//#else
//#define SerialDebug Serial1
//#endif

// Initialization
// Initialize PS2 output device
PS2dev PS2keyboard(3, 2); //clock, data
// Initialize USB Controller
USBHost usb;
// Attach keyboard controller to USB
KeyboardController USBkeyboard(usb);


// for just lower case this seems simplistic
// but eventually I'll handle a lot more here (special, modifiers, etc)
unsigned char GetCode(char usb_sym) {
  switch (usb_sym) {
    // letters
    case 4: // a
      return 0x1c;
    case 5: // b
      return 0x32;
    case 6: // c ... rest of alphabet goes up to 29
      return 0x21;
    case 7:
      return 0x23;
    case 8:
      return 0x24;
    case 9:
      return 0x2b;
    case 10:
      return 0x34;
    case 11:
      return 0x33;
    case 12:
      return 0x43;
    case 13:
      return 0x3b;
    case 14:
      return 0x42;
    case 15:
      return 0x4b;
    case 16:
      return 0x3a;
    case 17:
      return 0x31;
    case 18:
      return 0x44;
    case 19:
      return 0x4d;
    case 20:
      return 0x15;
    case 21:
      return 0x2d;
    case 22:
      return 0x1b;
    case 23:
      return 0x2c;
    case 24:
      return 0x3c;
    case 25:
      return 0x2a;
    case 26:
      return 0x1d;
    case 27:
      return 0x22;
    case 28:
      return 0x35;
    case 29:
      return 0x1a;

    // numbers
    case 30: // 1
      return 0x16;
    case 31: // 2 ...
      return 0x1e;
    case 32:
      return 0x26;
    case 33:
      return 0x25;
    case 34:
      return 0x2e;
    case 35:
      return 0x36;
    case 36:
      return 0x3d;
    case 37:
      return 0x3e;
    case 38:
      return 0x46;
    case 39: // 0
      return 0x45;

    // symbols
    case 40: // carriage return
      return 0x5a;
    case 44: // space
      return 0x29;
    case 57: // caps
      return 0x14;
    case 41: //esc
      return 0x08;
    case 43: //tab
      return 0x0d;
    case 53: // `
      return 0x0e;
    case 45: // -
      return 0x4e;
    case 46: // =
      return 0x55;
    case 51: // ;
      return 0x4c;
    case 49: // backslash
      return 0x5c;
    case 56: // /
      return 0x4a;
    case 52: // '
      return 0x52;
    case 54: // ,
      return 0x41;
    case 55: // .
      return 0x49;
    case 47: // [
      return 0x54;
    case 48: // ]
      return 0x5b;
    case 42: // backspace
      return 0x66;
    case 76: // delete
      return 0x64;
    case 81: // down arrow
      return 0x60;
    case 80: // l arrow
      return 0x61;
    case 82: // u arrow
      return 0x63;
    case 79: // r arrow
      return 0x6a;

    // pad
    case 98: // pad 0
      return 0x70;
    case 89: // pad 1
      return 0x69;
    case 90: // pad 2
      return 0x72;
    case 91: // pad 3
      return 0x7a;
    case 92: // pad 4
      return 0x6b;
    case 93: // pad 5
      return 0x75;
    case 94: // pad 6
      return 0x74;
    case 95: // pad 7
      return 0x6c;
    case 96: // pad 8
      return 0x75;
    case 97: // pad 9
      return 0x7d;
    case 99: // pad .
      return 0x71;
    case 86: // pad -
      return 0x84;
    case 88: // pad enter
      return 0x79;

    // function keys
    case 58: // f1
      return 0x07;
    case 59: // f2 ..
      return 0x0f;
    case 60:
      return 0x17;
    case 61:
      return 0x1f;
    case 62:
      return 0x27;
    case 63:
      return 0x2f;
    case 64:
      return 0x37;
    case 65:
      return 0x3f;
    case 66:
      return 0x47;
    case 67:
      return 0x4f;
    case 68:
      return 0x56;
    case 69: // f12
      return 0x5e; // yeah not f, weird pattern eh

    // some more symbols, because following the irix manpage for pckeyboard
    case 70: // prntscreen
      return 0x57;
    case 71: // scroll
      return 0x5f;
    case 72: // pause
      return 0x62;
    case 73: // insert
      return 0x67;
    case 74: // home
      return 0x6e;
    case 75: //pgup
      return 0x6f;
    case 77: // end
      return 0x65;
    case 78: // pgdwn
      return 0x6d;
    case 83: //numlock
      return 0x76;
    case 84: //pad /
      return 0x77;
    case 85: // pad splat
      return 0x7e;
    case 87: // pad +
      return 0x7c;

    // if nothing else
    default:
      return 0x49; // if we don't know just put in a period so something registers
  }
}

void modKeys() {
  // handle modifier keys like shift
  uint8_t getModifiers = USBkeyboard.getModifiers();
  #ifdef SerialDebug
    SerialDebug.print("Modifier mask: ");
    SerialDebug.println(getModifiers);
  #endif

  // pressed keys
  if (getModifiers & 1) { // left ctrl
    PS2keyboard.keyboard_press(0x11);
  }
  if (getModifiers & 2) { // Left shift
    PS2keyboard.keyboard_press(0x12);
  }
  if (getModifiers & 4) { // (Left) Alt
    PS2keyboard.keyboard_press(0x19);
  }
//  if (getModifiers & 8) { // Left CMD - not on SGI boards
//    PS2keyboard.keyboard_press();
//  }
  if (getModifiers & 16) { // Right Ctrl
    PS2keyboard.keyboard_press(0x58);
  }
  if (getModifiers & 32) { // Right shift
    PS2keyboard.keyboard_press(0x59);
  }
  if (getModifiers & 64) { // Alt Gr
    PS2keyboard.keyboard_press(0x39);
  }
//  if (getModifiers & 128) { // Right CMD - not on SGI
//    PS2keyboard.keyboard_press();
//  }

  // released keys
  if (!(getModifiers & 1)) { // left ctrl
    PS2keyboard.keyboard_release(0x11);
  }
  if (!(getModifiers & 2)) { // Left shift
    PS2keyboard.keyboard_release(0x12);
  }
  if (!(getModifiers & 4)) { // (Left) Alt
    PS2keyboard.keyboard_release(0x19);
  }
//  if (!(getModifiers & 8)) { // Left CMD - not on SGI
//    PS2keyboard.keyboard_release();
//  }
  if (!(getModifiers & 16)) { // Right Ctrl
    PS2keyboard.keyboard_release(0x58);
  }
  if (!(getModifiers & 32)) { // Right shift
    PS2keyboard.keyboard_release(0x59);
  }
  if (!(getModifiers & 64)) { // Alt Gr
    PS2keyboard.keyboard_release(0x39);
  }
//  if (!(getModifiers & 128)) { // Right CMD - not on SGI
//    PS2keyboard.keyboard_release();
//  }
}


// This function intercepts key press
// There's no 'hold' handling just yet
void keyPressed() {
  // handle modifiers (eg shift)
  modKeys();

  //get pressed key
  char keyPress = USBkeyboard.getOemKey();
  // what did we get?
  #ifdef SerialDebug
    SerialDebug.print("KeyPress OEM Key: ");
    SerialDebug.println(USBkeyboard.getOemKey());
  #endif

  // grab correct ps2 hex
  unsigned char letter = GetCode(keyPress);
  // what is our translation?
  #ifdef SerialDebug
    SerialDebug.print("PS/2 Code Translation: ");
    SerialDebug.println(letter, HEX);
  #endif
  if ( (digitalRead(3) == HIGH) || (digitalRead(2) == HIGH)) {
    PS2keyboard.keyboard_press(letter); // attempt conversion
  }
}

// This function intercepts key release
void keyReleased() {
  //get released key
  char keyRelease = USBkeyboard.getOemKey();

  //  grab correct ps2 hex
  unsigned char letter = GetCode(keyRelease);
  if ( (digitalRead(3) == HIGH) || (digitalRead(2) == HIGH)) {
    PS2keyboard.keyboard_release(letter); // attempt conversion
  }

  // handle modifiers
  modKeys();
}


// Default Arduino initialization function
void setup()
{
  delay(500);
  #ifdef SerialDebug
    SerialDebug.begin(115200);
    while (!SerialDebug); // Wait for serial port to connect
    SerialDebug.println("Keyboard Controller Program started");
    SerialDebug.println("Setting up USB Init.");
  #endif
  usb.Init();
  #ifdef SerialDebug
    SerialDebug.println("USB Finished, setting up PS2 KB Init.");
  #endif
  PS2keyboard.keyboard_init();
  #ifdef SerialDebug
    SerialDebug.println("PS2 KB Finished beginning output.");
  #endif
  Serial.begin(9600); // start PS/2 output
  pinMode(LED_BUILTIN, OUTPUT);
}


uint32_t lastUSBstate = 0;
void loop()
{
  // Process USB tasks
  usb.Task();
  uint32_t currentUSBstate = usb.getUsbTaskState();
  lastUSBstate = currentUSBstate;
  // Handle PS2 communication and react to keyboard led change
  // This should be done at least once each 10ms
  unsigned char leds;
  while(PS2keyboard.keyboard_handle(&leds)!=0);
  // user for lock indicators 0 - scroll / 1 - num / 2- caps
}
