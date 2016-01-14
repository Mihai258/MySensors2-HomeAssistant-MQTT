// Relay(4) + IR receiver (HX1838 kit)+ DS18B20(3) module + Water Leak + PIR + MQ2
#define MY_DEBUG 
#define MY_RADIO_NRF24
#define MY_NODE_ID 10
#define MY_REPEATER_FEATURE
#include <SPI.h>
#include <MySensor.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <DHT.h>  

#define CHILD_ID_REL1 1
#define CHILD_ID_REL2 2
#define CHILD_ID_REL3 3
#define CHILD_ID_REL4 4
#define CHILD_ID_WATER_LEAK 5
#define CHILD_ID_PIR 6
#define CHILD_ID_GAS 7
#define CHILD_ID_T1 11
#define CHILD_ID_T2 12
#define CHILD_ID_T3 13
#define CHILD_ID_HUM 21
#define CHILD_ID_TEMP 22

#define REL1_PIN 8
#define REL2_PIN 7
#define REL3_PIN 6
#define REL4_PIN 5
#define WATER_LEAK_PIN 3
#define ONE_WIRE_BUS 2
#define PIR_PIN A1
#define MQ_PIN A0
#define HUMIDITY_SENSOR_DIGITAL_PIN A2

#define TEMPERATURE_PRECISION 12
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

MyMessage msgREL1(CHILD_ID_REL1, V_LIGHT);  // 2
MyMessage msgREL2(CHILD_ID_REL2, V_LIGHT);
MyMessage msgREL3(CHILD_ID_REL3, V_LIGHT);
MyMessage msgREL4(CHILD_ID_REL4, V_LIGHT);
MyMessage msgT1(CHILD_ID_T1, V_TEMP); // 0
MyMessage msgT2(CHILD_ID_T2, V_TEMP); // 0
MyMessage msgT3(CHILD_ID_T3, V_TEMP); // 0
MyMessage msgWater(CHILD_ID_WATER_LEAK, V_TRIPPED);  // 16
MyMessage msgPir(CHILD_ID_PIR, V_TRIPPED);
MyMessage msgGas(CHILD_ID_GAS, V_VAR1);  // 24 .. 28
MyMessage msgHum(CHILD_ID_HUM, V_HUM); // 1
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP); // 0

DeviceAddress addrTemp1 = { 0x28, 0xFF, 0x57, 0x98, 0x81, 0x15, 0x02, 0x17 };
DeviceAddress addrTemp2 = { 0x28, 0xFF, 0xF1, 0x55, 0xA2, 0x15, 0x04, 0xB1 };
DeviceAddress addrTemp3 = { 0x28, 0xFF, 0xF1, 0x55, 0xA2, 0x15, 0x04, 0xB2 };
DHT dht;

bool StateREL=0, StateREL1=0, StateREL2=0, StateREL3=0, StateREL4=0;
const unsigned long tUpdate=30000; // update interval
unsigned long t0;
float Temp1, Temp2, Temp3;
float Temp, Hum;
byte StateWATER=0;
byte oldStateWATER=0;
byte StatePIR=0;
byte oldStatePIR=0;
int InAnalog1;
int valMQ=0;
int refMQ=1023; // refference value will be the lowest value read from the sensor
unsigned long tSetup=30000; // warming time

void presentation()  {
  sendSketchInfo("Multi", "1.0");
  present(CHILD_ID_REL1, S_LIGHT);
  present(CHILD_ID_REL2, S_LIGHT);
  present(CHILD_ID_REL3, S_LIGHT);
  present(CHILD_ID_REL4, S_LIGHT);
  present(CHILD_ID_T1, S_TEMP);
  present(CHILD_ID_T2, S_TEMP);
  present(CHILD_ID_T3, S_TEMP);
  present(CHILD_ID_PIR, S_MOTION);
  present(CHILD_ID_GAS, S_CUSTOM);
  present(CHILD_ID_WATER_LEAK, S_MOTION);
  present(CHILD_ID_HUM, S_HUM);
  present(CHILD_ID_TEMP, S_TEMP);  
}

void setup() {
  sensors.begin();
  sensors.setResolution(addrTemp1, TEMPERATURE_PRECISION);
  sensors.setResolution(addrTemp2, TEMPERATURE_PRECISION);
  sensors.setResolution(addrTemp3, TEMPERATURE_PRECISION);
  
  pinMode(REL1_PIN, OUTPUT);
  digitalWrite(REL1_PIN, StateREL1);
  pinMode(REL2_PIN, OUTPUT);
  digitalWrite(REL2_PIN, StateREL2);
  pinMode(REL3_PIN, OUTPUT);
  digitalWrite(REL3_PIN, StateREL3);
  pinMode(REL4_PIN, OUTPUT);
  digitalWrite(REL4_PIN, StateREL4);
  
  pinMode(WATER_LEAK_PIN, INPUT_PULLUP);
  pinMode(PIR_PIN, INPUT_PULLUP);

  dht.setup(HUMIDITY_SENSOR_DIGITAL_PIN);
  
  t0=millis();
}

void loop() {
  StatePIR=digitalRead(PIR_PIN);
  if (StatePIR != oldStatePIR) {
    oldStatePIR=StatePIR;
    send(msgPir.set(StatePIR ? "ON" : "OFF"));
  }

  StateWATER=digitalRead(WATER_LEAK_PIN);
  if (StateWATER != oldStateWATER) {
    oldStateWATER=StateWATER;
    send(msgWater.set(StateWATER ? "ON" : "OFF"));
  }
    
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
  
  send(msgPir.set(StatePIR ? "ON" : "OFF"));
  send(msgWater.set(StateWATER ? "ON" : "OFF"));

  InAnalog1=analogRead(MQ_PIN);
  if (InAnalog1<refMQ) refMQ=InAnalog1; // if read value is lower that actual, change the 0 refference with the actual one
  valMQ=InAnalog1-refMQ;
  send(msgGas.set(valMQ));

  Hum = dht.getHumidity();
  Temp = dht.getTemperature();
  send(msgTemp.set(Temp, 1));
  send(msgHum.set(Hum, 1));

  t0=millis();
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
