
#include <DoorBellReceiver.h>

DoorBellReceiver doorbellReceiver(0, 1, myCallBack);

void setup() {
  Serial.begin(9600);
}

void loop() {

}

void myCallBack(unsigned int code) {

  Serial.println(code, BIN);

}