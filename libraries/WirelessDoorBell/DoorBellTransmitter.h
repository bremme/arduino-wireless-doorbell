/*
  DoorBellTransmitter - Library for sending and receiving codes to "Select Plus Wireless doorbell"
  Created by Bram Harmsen, Januarie 15, 2015
  Released into the public domain.
*/

#ifndef DoorBellTransmitter_h
#define DoorBellTransmitter_h

#include "Arduino.h"

class DoorBellTransmitter {
  public:
    DoorBellTransmitter(unsigned char txPin = 11,  char charCode[19] = "X10101FFF0XXXXXXXX",  unsigned int period = 1500, unsigned char codeRepeat = 6, unsigned char messageRepeat = 6 );
    void ring();
    // void setCharCode();
    
    unsigned int getShortPulse();
    unsigned int getLongPulse();
    void printConfig();
    // void setShortPulse();
    // void setCodeRepeat();
  private:
    unsigned char _txPin;
    unsigned int _shortPulse;
    unsigned int _longPulse;
    unsigned char _codeRepeat;
    unsigned char _messageRepeat;
    unsigned char _messageDelay;
    char * _charCode;

    void sendSingleCode();
    void sendSync();
    void sendLow();
    void send0();
    void send1();
    void sendX();
    void sendF();
};

#endif