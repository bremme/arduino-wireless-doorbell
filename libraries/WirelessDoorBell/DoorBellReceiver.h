/*
  WirelessDoorBell - Library for sending and receiving codes to "Select Plus Wireless doorbell"
  Created by Bram Harmsen, Januarie 15, 2015
  Released into the public domain.
  Licence: GNU GENERAL PUBLIC LICENSE V2.0
*/

#ifndef DoorBellReceiver_h
#define DoorBellReceiver_h

#include "Arduino.h"

typedef void (*DoorBellReceiverCallback)(unsigned int);

class DoorBellReceiver {
  public:
    /**
    * Initializes the decoder.
    *
    * @param interrupt  The interrupt as is used by Arduino's attachInterrupt function. See attachInterrupt for details.
    * @param minRepeats The number of times the same code must be received in a row before the callback is called
    * @param callback Pointer to a callback function, with signature void (*func)(unsigned int). First parameter is the decoded data
    */
    DoorBellReceiver(int8_t interrupt, byte minRepeats, DoorBellReceiverCallback callback);
    /**
    * Enable decoding.
    */
    static void enable();
    /**
    * Disable decoding. You can re-enable decoding by calling enable();
    */
    static void disable(); 
    /**
    * Print current configuration over serial ( call Serial.begin(9600) before using )
    */
    static void printConfig();
    /**
    * Update the timing margings
    *
    * A good pulse is shortPulse +- pulseMargin, longPulse +- pulseMargin and syncPulse +- syncPulseMarging
    * 
    * @param pulseMargin      Margin for short and long pulse, default = 100 us
    * @param syncPulseMargin  Margin for sync pulse default = 300 us
    */
    static void setMargin( unsigned int pulseMarging, unsigned int syncPulseMargin );
    /**
    * Update period
    *
    * The period is the time in us of one full period, e.g. short pulse, long pulse.
    * 
    * @param period           Period duration, default = 1500
    */
    static void setPeriod( unsigned int period );
    /**
    * Update the minimum number of code repeats
    *
    * @param minRepeats       Minimum number of repeated codes before calling callback
    */
    static void setMinRepeats( byte minRepeats );
    /**
    * Handle the arduino interrupt
    * The interrupt handler is called whenever the interrupt pin changes.
    */   
    static void interruptHandler();
  private:
    static byte                     _interrupt;       // Radio input (RX)
    volatile static char            _state;           // State of decoding process. There are 37 state, -2 for waiting first sync part, -1 for waiting second sync part and 0..37 for decoding bits.
    static byte                     _minRepeats;      // minimum number of repeats before calling callback
    static DoorBellReceiverCallback _callback;        // user defined callback function
    static boolean                  _inCallback;      // In callback flag
    static boolean                  _enabled;         // Enabled flag
    static unsigned int             _shortPulse;      // Short pulse length
    static unsigned int             _longPulse;       // Long pulse length
    static unsigned int             _syncPulse;       // Sync pulse length
    static unsigned int             _syncPulseMargin; // Sync pulse marging
    static unsigned int             _pulseMargin;     // Pulse marging

};

#endif