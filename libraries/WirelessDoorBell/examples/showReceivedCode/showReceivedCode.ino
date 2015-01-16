
#include <DoorBellReceiver.h>

DoorBellReceiver doorbellReceiver(0, 1, myCallBack);

void setup() {
  // Setup serial connection 9600
  Serial.begin(9600);
  // Print receiver config
  doorbellReceiver.printConfig();
}

void loop() {

}

void myCallBack(unsigned int code) {

  // Print received code
  Serial.print(F("Received code: "));
  Serial.println(code);

}