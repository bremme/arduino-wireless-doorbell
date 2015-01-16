/*
  DoorBellTransmitter - Library for sending and receiving codes to "Select Plus Wireless doorbell"
  Created by Bram Harmsen, Januarie 15, 2015
  Released into the public domain.
*/

#include "Arduino.h"
#include "DoorBellTransmitter.h"

// Constructor
DoorBellTransmitter::DoorBellTransmitter( unsigned char txPin, char *charCode, unsigned int period, unsigned char codeRepeat, unsigned char messageRepeat ) {
  _txPin          = txPin;
  _charCode       = charCode;
  _shortPulse     = period/4;
  _longPulse      = period*3/4;
  _codeRepeat     = codeRepeat;
  _messageRepeat  = messageRepeat;
}

// Public Methods //////////////////////////////////////////////////////////////

void DoorBellTransmitter::ring(){
  // send mesRep complete messages
  for( byte i = _messageRepeat; i > 0; i--) {
  
    // Send sync at beginning of message
    sendSync();
  
    // send codeRep codes
    for ( byte j = _codeRepeat; j > 0; j--) {      
      sendSingleCode();      
    }
    
    // short delay before sending next message
    delay( _messageDelay );  
  }
}

void DoorBellTransmitter::printConfig(){

  if(Serial) {
    Serial.println(F("SELECT PLUS - Wireless Doorbell"));
    Serial.println(F(""));
    Serial.print(F("Trasmitter TX pin: "));
    Serial.println(_txPin);
    Serial.print(F("Character code: "));
    for ( byte i=0 ; i < 19; i++ ) {
      Serial.print(_charCode[i]);
    }
    Serial.println(F(""));
    Serial.print(F("Short pulse: "));
    Serial.print(_shortPulse);
    Serial.print(F(" us, Long pulse: "));
    Serial.print(_longPulse);
    Serial.println(F(" us"));
    Serial.print(F("Code repeats: "));
    Serial.print(_codeRepeat);
    Serial.print(F(" , Message repeats: "));
    Serial.println(_messageRepeat);
    Serial.println(F(""));
  }

}

unsigned int DoorBellTransmitter::getShortPulse() {
  return _shortPulse;
}
unsigned int DoorBellTransmitter::getLongPulse() {
  return _longPulse;
}
// Private Methods /////////////////////////////////////////////////////////////

void DoorBellTransmitter::sendSingleCode() {

  for( byte i=0; i < 19; i++ ){
  
    switch( _charCode[i]) {    
      case '0':
        send0();        
        break;
      case '1':
        send1();
        break;
      case 'F':
        sendF();
        break;
      case 'X':
        sendX();
        break;
    }
  }
  sendLow();
}

//          __
// 'Sync'  | |________________ short-very-long
void DoorBellTransmitter::sendSync(){
  digitalWrite(_txPin,HIGH);
  delayMicroseconds(_shortPulse);
  sendLow();
}
//
// 'Low'  ________________ very-long
void DoorBellTransmitter::sendLow(){
  digitalWrite(_txPin,LOW); 
  delayMicroseconds(((_shortPulse + _longPulse)*4) - _shortPulse);
}
//      __
// '0'  | |__ short-short
void DoorBellTransmitter::send0(){
  digitalWrite(_txPin, HIGH);
  delayMicroseconds(_shortPulse);
  digitalWrite(_txPin, LOW);
  delayMicroseconds(_shortPulse);
}
//      ______
// '1'  |     |______ long-long
void DoorBellTransmitter::send1(){
  digitalWrite(_txPin, HIGH);
  delayMicroseconds(_longPulse);
  digitalWrite(_txPin, LOW);
  delayMicroseconds(_longPulse);
}
//      ______
// 'X'  |     |__ long-short
void DoorBellTransmitter::sendX(){
  digitalWrite(_txPin, HIGH);
  delayMicroseconds(_longPulse);
  digitalWrite(_txPin, LOW);
  delayMicroseconds(_shortPulse);
}
//      __
// 'F'  | |______ short-long
void DoorBellTransmitter::sendF(){
  digitalWrite(_txPin, HIGH);
  delayMicroseconds(_shortPulse);
  digitalWrite(_txPin, LOW);
  delayMicroseconds(_longPulse);
}