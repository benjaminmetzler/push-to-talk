![PTT](./PTT1.png)

## Introduction
Working from home, I  spend a lot of time in virtual meetings.  For one-on-one meetings an open mic is fine but during group meetings I keep my mic muted unless I’m talking. This keeps my background noise from interrupting the speaker and is generally good form in group meetings.

Windows and Linux don't have a built in push-to-talk system, so third-party software needs to be installed.  When I'm on a Windows system, I use an app called [Talk Toggle](https://apps.microsoft.com/store/detail/talk-toggle/9NRJCS6G10KT) which provides system wide Push-To-Talk functionality.   On the Mac the app [MuteKey](https://apps.apple.com/us/app/mutekey/id1509590766?mt=12) has similar functionality.  

_Yes, both Microsoft Teams and Zoom have push-to-talk functionality, but I found the third-party apps are a better choice because the hotkey can be set to system-wide instead of only in the app.  Also Microsoft Teams doesn't let you remap the keyboard key._

While the software works, I have a tendency to forget to press the PTT hotkey when I started talking.  I need something I can hold in my hands as a reminder.  Commercal version were expensive A quick google and I saw that there were options but they expensive or didn't work the way I wanted.  So I decided to build my own push-to-talk button.

## Shopping List
1. Handheld push button - I use the [Philmore 30-825](https://www.ebay.com/sch/i.html?_nkw=Philmore+30-825%2C&_sacat=0), but any momentary switch will work.
   1. You can also use a foot pedal version like the  [Linemaster T-91-S](https://linemaster.com/product/378/Treadlite-Ii/T-91-S/) if you want to keep your hands free (also works good for activating stealth mode in games).
2. Any micro-controller that can emulate an HID will work.  Two options are:
   1. [Teensy USB Development Board (with pins)](https://www.pjrc.com/store/teensy_pins.html)
   2. [Raspberry Pi Pico](https://www.raspberrypi.org/products/raspberry-pi-pico/)
3. [USB Cable - A-Male to Mini-B Cord](https://www.pjrc.com/store/cable_usb_micro_b.html)
4. [Heat shrink tube](https://www.amazon.com/560PCS-Heat-Shrink-Tubing-Eventronic/dp/B072PCQ2LW)
5. Miscellaneous wires.  I used an broken USB-C cable and some wires I had laying around.

## Building with the Teensy

### Assembly
Connect wires between the handheld push button poles and  pins `B1` and `GND`.  This is a simple switch so it doesn't matter which wire is connected to which pin.  I used a breadboard and some jumper wires to avoid soldering to the teensy board.  Heat shrink up any soldered wires too keep things safe and looking clean.

### Code
Start up [Teensydino](https://www.pjrc.com/teensy/td_download.html) and make sure it detects your teensy.  Set the USB Type (under Tools) to `Keyboard + Mouse + Joystick` so that it emulates a keyboard.  Then load the below code.  It will detect when B1 is closed and will press `F13`.  With the above Talk Toggle or MuteKey, pressing the `F13` will unmute the mics.  Release the PTT button and the teensy will release `F13`, reactivating mic mute.

````c
#include <Bounce.h>

Bounce buttonPTT = Bounce(PIN_B1, 10);

void setup() {
  pinMode(PIN_B1, INPUT_PULLUP); // PTT button
  pinMode(PIN_D6, OUTPUT); // LED
}

void loop() {

  buttonPTT.update();

  // When the button is pushed, send ALT+SPACE down
  if (buttonPTT.fallingEdge()) {
    Keyboard.press(KEY_F13);
    digitalWrite(PIN_D6, HIGH); // LED ON
  }

  // When the button is release, send ALT+SPACE release
  if(buttonPTT.risingEdge()) {
    Keyboard.press(KEY_F13);
    digitalWrite(PIN_D6, LOW); // LED OFF
  }
}
````

## Building with the Raspberry Pi Pico
At $4 US, these inexpensive MPC drives the cost of the project to less then $20 (slightly more than the cost of the Teensy alone).  My main reluctance in using the pico is that HID emulation [is currently not supported](https://github.com/micropython/micropython/issues/6811) in the [MicroPython](https://micropython.org/).  Instead I use [CircuitPython](https://circuitpython.org/) to emulate a keyboard with the [Adafruit HID library](https://github.com/adafruit/Adafruit_CircuitPython_HID).  CircuitPython (as of 2021-03-06) shows up as a USB drive when plugged in which makes for easy code editing but some companies have strict no-USB drive policy on company laptops.  As a result the pico option is not as widely usable as the Teensy which just shows up as a keyboard.

### Assembly
Connect wires between the handheld push button poles and pins `GP15` and `3v3`.  This is a simple switch so it doesn't matter which wire is connected to which pin.  I used a breadboard and some jumper wires to avoid soldering to the pico board.  Heat shrink up any soldered wires too keep things safe and looking clean.

### Code
1. Download [CircuitPython](https://circuitpython.org/board/raspberry_pi_pico/).  Select the UF2 and then install it on the pico.  The pico will reboot after copying the UF2 file over.  When it comes up, the pico will be accessable via the file browser.
1. Download [Adafruit_CircuitPython_HID](https://github.com/adafruit/Adafruit_CircuitPython_HID) for HID functionality.  Expand and then copy the lib folder contents to the pico lib folder
1. Create a file called `code.py` on the pi pico.  This will be the main code for your keyboard emulator.
1. Add the below code to `code.py`.  It will send a F13 key press when the button is pushed and release F13 when the button is released.

``` python
import time
import digitalio
import board
import usb_hid
from adafruit_hid.keyboard import Keyboard
from adafruit_hid.keycode import Keycode

# Emulate the HID Keyboard
keyboard = Keyboard(usb_hid.devices)

# Grab the LED for status indication
led = digitalio.DigitalInOut(board.LED)
led.direction = digitalio.Direction.OUTPUT

# Assign GP15 to the button
button_1 = digitalio.DigitalInOut(board.GP15)
button_1.direction = digitalio.Direction.INPUT
button_1.pull = digitalio.Pull.DOWN

is_active = False
while True:
    if button_1.value:
        led.value = True # Light up the LED to indicate the activity
        keyboard.press(Keycode.F13) # Send F13
        is_active = True # Track if the button is in a pushed state
    elif(is_active):  # only send the release if the button is active
        led.value = False # Turn off the LED as activity is stopping
        keyboard.release(Keycode.F13) # Release F13
        is_active = False # Indicate the button is no longer pushed.

    time.sleep(0.1)
```

#### Next Steps
Using the teensy or pico for this project is a bit of overkill.  I am planning on making a macro-keyboard, similar to the [DIY Stream Deck](https://www.partsnotincluded.com/diy-stream-deck-mini-macro-keyboard/).  With it I will be able to switch between [OBS](https://obsproject.com/) streams and potentially enable notification lights.

#### Miscellaneous links
1. [Original Inspiration](https://timmyomahony.com/blog/making-usb-push-buttons/)
1. [Helped me code the Teensy](https://www.pjrc.com/teensy/td_keyboard.html)
1. [Using Pico as an HID](https://hridaybarot.home.blog/2021/01/31/using-raspberry-pi-pico-has-hid-device-to-control-mouse-and-keyboard/) - Provided a good basis for using the pico as an HID.
1. [DIY Macro Keyboard video](https://www.youtube.com/watch?v=aEWptdD32iA) - Helped my craft my code for the pico.
1. [Pico StreamDeck](https://github.com/pjgpetecodes/pico-streamdeck) - Well this just looks cool.
