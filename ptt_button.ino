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
//    Keyboard.press(MODIFIERKEY_ALT);  
//    Keyboard.press(KEY_SPACE);
    digitalWrite(PIN_D6, HIGH); // LED ON
  }
  
  // When the button is release, send ALT+SPACE release
  if(buttonPTT.risingEdge()) {
      Keyboard.release(KEY_F13);
//      Keyboard.release(MODIFIERKEY_ALT);  
//      Keyboard.release(KEY_SPACE);  
      digitalWrite(PIN_D6, LOW); // LED OFF
  }
}
