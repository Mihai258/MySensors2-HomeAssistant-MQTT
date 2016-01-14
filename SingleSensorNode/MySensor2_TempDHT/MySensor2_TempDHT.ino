// temperature sensor using DHT 11 or DHT22
#define MY_DEBUG 
#define MY_RADIO_NRF24
#define MY_NODE_ID 15
#define MY_REPEATER_FEATURE
#include <MySensor.h>
#include <SPI.h>
#include <DHT.h>  

#define CHILD_ID_HUM 1
#define CHILD_ID_TEMP 2
#define HUMIDITY_SENSOR_DIGITAL_PIN 3

MyMessage msgHum(CHILD_ID_HUM, V_HUM); // 1
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP); // 0

DHT dht;

float Temp, Hum;
const unsigned long tUpdate=60000; // update interval
unsigned long t0;

void presentation()  {
  sendSketchInfo("TempDHT", "1.0");
  present(CHILD_ID_HUM, S_HUM);
  present(CHILD_ID_TEMP, S_TEMP);  
}

void setup() {
  dht.setup(HUMIDITY_SENSOR_DIGITAL_PIN); 
}

void loop() {
  if ((millis()-t0) > tUpdate) ServerUpdate();
}

void ServerUpdate() {
  Hum = dht.getHumidity();
  Temp = dht.getTemperature();
  send(msgTemp.set(Temp, 1));
  send(msgHum.set(Hum, 1));
  t0=millis();
}

