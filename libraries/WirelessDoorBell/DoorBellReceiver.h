/*
  WirelessDoorBell - Library for sending and receiving codes to "Select Plus Wireless doorbell"
  Created by Bram Harmsen, Januarie 15, 2015
  Released into the public domain.
*/

/* 
# General introduction

This library can be used to send an received code to/from a SELECT PLUS Wireless Doorbell. In the Netherlands these where sold at the Action store (jan 2015). Perhaps they are also sold at other stores using a different brand name. 

To identify if your unit is compatible with this library; the transmitter PCB is labled with: QH-C-CE-3V Rev1.0 Quhwa and the main IC has type number: MC908T 1439N. The receiver PCB is labled with: QH-832AC Rev1.0 Quhwa and has two IC's: TR1834B and TX102R. All of these IC are undocumented, at least I couldn't find any documentation.

# Signal
I only tested this library with a single wireless doorbell, so it could well be
possible that another doorbell uses another code.

This is how the signal looks like:
 __                              ______    ______        __    ______        __
|  |____________________________|      |__|      |______|  |__|      |______|  |

   ______        __        __        __        __    ______    ______    ______
__|      |______|  |______|  |______|  |______|  |__|      |__|      |__|      |

   ______    ______    ______    ______    ______
__|      |__|      |__|      |__|      |__|      |

One way to look at the above signal is to use 4 different bits and a sync signal:
 __
|  |____________________________  : SYNC
 ______
|      |__        : LONG - SHORT  = 'X'
 ______
|      |______    : LONG - LONG   = '1'
 __
|  |__            : SHORT - SHORT = '0'
 __
|  |______        : SHORT - LONG  = 'F'

But it makes more sense to inverse the received signal, since then we have only 2 bits:
    ____________________________        __        ______    __        ______
 __|                            |______|  |______|      |__|  |______|      |__
 __        ______    ______    ______    ______    __        __        __        
|  |______|      |__|      |__|      |__|      |__|  |______|  |______|  |______
 __        __        __        __        __       
|  |______|  |______|  |______|  |______|  |______

   ____________________________ 
__|                            |______  : SYNC
 __
|  |______                              : SHORT - LONG  = '0'
 ______
|      |__                              : LONG  - SHORT = '1'

Since the whole code is based on edge detection we actualy don't have to inverse the signal anyway.

The most important part of encoding/decoding the signal is timing. I have found the following timing:

period:    ~1500 us
  short:    ~375  us (1/4 of period)
  long:     ~1125 us (3/4 of period)
sync low:  ~5800 us

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
              If < 0, you must call interruptHandler() yourself.
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
    static void printConfig();
    static void setMargin( unsigned int pulseMarging, unsigned int syncPulseMargin );
    static void setPeriod( unsigned int period );
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
    static unsigned int             _pulseMargin;    // Pulse marging

};

#endif