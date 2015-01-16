/*
  DoorBellTransmitter - Library for sending and receiving codes to "Select Plus Wireless doorbell"
  Created by Bram Harmsen, Januarie 15, 2015
  Released into the public domain.
*/

#include "Arduino.h"
#include "DoorBellTransmitter.h"

// Constructor
DoorBellTransmitter::DoorBellTransmitter( unsigned char txPin, unsigned long code, unsigned int period, unsigned char codeRepeat, unsigned char messageRepeat ) {
  _txPin          = txPin;
  _code           = code;
  _shortPulse     = period*1/4;
  _longPulse      = period*3/4;
  _syncPulse      = period*4 - period*1/8;
  _codeRepeat     = codeRepeat;
  _messageRepeat  = messageRepeat;
}

// Public Methods //////////////////////////////////////////////////////////////

void DoorBellTransmitter::ring(){
  // send mesRep complete messages
  for( byte i = _messageRepeat; i > 0; i--) {
  
    // Send sync at beginning of message
    sendFirstSync();
  
    // send codeRep codes
    for ( byte j = _codeRepeat; j > 0; j--) {      
      sendSingleCode();      
    }
    digitalWrite(_txPin, LOW);
    // short delay before sending next message
    delay( 5 );  
  }
}

void DoorBellTransmitter::printConfig(){

  if(Serial) {
    Serial.println(F("SELECT PLUS - Wireless Doorbell"));
    Serial.println(F(""));
    Serial.print(F("Trasmitter TX pin: "));
    Serial.println(_txPin);
    Serial.print(F("Code: 0"));
    Serial.println(_code, BIN);
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

// Private Methods /////////////////////////////////////////////////////////////

void DoorBellTransmitter::sendSingleCode() {

  // divisor MSB = 1, all other 0
  unsigned long div = 1;
  div <<= 16;

  // mask, only first 17 bit are important
  _code &= 131071;

  // scan bits in _code, send1() for '1' and send0() for '0'
  while ( div > 0 ) {
    if ( _code >= div ) {
      send1();
      _code -= div;
    } else {
      send0();
    }
    div >>= 1;
  }
  sendSecondSync();
}

// Inverse pulses (makes more sense)

//          __________________        
// 'S1' |__|                  |______
void DoorBellTransmitter::sendFirstSync() {
  digitalWrite(_txPin, HIGH);
  delayMicroseconds(_shortPulse);
  sendSecondSync();
}
//       __________________
// 'S2' |                  |______
void DoorBellTransmitter::sendSecondSync() {
  digitalWrite(_txPin, LOW);
  delayMicroseconds(_syncPulse);
  digitalWrite(_txPin, HIGH);
  delayMicroseconds(_longPulse);
}
//      __
// '0'  | |______ short-long
void DoorBellTransmitter::send0(){
  digitalWrite(_txPin, LOW);
  delayMicroseconds(_shortPulse);
  digitalWrite(_txPin, HIGH);
  delayMicroseconds(_longPulse);
}
//      ______
// '1'  |     |__ long-short
void DoorBellTransmitter::send1(){
  digitalWrite(_txPin, LOW);
  delayMicroseconds(_longPulse);
  digitalWrite(_txPin, HIGH);
  delayMicroseconds(_shortPulse);
}