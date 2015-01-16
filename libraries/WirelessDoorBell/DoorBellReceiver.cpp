

#include "DoorBellReceiver.h"

byte                      DoorBellReceiver::_interrupt;
volatile char             DoorBellReceiver::_state;
byte                      DoorBellReceiver::_minRepeats;
DoorBellReceiverCallback  DoorBellReceiver::_callback;
boolean                   DoorBellReceiver::_inCallback      = false;
boolean                   DoorBellReceiver::_enabled         = false;
unsigned int              DoorBellReceiver::_shortPulse      = 375;
unsigned int              DoorBellReceiver::_longPulse       = 1125;
unsigned int              DoorBellReceiver::_syncPulse       = 5873;
unsigned int              DoorBellReceiver::_syncPulseMargin = 300;
unsigned int              DoorBellReceiver::_pulseMargin     = 100;



// Constructor /////////////////////////////////////////////////////////////////


// Public //////////////////////////////////////////////////////////////////////
DoorBellReceiver::DoorBellReceiver( int8_t interrupt, byte minRepeats, DoorBellReceiverCallback callback ) {
  if (interrupt == 0 || interrupt == 1) {
    _interrupt  = interrupt;
    _minRepeats = minRepeats;
    _callback   = callback;
    enable();
    if (Serial) {
      Serial.println(F("Initialization succes!"));
      Serial.println(F(""));
    }
    attachInterrupt( interrupt, interruptHandler, CHANGE);  
  } else {
    if (Serial) {
      Serial.println(F("Initialization failed, wrong interrupt (can only be 0 or 1"));
      Serial.println(F(""));
    }
  }
}

void DoorBellReceiver::enable() {
  _state    = -2;
  _enabled  = true;
}

void DoorBellReceiver::disable() {
  _enabled  = false;
}

void DoorBellReceiver::setMargin(unsigned int pulseMargin, unsigned int syncPulseMargin ) {
  _pulseMargin     = pulseMargin;
  _syncPulseMargin = syncPulseMargin;
}

void DoorBellReceiver::setPeriod(unsigned int period) {
  _longPulse = period*3/4;
  _shortPulse= period*1/4;
}

void DoorBellReceiver::setMinRepeats( byte minRepeats ) {
  _minRepeats = minRepeats;
}

void DoorBellReceiver::printConfig() {

  if(Serial) {
    Serial.println(F("SELECT PLUS - Wireless Doorbell - Receiver"));
    Serial.println(F(""));
    Serial.print(F("Interrupt: "));
    Serial.println(_interrupt);
    Serial.print(F("Short pulse: "));
    Serial.print(_shortPulse);
    Serial.print(F(" us, Long pulse: "));
    Serial.print(_longPulse);
    Serial.println(F(" us"));
    Serial.print(F("Pulse margin: "));
    Serial.print(_pulseMargin);
    Serial.print(F(" us"));
    Serial.print(F(" , Sync pulse margin: "));
    Serial.print(_syncPulseMargin);
    Serial.println(F(" us"));
    Serial.println(F(""));
  }

}
// Private /////////////////////////////////////////////////////////////////////

void DoorBellReceiver::interruptHandler() {
  // if received is disabled return
  if (!_enabled) {
    return;
  }
  // initialize local variables
  static unsigned long  edgeTimeStamp[3]  = {0, };
  static unsigned long  pulseLength       = 0;
  static boolean        skip              = false;
  static byte           receivedBit;
  static unsigned int   receivedCode      = 0;
  static unsigned int   previousCode      = 0;
  static byte           repeats           = 0;

  // if skip (last edge was too short) return
  if (skip) {
    skip = false;
    return;
  }

  // get timestamps
  edgeTimeStamp[1]  = edgeTimeStamp[2];
  edgeTimeStamp[2]  = micros();

  // LP filter (skip when edged are too short)
  if ( edgeTimeStamp[2] - edgeTimeStamp[1] < _shortPulse - _pulseMargin ) {
    skip = true;  
    return;
  }
  
  pulseLength       = edgeTimeStamp[1] - edgeTimeStamp[0];
  edgeTimeStamp[0]  = edgeTimeStamp[1];
  
  // Waiting for first sync part (very long) ///////////////////////////////////
  if ( _state == -2 ) {
    if ( !((pulseLength > (_syncPulse - _syncPulseMargin)) && (pulseLength < (_syncPulse + _syncPulseMargin))) ) {
      return;
    }      
  }
  // Waiting for second sync part (long) ///////////////////////////////////////
  else if ( _state == -1 ) {
    if ( !(pulseLength > ( _longPulse - _pulseMargin)) && ( pulseLength < (_longPulse + _pulseMargin)) ) {
      _state = -2;
      return;  
    }
    receivedCode = 0;    
  }
  // Decoding 17 bits (0..16) //////////////////////////////////////////////////
  else if (_state < 34 ) {
    // shift current receiving bit 1 to the left
    receivedBit <<= 1;
    // receiving short pulse
    if ( pulseLength <= _shortPulse + _pulseMargin ) {
      receivedBit &= B10; // Clear LSB of receivedBit  
    }
    // receiving long pulse
    else if ( pulseLength >  _longPulse - _pulseMargin &&  pulseLength < _longPulse + _pulseMargin ) {
      receivedBit |= B1; //  Set LSB of receivedBit  
    }
    // receiving rubbish -> reset
    else {
      _state = -2;
      return;
    }
    // second pulse received, decode bit
    if ( (_state%2) == 1 ) {
      receivedCode *= 2;  // Shift receivedCode
      switch( receivedBit & B11 ) {// only 2 LSB's are used; trim the rest
        case B01:  // short - long = '0'          
          receivedCode += 0;
          break;
        case B10:  // long - short = '1'          
          receivedCode += 1;
          break;
        default:
          _state = -2;
          return; 
      }
    }
  }
  // Complete code received! ///////////////////////////////////////////////////
  else if ( _state == 34 ) {

    if ( receivedCode != previousCode ) {
      repeats = 0;
      previousCode = receivedCode;
    }    
    repeats++;

    if ( repeats >= _minRepeats ) {
      if ( !_inCallback ) {
        _inCallback = true;
        ( _callback )( receivedCode );
        _inCallback = false;
        repeats = 0;
      }
    }
    // if first sync part received jump to state -1
    if ( ((pulseLength > (_syncPulse - _syncPulseMargin)) && (pulseLength < (_syncPulse + _syncPulseMargin))) ) {
      _state = -1;
      return;
    } else {
      _state = -2;
      return;
    }  
  }

  _state++;
  return;

}