// Get analog value from defined Analog_PIN and send it to the gateway
#define MY_DEBUG 
#define MY_RADIO_NRF24
#define MY_NODE_ID 14
#define MY_REPEATER_FEATURE
#include <SPI.h>
#include <MySensor.h>;

#define CHILD_ID_Analog1 1

#define Analog_PIN A0

int InA1;

MyMessage msgAnalog(CHILD_ID_Analog1, V_VAR1); //24 .. 28

const unsigned long tUpdate=60000; //update interval
unsigned long t0;

void presentation()  {
  sendSketchInfo("Analog Sensor", "1.0");
  present(CHILD_ID_Analog1, S_CUSTOM);
}
void setup() {
  t0=millis();
}

void loop() {
  if ((millis()-t0) > tUpdate) ServerUpdate();
}

void ServerUpdate() {
  InA1=analogRead(Analog_PIN);
  send(msgAnalog.set(InA1));
  t0=millis();
}

