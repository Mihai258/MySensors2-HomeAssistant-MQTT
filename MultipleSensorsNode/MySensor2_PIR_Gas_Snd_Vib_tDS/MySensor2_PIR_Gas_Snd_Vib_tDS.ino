// get status from several inputs and send them to the gateway
#define MY_DEBUG 
#define MY_RADIO_NRF24
#define MY_NODE_ID 19
#define MY_REPEATER_FEATURE
#include <MySensor.h>
#include <SPI.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#define CHILD_ID_PIR 1
#define CHILD_ID_BINARY 2
#define CHILD_ID_GAS 3
#define CHILD_ID_SOUND 4
#define CHILD_ID_VIBRATION 5
#define CHILD_ID_T1 10
#define CHILD_ID_T2 11
#define CHILD_ID_T3 12

#define PIR_PIN 2
#define BINARY_PIN 3
#define MQ_PIN A0
#define SOUND_PIN 5
#define VIBRATION_PIN 6
#define ONE_WIRE_BUS 4
#define TEMPERATURE_PRECISION 12
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

MyMessage msgPir(CHILD_ID_PIR, V_TRIPPED); // 16
MyMessage msgBin(CHILD_ID_BINARY, V_STATUS); // 2
MyMessage msgGas(CHILD_ID_GAS, V_VAR1);  // 24 .. 28
MyMessage msgSnd(CHILD_ID_SOUND, V_TRIPPED); // 16
MyMessage msgVib(CHILD_ID_VIBRATION, V_TRIPPED);  // 16
MyMessage msgT1(CHILD_ID_T1, V_TEMP);  // 0
MyMessage msgT2(CHILD_ID_T2, V_TEMP);  // 0
MyMessage msgT3(CHILD_ID_T3, V_TEMP);  // 0
// Addresses must be updated!
DeviceAddress addrTemp1 = { 0x28, 0xFF, 0x75, 0x19, 0xA2, 0x15, 0x04, 0x97 };
DeviceAddress addrTemp2 = { 0x28, 0xFF, 0xF1, 0x55, 0xA2, 0x15, 0x04, 0xB1 };
DeviceAddress addrTemp3 = { 0x28, 0xFF, 0xF1, 0x55, 0xA2, 0x15, 0x04, 0xB2 };

byte StatePIR=0;
byte oldStatePIR=0;
byte StateBINARY=0;
byte oldStateBINARY=0;
byte StateSOUND=0;
byte oldStateSOUND=0;
byte StateVIBRATION=0;
byte oldStateVIBRATION=0;
int InAnalog1;
int valMQ=0;
int refMQ=1023; // refference value will be the lowest value read from the sensor
float Temp1, Temp2, Temp3;
const unsigned long tUpdate=30000; // update the digital input status at this interval
unsigned long tSetup=30000; // warming time
unsigned long t0;

void presentation()  {
  sendSketchInfo("MultiSensor", "1.0");
  present(CHILD_ID_PIR, S_MOTION);
  present(CHILD_ID_BINARY, S_MOTION);
  present(CHILD_ID_GAS, S_CUSTOM);
  present(CHILD_ID_SOUND, S_MOTION);
  present(CHILD_ID_VIBRATION, S_MOTION);
  present(CHILD_ID_T1, S_TEMP);
  present(CHILD_ID_T2, S_TEMP);
  present(CHILD_ID_T3, S_TEMP);  
}


void setup() {
  pinMode(PIR_PIN, INPUT_PULLUP);
  pinMode(BINARY_PIN, INPUT_PULLUP);
  pinMode(SOUND_PIN, INPUT_PULLUP);
  pinMode(VIBRATION_PIN, INPUT_PULLUP);

  sensors.begin();
  sensors.setResolution(addrTemp1, TEMPERATURE_PRECISION);
  sensors.setResolution(addrTemp2, TEMPERATURE_PRECISION);
  sensors.setResolution(addrTemp3, TEMPERATURE_PRECISION);  
    
  t0=millis();
}

void loop() {
  // digital PIN status update if they change:
  StatePIR=digitalRead(PIR_PIN);
  if (StatePIR != oldStatePIR) {
    oldStatePIR=StatePIR;
    send(msgPir.set(StatePIR ? "ON" : "OFF"));
  }
  
  StateBINARY=digitalRead(BINARY_PIN);
  if (StateBINARY != oldStateBINARY) {
    oldStateBINARY=StateBINARY;
    send(msgBin.set(StateBINARY ? "ON" : "OFF"));
  }
  
  StateSOUND=digitalRead(SOUND_PIN);
  if (StateSOUND != oldStateSOUND) {
    oldStateSOUND=StateSOUND;
    send(msgSnd.set(StateSOUND ? "ON" : "OFF"));
  }
  
  StateVIBRATION=digitalRead(VIBRATION_PIN);
  if (StateVIBRATION != oldStateVIBRATION) {
    oldStateVIBRATION=StateVIBRATION;
    send(msgVib.set(StateVIBRATION ? "ON" : "OFF"));
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
  send(msgBin.set(StateBINARY ? "ON" : "OFF"));
  send(msgSnd.set(StateSOUND ? "ON" : "OFF"));
  send(msgVib.set(StateVIBRATION ? "ON" : "OFF"));

  InAnalog1=analogRead(MQ_PIN);
  if (InAnalog1<refMQ) refMQ=InAnalog1; // if read value is lower that actual, change the 0 refference with the actual one
  valMQ=InAnalog1-refMQ;
  send(msgGas.set(valMQ));
  
  t0=millis();
}

