// read the light value from the KY018 sensor and send it to the gateway as Lux value
#define MY_DEBUG 
#define MY_RADIO_NRF24
#define MY_NODE_ID 18
#define MY_REPEATER_FEATURE
#include <MySensor.h>
#include <SPI.h>

#define CHILD_ID_LIGHT 1

#define LIGHT_PIN A0

MyMessage msgLight(CHILD_ID_LIGHT, V_LIGHT_LEVEL);  // 23

const unsigned long tUpdate=60000; // update interval
unsigned long t0;

void presentation() {
  sendSketchInfo("Light Sensor", "1.0");
  present(CHILD_ID_LIGHT, S_LIGHT_LEVEL);  
}

void setup() {
  t0=millis();
}

void loop() {
  if ((millis()-t0) > tUpdate) ServerUpdate();
}

void ServerUpdate() {
  send(msgLight.set(f_KY018(),0));
  t0=millis();
}

float f_KY018() {
       const float RES_0 = 10.0; // R in Kohm sensor
       int t_raw = 0;
       float volts = 0;
       float lux = 0;
 
       t_raw = 1023 - analogRead(LIGHT_PIN);
       volts = t_raw * 0.0048828125;
       lux = 500.0 / (RES_0 * ((5.0 - volts) / volts));
       return(lux);
 } 
