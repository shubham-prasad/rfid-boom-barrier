#ifndef memory_h
#define memory_h

#include <EEPROM.h>

byte TempCard[4] ;

void wipeAll() {
  for (uint16_t x = 0; x < EEPROM.length(); x = x + 1) { //Loop end of EEPROM address
    if (EEPROM.read(x) == 0) continue ; // already clear
    EEPROM.write(x, 0);       // if not write 0 to clear, it takes 3.3mS
  }
}

byte* readCard(int offset){
  for ( uint8_t j = 0; j < 4; j++ ) {
    TempCard[j] = EEPROM.read(offset + j);
  }
  return TempCard;
}

void storeCard(int offset, byte* card){
  for ( uint8_t j = 0; j < 4; j++ ) {
    EEPROM.write(offset + j, card[j]);
  }
}

#endif
