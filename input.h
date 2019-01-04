 /**
  * LCD Keypad shield Input Handler  
  *
  * Reads input from arduino lcd keypad shield
  * debounceDelay can be adjusted if to give more confidant results
  * 
  * example :
  * #include <LiquidCrystal.h>
  * #include "input.h"
  *	LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
  *
  *	void setup()
  * {
  *	 lcd.begin(16, 2);              // start the library
  * }
  *  
  * int ctr = 0 ; // debug value; used to verify if only single event is emitted 	 
  * 
  *	void loop()
  *	{
  *	 lcd.setCursor(9,1);            // move cursor to second line "1" and 9 spaces over
  *	 lcd.print(millis()/1000);      // display seconds elapsed since power-up
  *
  *	 // debug
  *	 bool keyEvent = checkInputState();
  *
  *	 if(keyEvent && buttonState != BTN_NONE){
  *	  lcd.setCursor(0,0);
  *	  lcd.print(buttonState); 
  *	  lcd.setCursor(8,0);
  *	  lcd.print(ctr++);
  *	 }
  *	}
  * 
  * sources - 
  * https://www.arduino.cc/en/Tutorial/Debounce
  * https://www.elecrow.com/wiki/index.php?title=LCD_Keypad_Shield
  * see above urls for original authors
  *
  * Shubham Prasad, shubham@totalcloud.io
  */

#ifndef INPUT_H
#define INPUT_H
 
#define BTN_RIGHT  0
#define BTN_UP     1
#define BTN_DOWN   2
#define BTN_LEFT   3
#define BTN_SELECT 4
#define BTN_NONE   5

/*
  Debounce Input read
  read adc pin 0, 
  buttons when read are centered at these valies: 0, 144, 329, 504, 741
  we add approx 50 to those values and check to see if we are close
*/

int read_LCD_buttons()
{
 int reading = analogRead(0);
 if (reading > 1500) return BTN_NONE; // We make this the 1st option for speed reasons since it will be the most likely result
 if (reading < 50)   return BTN_RIGHT;  
 if (reading < 195)  return BTN_UP; 
 if (reading < 380)  return BTN_DOWN; 
 if (reading < 500)  return BTN_LEFT; 
 if (reading < 700)  return BTN_SELECT;   
 return BTN_NONE;  // when all others fail, return this...
}

// we will use this to see what key is pressed
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

bool checkInputState() {
  
  // read the state of the switch into a local variable:
  int reading = read_LCD_buttons();

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // set this as last state and exit the loop
      lastButtonState = reading;
      return true;
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
  return false;
}

#endif
