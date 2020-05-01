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
#if (USB_VID==0x2341 && defined(ARDUINO_SAMD_ZERO)) || (USB_VID==0x2a03 && defined(ARDUINO_SAM_ZERO))
#define SerialDebug SERIAL_PORT_MONITOR
#else
#define SerialDebug Serial1
#endif

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
    case 'a':
      return 0x1c;
    case 'b':
      return 0x32;
    case 'c':
      return 0x21;
    case 'd':
      return 0x23;
    case 'e':
      return 0x24;
    case 'f':
      return 0x2b;
    case 'g':
      return 0x34;
    case 'h':
      return 0x33;
    case 'i':
      return 0x43;
    case 'j':
      return 0x3b;
    case 'k':
      return 0x42;
    case 'l':
      return 0x4b;
    case 'm':
      return 0x3a;
    case 'n':
      return 0x31;
    case 'o':
      return 0x44;
    case 'p':
      return 0x4d;
    case 'q':
      return 0x15;
    case 'r':
      return 0x2d;
    case 's':
      return 0x1b;
    case 't':
      return 0x2c;
    case 'u':
      return 0x3c;
    case 'v':
      return 0x2a;
    case 'w':
      return 0x1d;
    case 'x':
      return 0x22;
    case 'y':
      return 0x35;
    case 'z':
      return 0x1a;
    case 0x0d: // enter
      return 0x5a;
    case 0x20: // space
      return 0x29;
    default:
      return 0x49;
  }
}


// This function intercepts key press
// There's no 'hold' handling just yet
void keyPressed() {
  // handle modifier keys like shift
  uint8_t getModifiers = USBkeyboard.getModifiers();
  if (getModifiers && 2) { // Left shift
    PS2keyboard.keyboard_press(0x12);
  }
  if (getModifiers && 32) { // Right shift
    PS2keyboard.keyboard_press(0x59);
  }
  //  uint8_t getModifiers = USBkeyboard.getModifiers();
  if (! getModifiers && 2) { // Left shift
    PS2keyboard.keyboard_release(0x12);
  }
  if (! getModifiers && 32) { // Right shift
    PS2keyboard.keyboard_release(0x59);
  }

  //get lower of pressed key
  char keyPress = USBkeyboard.getKey();
  if (isupper(keyPress))
    keyPress = tolower(keyPress);

  // grab correct ps2 hex
  unsigned char letter = GetCode(keyPress);
  if ( (digitalRead(3) == HIGH) || (digitalRead(2) == HIGH)) {
    //PS2keyboard.keyboard_press(letter); // attempt conversion
    PS2keyboard.keyboard_mkbrk(letter); // attempt conversion
  }
}

// This function intercepts key release
void keyReleased() {
  // handle modifier keys like shift
  uint8_t getModifiers = USBkeyboard.getModifiers();
  if (getModifiers && 2) { // Left shift
    PS2keyboard.keyboard_press(0x12);
  }
  if (getModifiers && 32) { // Right shift
    PS2keyboard.keyboard_press(0x59);
  }
  //  uint8_t getModifiers = USBkeyboard.getModifiers();
  if (! getModifiers && 2) { // Left shift
    PS2keyboard.keyboard_release(0x12);
  }
  if (! getModifiers && 32) { // Right shift
    PS2keyboard.keyboard_release(0x59);
  }

  //get lower of released key
  char keyPress = USBkeyboard.getKey();
  if (isupper(keyPress))
    keyPress = tolower(keyPress);

  // grab correct ps2 hex
  //  unsigned char letter = GetCode(keyPress);
  //  if ( (digitalRead(3) == HIGH) || (digitalRead(2) == HIGH)) {
  //    PS2keyboard.keyboard_release(letter); // attempt conversion
  //  }
}


// Default Arduino initialization function
void setup()
{
  SerialDebug.begin(115200);
  while (!SerialDebug); // Wait for serial port to connect
  SerialDebug.println("Keyboard Controller Program started");
  SerialDebug.println("Setting up USB Init.");
  usb.Init();
  SerialDebug.println("USB Finished, setting up PS2 KB Init.");
  PS2keyboard.keyboard_init();
  SerialDebug.println("PS2 KB Finished beginning output.");
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
  // user for lock indicators 0 - scroll / 1 - num / 2- caps
  if (PS2keyboard.keyboard_handle(&leds)) {
    // kbdLockingKeys.kbdLeds.bmNumLock = ~kbdLockingKeys.kbdLeds.bmNumLock;
    // kbdLockingKeys.kbdLeds.bmCapsLock = ~kbdLockingKeys.kbdLeds.bmCapsLock;
    // kbdLockingKeys.kbdLeds.bmScrollLock = ~kbdLockingKeys.kbdLeds.bmScrollLock;
    digitalWrite(LED_BUILTIN, leds);
  }
}
