// get MQ2 sensor input and send it to the gateway
#define MY_DEBUG 
#define MY_RADIO_NRF24
#define MY_NODE_ID 16
#define MY_REPEATER_FEATURE
#include <MySensor.h>
#include <SPI.h>

#define CHILD_ID_GAS 1

#define MQ_PIN A0

MyMessage msgGas(CHILD_ID_GAS, V_VAR1); //24

int InAnalog1;
int valMQ=0;
int refMQ=1023; // refference value will be the lowest value read from the sensor
const unsigned long tUpdate=60000; // update interval data
unsigned long t0, ts0;

void presentation() {
  sendSketchInfo("GAS Sensor", "1.0");
  present(CHILD_ID_GAS, S_CUSTOM);
}

void setup() {
  t0=millis();
}

void loop() {
  if ((millis()-t0) > tUpdate) ServerUpdate();
}

void ServerUpdate() {
  InAnalog1=analogRead(MQ_PIN);
  if (InAnalog1<refMQ) refMQ=InAnalog1; // if read value is lower that actual, change the 0 refference with the actual one
  valMQ=InAnalog1-refMQ;
  send(msgGas.set(valMQ));
  t0=millis();
}

