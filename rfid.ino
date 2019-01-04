#include <LiquidCrystal.h>
#include <SPI.h>        // RC522 Module uses SPI protocol
#include <MFRC522.h>  // Library for Mifare RC522 Devices
#include "rfid.h"
#include "input.h"
#include "menu.h"


#define SS_PIN A1
#define RST_PIN A2

MFRC522 mfrc522(SS_PIN, RST_PIN);

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void no_op () {
  // no operation ; init function
}

void (*__loop)(void);

void printMenu(String s){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("<Menu>");
  lcd.setCursor(0,1);
  lcd.print(s);
}

void printRfidError(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Rfid Init Error !!!");
}

void regAllowCard() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("place card");
  waitForCard();
  registerCard(CurrentCard, Card_Allow);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("card saved");
}

void regDenyCard() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("place card");
  waitForCard();
  registerCard(CurrentCard, Card_Deny);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("card saved");
}

void deleteAllCards() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("deleting ...");
  wipeAll();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("delete ok");
}

void observe(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("waiting for tag ...");
  __loop = __observe;
}

void __observe () {
  if(!checkForCard()) return;
  int cardType = checkCard(CurrentCard);
  switch(cardType){
    case Card_Allow : youShallPass(); return;
    case Card_Deny : youShallNotPass(); return;
    case Card_Unknown : iDontKnowYou(); return;
  }
}

void youShallPass() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("youShallPass");
}

void youShallNotPass(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("youShallNotPass");
}

void iDontKnowYou(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("iDontKnowYou");
}

MenuItem option1 = MenuItem("observe", observe);
MenuItem option2 = MenuItem("regAllowCard", regAllowCard);
MenuItem option3 = MenuItem("regDenyCard", regDenyCard);
MenuItem option4 = MenuItem("deleteAllCards", deleteAllCards);

MenuItem menuItems [] = {option1, option2, option3, option4};

Menu menu = Menu(menuItems, 4, printMenu);

void setup()
{
 lcd.begin(16, 2);              // start the library
 Serial.begin(115200);
 while (!Serial);
  __loop = no_op;
  SPI.begin();           // MFRC522 Hardware uses SPI protocol
  mfrc522.PCD_Init();    // Initialize MFRC522 Hardware
  if(!getReaderInfo()){
    printRfidError();
    while(true);
  }
}

void loop()
{
 // debug
 bool keyEvent = checkInputState();

 if(keyEvent && buttonState != BTN_NONE){
  menu.update(buttonState);
 }

 (*__loop)(); // call method loop

}
