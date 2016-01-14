// relay sensor, 4 relays included; state not saved
#define MY_DEBUG 
#define MY_RADIO_NRF24
#define MY_NODE_ID 10
#define MY_REPEATER_FEATURE
#include <SPI.h>
#include <MySensor.h>

#define CHILD_ID_REL1 1
#define CHILD_ID_REL2 2
#define CHILD_ID_REL3 3
#define CHILD_ID_REL4 4

#define REL1_PIN 8
#define REL2_PIN 7
#define REL3_PIN 6
#define REL4_PIN 5

MyMessage msgREL1(CHILD_ID_REL1, V_LIGHT);  // 2
MyMessage msgREL2(CHILD_ID_REL2, V_LIGHT);
MyMessage msgREL3(CHILD_ID_REL3, V_LIGHT);
MyMessage msgREL4(CHILD_ID_REL4, V_LIGHT);

byte StateREL=0, StateREL1=0, StateREL2=0, StateREL3=0, StateREL4=0;

void setup() {
  pinMode(REL1_PIN, OUTPUT);
  digitalWrite(REL1_PIN, StateREL1);
  pinMode(REL2_PIN, OUTPUT);
  digitalWrite(REL2_PIN, StateREL2);
  pinMode(REL3_PIN, OUTPUT);
  digitalWrite(REL3_PIN, StateREL3);
  pinMode(REL4_PIN, OUTPUT);
  digitalWrite(REL4_PIN, StateREL4);

}

void presentation()  {
  sendSketchInfo("Relay", "1.0");
  present(CHILD_ID_REL1, S_LIGHT);
  present(CHILD_ID_REL2, S_LIGHT);
  present(CHILD_ID_REL3, S_LIGHT);
  present(CHILD_ID_REL4, S_LIGHT);
}


void loop() {

}

void receive(const MyMessage &message) {
  // We only expect one type of message from controller. But we better check anyway.
  if (message.type==V_LIGHT) {
    StateREL=strcmp(message.data,"ON")==0;
    switch(message.sensor) {
      case 1: {
        StateREL1=StateREL;
        if (StateREL) {
          digitalWrite(REL1_PIN, HIGH);
        }
        else {
          digitalWrite(REL1_PIN, LOW);
        }
        send(msgREL1.set(StateREL1 ? "ON":"OFF"));
      }
      break;
      case 2: {
        StateREL2=StateREL;
        if (StateREL) {
          digitalWrite(REL2_PIN, HIGH);
        }
        else {
          digitalWrite(REL2_PIN, LOW);
        }
        send(msgREL2.set(StateREL2 ? "ON":"OFF"));
      }
      break;
      case 3: {
        StateREL3=StateREL;
        if (StateREL) {
          digitalWrite(REL3_PIN, HIGH);
        }
        else {
          digitalWrite(REL3_PIN, LOW);
        }
        send(msgREL3.set(StateREL3 ? "ON":"OFF"));
      }
      break;
      case 4: {
        StateREL4=StateREL;
        if (StateREL) {
          digitalWrite(REL4_PIN, HIGH);
        }
        else {
          digitalWrite(REL4_PIN, LOW);
        }
        send(msgREL4.set(StateREL4 ? "ON":"OFF"));
      }
      break;
    }

   } 
}
