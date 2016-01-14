// temperature sensor with DS18B20 (3)
#define MY_DEBUG 
#define MY_RADIO_NRF24
#define MY_NODE_ID 13
#define MY_REPEATER_FEATURE
#include <MySensor.h>
#include <SPI.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#define CHILD_ID_T1 1
#define CHILD_ID_T2 2
#define CHILD_ID_T3 3

#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 12
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

MyMessage msgT1(CHILD_ID_T1, V_TEMP); // 0
MyMessage msgT2(CHILD_ID_T2, V_TEMP); // 0
MyMessage msgT3(CHILD_ID_T3, V_TEMP); // 0
// addresses must be updated according to your sensors!
DeviceAddress addrTemp1 = { 0x28, 0xFE, 0xF1, 0x55, 0xA2, 0x15, 0x04, 0x01 };
DeviceAddress addrTemp2 = { 0x28, 0xFE, 0xF1, 0x55, 0xA2, 0x15, 0x04, 0x02 };
DeviceAddress addrTemp3 = { 0x28, 0xFE, 0xF1, 0x55, 0xA2, 0x15, 0x04, 0x03 };

float Temp1, Temp2, Temp3;
const unsigned long tUpdate=60000; // update interval
unsigned long t0;

void presentation()  {
  sendSketchInfo("Temp", "1.0");
  present(CHILD_ID_T1, S_TEMP);
  present(CHILD_ID_T2, S_TEMP);
  present(CHILD_ID_T3, S_TEMP);  
}

void setup() {
  sensors.begin();
  sensors.setResolution(addrTemp1, TEMPERATURE_PRECISION);
  sensors.setResolution(addrTemp2, TEMPERATURE_PRECISION);
  sensors.setResolution(addrTemp3, TEMPERATURE_PRECISION);  
}

void loop() {
  if ((millis()-t0) > tUpdate) ServerUpdate();
}

void ServerUpdate() {
  sensors.requestTemperatures();
  Temp1 = sensors.getTempC(addrTemp1);
  Temp2 = sensors.getTempC(addrTemp2);  
  Temp3 = sensors.getTempC(addrTemp3); 
  
  send(msgT1.set(Temp1,1));
  send(msgT2.set(Temp2,1));
  send(msgT3.set(Temp3,1));
  
  t0=millis();
}

