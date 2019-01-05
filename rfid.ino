#include <LiquidCrystal.h>
#include <SPI.h>        // RC522 Module uses SPI protocol
#include <MFRC522.h>  // Library for Mifare RC522 Devices
#include "rfid.h"
#include "input.h"
#include "menu.h"

#define SS_PIN A1
#define RST_PIN A2

#define MotorP A4
#define MotorN A5

#define HaltPin1 A3
#define HaltPin2 3

MFRC522 mfrc522(SS_PIN, RST_PIN);

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

bool halt = false;

void no_op () {
  // no operation ; init function
}

void (*__loop)(void);

int closeTimer = 0 ;

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

void regSecondaryAllowCard() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("place card");
  waitForCard();
  registerCard(CurrentCard, Card_Allow_2);
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

void erase_memory() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("deleting ...");
  wipeSavedMovement();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("delete ok");
}

int last_count = -1;
byte last_id = -1;
void __replay_memory(int count, byte id) {
  int y = last_count >= 0 ? 1 : 0 ;
  lcd.setCursor(0,y);
  lcd.print("#"+String(count + 1)+" truck_id: " +String(id));
  delay(1500);
  if(last_count >= 0){
    lcd.setCursor(0,0);
    lcd.print("#"+String(last_count + 1)+" truck_id: " +String(last_id));  
  }
  last_count = count;
  last_id = id;
}

void replay_memory() {
  last_count = -1;
  lcd.clear();
  replaySavedMovement(__replay_memory);
}

void deleteEverything() {
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
    case Card_Allow : youShallPass(1); return;
    case Card_Allow_2 : youShallPass(2); return;
    case Card_Deny : youShallNotPass(); return;
    case Card_Unknown : iDontKnowYou(); return;
  }
}

void youShallPass(int i) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("youShallPass");
  storeMovement(i);
  openGate();
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

void openGate() {
  lcd.setCursor(0,1);
  lcd.print("open gate");
  digitalWrite(MotorP, LOW);
  digitalWrite(MotorN, HIGH);
  delay(1500);
  digitalWrite(MotorP, LOW);
  digitalWrite(MotorN, LOW);
  
  // closeTimer
  closeTimer = 30;
  __loop = closeGateTimer;
}

void closeGate() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("close gate");
  digitalWrite(MotorP, HIGH);
  digitalWrite(MotorN, LOW);
}

void closeGateTimer() {
  if(closeTimer > 0) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("closing gate in ");
    lcd.setCursor(0,1);
    lcd.print( String(closeTimer) + "s");
    delay(1000);
    closeTimer -= 1;
    return;
  }

  closeGate();
  __loop = __observe;
}

void stopGate() {
  if(digitalRead(HaltPin1) == HIGH && digitalRead(HaltPin2) == LOW) { // btn not pressed
    halt = false;
    return;
  }
  if(halt) return ; // halt logic already executed

  halt = true;
  lcd.setCursor(13,1);
  lcd.print("<s>");
  digitalWrite(MotorP, LOW);
  digitalWrite(MotorN, LOW);
}

MenuItem option0 = MenuItem("replay memory", replay_memory);
MenuItem option1 = MenuItem("erase memory", erase_memory);
MenuItem option2 = MenuItem("read tag", observe);
MenuItem option3 = MenuItem("openGate", openGate);
MenuItem option4 = MenuItem("closeGate", closeGate);
MenuItem option5 = MenuItem("regAllowCard", regAllowCard);
MenuItem option6 = MenuItem("regAllowCard2", regSecondaryAllowCard);
MenuItem option7 = MenuItem("regDenyCard", regDenyCard);
MenuItem option8 = MenuItem("delete everything", deleteEverything);

MenuItem menuItems [] = {option0, option1, option2, option3, option4, option5, option6, option7, option8};

Menu menu = Menu(menuItems, 9, printMenu);

void setup()
{
 lcd.begin(16, 2);              // start the library
 Serial.begin(115200);
 while (!Serial);
  SPI.begin();           // MFRC522 Hardware uses SPI protocol
  mfrc522.PCD_Init();    // Initialize MFRC522 Hardware
  if(!getReaderInfo()){
    printRfidError();
    while(true);
  }

  // setup motor control
  pinMode(HaltPin1, INPUT_PULLUP);
  pinMode(HaltPin2, INPUT_PULLUP);
  pinMode(MotorP, OUTPUT);
  pinMode(MotorN, OUTPUT);

  __loop = __observe; // start in observe mode;
}

void loop()
{
 // debug
 bool keyEvent = checkInputState();

 if(keyEvent && buttonState != BTN_NONE){
  menu.update(buttonState);
 }

 (*__loop)(); // call method loop

 stopGate(); // check for halt irrespective of operation

}
