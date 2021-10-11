# General introduction

This library can be used to send an received code to/from a SELECT PLUS Wireless Doorbell. In the Netherlands these where sold at the Action store (jan 2015). Perhaps they are also sold at other stores using a different brand name.

To identify if your unit is compatible with this library; the transmitter PCB is labled with: QH-C-CE-3V Rev1.0 Quhwa and the main IC has type number: MC908T 1439N. The receiver PCB is labled with: QH-832AC Rev1.0 Quhwa and has two IC's: TR1834B and TX102R. All of these IC are undocumented, at least I couldn't find any documentation.

The transmitter uses simple OOK/ASK modulation sheme. As far as I understand this, this just means that the transmitter is just turned ON and OFF. When the transmitter is ON it will transmit a 433 MHz signal and when it's OFF, nothing will be transmitted.

# Signal

I only tested this library with a single wireless doorbell, so it could well be
possible that other doorbells uses other codes.

This is how the signal looks like:
```
 __                              ______    ______        __    ______        __
|  |____________________________|      |__|      |______|  |__|      |______|  |

   ______        __        __        __        __    ______    ______    ______
__|      |______|  |______|  |______|  |______|  |__|      |__|      |__|      |

   ______    ______    ______    ______    ______
__|      |__|      |__|      |__|      |__|      |
```
One way to look at the above signal is to use 4 different bits and a sync signal:
```
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
```
Using the above scheme, the code my wireless doorbell transmitter is sending is: X10101FFF0XXXXXXXX

But it makes more sense to inverse the received signal, since then we only need 2 bits:
```
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
```
Using the above improved scheme, the code my wireless doorbell transmitter is sending is: 0 1010 1111 0000 0000 (decimal: 44800)

Since the whole code is based on edge detection we actualy don't have to inverse the signal anyway.

The most important part of encoding/decoding the signal is timing. I have found the following timing:

period:    ~1500 us
  short:    ~375  us (1/4 of period)
  long:     ~1125 us (3/4 of period)
sync low:  ~5800 us ( 4 * period - 1/8 * period )

# Using the library

To use the library, you have to download the files and add them to your arduino libraries folder.

The library consist of two classes, a receiver (DoorBellReceiver) and a transmitter class (DoorBellTransmitter). You can use either one or both in a sketch. The default pins are:

Receiver RX:    PIN 2 (arduino interrupt 0)
Transmitter TX: PIN 11

You could change the pins, by calling the constructors with different arguments.

## Using the receiver

A simple example could look like this:

```c++
#include <DoorBellReceiver.h>

void setup() {
    Serial.begin(9600);
    DoorBellReceiver doorbellReceiver(0, 1, myCallBack);
}

void loop(){}

void myCallBack(unsigned int code) {
  // Print received code
  Serial.print(F("Received code: "));
  Serial.println(code);
}
```
## using the transmitter

A simple example could look like this:

```c++
#include <DoorBellTransmitter.h>

DoorBellTransmitter bell;

void setup(){
  Serial.begin(9600);
  bell.printConfig();
}

void loop(){
  char buffer[7];

  if (Serial.available() > 5) {
    Serial.readBytes(buffer,6);
    if ( (buffer[0] == 'r') && (buffer[1] == 'i') && (buffer[2] == 'n') && (buffer[3] == 'g') ) {
      Serial.println("Ringing door bell");
      bell.ring();
    } else {
      Serial.println("Unknown command");
    }
  }
}
```
