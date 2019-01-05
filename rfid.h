#ifndef rfid_h
#define rfid_h

#include "memory.h"

extern MFRC522 mfrc522;

byte CurrentCard[4];

const char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

String hexStr(byte *data, int len)
{
  String s = "";
  for (int i = 0; i < len; ++i) {
    s += hexmap[(data[i] & 0xF0) >> 4];
    s += hexmap[data[i] & 0x0F];
    s += " ";
  }
  return s;
}

bool Equal(byte* first, byte* second){
  Serial.println(hexStr(first, 4));
  Serial.println(hexStr(second, 4));
  for (int i = 0; i < 4; ++i)
  {
    if(first[i] != second[i]) {
      return false;
    }
  }
  return true;
}

#define AllowCardOffset 1
#define AllowCardOffset2 5
#define DenyCardOffset 9

enum Card {
  Card_Allow,
  Card_Allow_2,
  Card_Deny,
  Card_Unknown
};

int checkCard(byte* card){
  if(Equal(readCard(AllowCardOffset), card))
    return Card_Allow;
  if(Equal(readCard(AllowCardOffset2), card))
    return Card_Allow_2;
  else if(Equal(readCard(DenyCardOffset), card))
    return Card_Deny;
  else
    return Card_Unknown;
}

void registerCard(byte* card, int type){
  if(type == Card_Allow){
    storeCard(AllowCardOffset, card);
  }
  if(type == Card_Allow_2){
    storeCard(AllowCardOffset2, card);
  }
  else if(type == Card_Deny){
   storeCard(DenyCardOffset, card);
  }
  else {
    Serial.println("invalid store location");
  }
}

bool getReaderInfo() {
  // Get the MFRC522 software version
  Serial.println("++++++++ reader info ++++++++");
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown),probably a chinese clone?"));
  Serial.println("");
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
    Serial.println(F("SYSTEM HALTED: Check connections."));
    return false;
  }
  Serial.println("-------- reader info --------");
  return true;
}

bool checkForCard() {
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return false;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return false;
  }

  // There are Mifare PICCs which have 4 byte or 7 byte UID care if you use 7 byte PICC
  // I think we should assume every PICC as they have 4 byte UID
  // Until we support 7 byte PICCs
  Serial.println(F("Scanned PICC's UID:"));
  for ( uint8_t i = 0; i < 4; i++) {  //
    CurrentCard[i] = mfrc522.uid.uidByte[i];
    Serial.print(CurrentCard[i], HEX);
  }
  Serial.println("");
  mfrc522.PICC_HaltA(); // Stop reading
  return true;
}

void waitForCard(){
  while(!checkForCard()); // loop until a card is read
}

#endif
