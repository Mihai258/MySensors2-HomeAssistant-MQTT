// get status from several digital inputs and send them to the gateway
#define MY_DEBUG 
#define MY_RADIO_NRF24
#define MY_NODE_ID 11
#define MY_REPEATER_FEATURE
#include <MySensor.h>
#include <SPI.h>

#define CHILD_ID_PIR 1
#define CHILD_ID_BINARY 2
#define CHILD_ID_WATER_LEAK 3
#define CHILD_ID_SOUND 4
#define CHILD_ID_VIBRATION 5

#define PIR_PIN 2
#define BINARY_PIN 3
#define WATER_LEAK_PIN 4
#define SOUND_PIN 5
#define VIBRATION_PIN 6

MyMessage msgPir(CHILD_ID_PIR, V_TRIPPED); // 16
MyMessage msgBin(CHILD_ID_BINARY, V_STATUS); // 2
MyMessage msgWater(CHILD_ID_WATER_LEAK, V_TRIPPED); // 16
MyMessage msgSnd(CHILD_ID_SOUND, V_TRIPPED); // 16
MyMessage msgVib(CHILD_ID_VIBRATION, V_TRIPPED); // 16

byte StatePIR=0;
byte oldStatePIR=0;
byte StateBINARY=0;
byte oldStateBINARY=0;
byte StateWATER=0;
byte oldStateWATER=0;
byte StateSOUND=0;
byte oldStateSOUND=0;
byte StateVIBRATION=0;
byte oldStateVIBRATION=0;

const unsigned long tUpdate=30000; // update the digital input status at this interval
unsigned long t0;

void presentation() {
  sendSketchInfo("DigitalMultiSensor", "1.0");
  present(CHILD_ID_PIR, S_MOTION);
  present(CHILD_ID_BINARY, S_MOTION);
  present(CHILD_ID_WATER_LEAK, S_MOTION);
  present(CHILD_ID_SOUND, S_MOTION);
  present(CHILD_ID_VIBRATION, S_MOTION);
}

void setup() {
  pinMode(PIR_PIN, INPUT_PULLUP);
  pinMode(BINARY_PIN, INPUT_PULLUP);
  pinMode(WATER_LEAK_PIN, INPUT_PULLUP);
  pinMode(SOUND_PIN, INPUT_PULLUP);
  pinMode(VIBRATION_PIN, INPUT_PULLUP);
 
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
  
  StateWATER=digitalRead(WATER_LEAK_PIN);
  if (StateWATER != oldStateWATER) {
    oldStateWATER=StateWATER;
    send(msgWater.set(StateWATER ? "ON" : "OFF"));
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
  send(msgPir.set(StatePIR ? "ON" : "OFF"));
  send(msgBin.set(StateBINARY ? "ON" : "OFF"));
  send(msgWater.set(StateWATER ? "ON" : "OFF"));
  send(msgSnd.set(StateSOUND ? "ON" : "OFF"));
  send(msgVib.set(StateVIBRATION ? "ON" : "OFF"));
  
  t0=millis();
}

