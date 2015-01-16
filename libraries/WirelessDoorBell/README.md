/* 
# General introduction

This library can be used to send an received code to/from a SELECT PLUS Wireless Doorbell. In the Netherlands these where sold at the Action store (jan 2015). Perhaps they are also sold at other stores using a different brand name. 

To identify if your unit is compatible with this library; the transmitter PCB is labled with: QH-C-CE-3V Rev1.0 Quhwa and the main IC has type number: MC908T 1439N. The receiver PCB is labled with: QH-832AC Rev1.0 Quhwa and has two IC's: TR1834B and TX102R. All of these IC are undocumented, at least I couldn't find any documentation.

The transmitter uses simple OOK/ASK modulation sheme. As far as I understand this, this just means that the transmitter is just turned ON and OFF. When the transmitter is ON it will transmit a 433 MHz signal and when it's OFF, nothing will be transmitted.

# Signal
I only tested this library with a single wireless doorbell, so it could well be
possible that another doorbell uses another code.

This is how the signal looks like:
\__                             \______    ______        __    ______        __
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

Using the above scheme, the code my wireless doorbell transmitter is sending is: X10101FFF0XXXXXXXX

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

Using the above improved sheme, the code my wireless doorbell transmitter is sending is: 0 1010 1111 0000 0000

Since the whole code is based on edge detection we actualy don't have to inverse the signal anyway.

The most important part of encoding/decoding the signal is timing. I have found the following timing:

period:    ~1500 us
  short:    ~375  us (1/4 of period)
  long:     ~1125 us (3/4 of period)
sync low:  ~5800 us ( 4 * period - 1/8 * period )

# Using the library
