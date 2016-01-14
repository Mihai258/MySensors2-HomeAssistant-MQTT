// get SW_PIN digital status and send it to the gateway as "ON" or "OFF" status
#define MY_DEBUG 
#define MY_RADIO_NRF24
#define MY_NODE_ID 12
#define MY_REPEATER_FEATURE
#include <SPI.h>
#include <MySensor.h>
#include <Bounce2.h>

#define CHILD_ID_SW 1

#define SW_PIN 3

MyMessage msgSW(CHILD_ID_SW, V_TRIPPED); // 16

Bounce debouncer = Bounce(); 
int oldValueSW=-1;
int valueSW=0;
const unsigned long tUpdate=300000; //update interval if input do not change
unsigned long t0;

void presentation() {
  sendSketchInfo("Switch Sensor", "1.0");
  present(CHILD_ID_SW, S_DOOR);  
}


void setup() {
  pinMode(SW_PIN,INPUT);
  digitalWrite(SW_PIN,HIGH);
  debouncer.attach(SW_PIN);
  debouncer.interval(5);

}

void loop() {
  debouncer.update();
  valueSW = debouncer.read();
 
  if (valueSW != oldValueSW) {
     ServerUpdate();
     oldValueSW = valueSW;
  }
  if ((millis()-t0) > tUpdate) ServerUpdate();
}

void ServerUpdate() {
  send(msgSW.set(valueSW==HIGH ? "ON" : "OFF"));
  t0=millis();
}

