// motion sensor
#define MY_DEBUG 
#define MY_RADIO_NRF24
#define MY_NODE_ID 11
#define MY_REPEATER_FEATURE
#include <MySensor.h>
#include <SPI.h>

#define CHILD_ID_PIR 1

#define PIR_PIN 2

MyMessage msgPir(CHILD_ID_PIR, V_TRIPPED); // 16

byte StatePIR=0;
byte oldStatePIR=0;
const unsigned long tUpdate=300000; // update interval if no movement detected
unsigned long t0;

void presentation() {
  sendSketchInfo("Motion Sensor", "1.0");
  present(CHILD_ID_PIR, S_MOTION);  
}

void setup() {
  pinMode(PIR_PIN, INPUT_PULLUP);
  t0=millis();
}

void loop() {
  StatePIR=digitalRead(PIR_PIN);
  if (StatePIR != oldStatePIR) {
    oldStatePIR=StatePIR;
    ServerUpdate();
  }
  if ((millis()-t0) > tUpdate) ServerUpdate();
  
}

void ServerUpdate() {
  send(msgPir.set(StatePIR ? "ON" : "OFF"));
  t0=millis();
  
}

