/*
  WirelessDoorBell - Library for sending and receiving codes to "Select Plus Wireless doorbell"
  Created by Bram Harmsen, Januarie 15, 2015
  Released into the public domain.
*/


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

}