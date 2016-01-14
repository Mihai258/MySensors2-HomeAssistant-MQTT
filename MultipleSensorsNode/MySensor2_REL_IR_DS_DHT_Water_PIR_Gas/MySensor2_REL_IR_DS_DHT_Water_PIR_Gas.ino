// Multisensor Node built with Arduino NANO + NRF24+
// Relay(4) + IR receiver (HX1838 kit)+ DS18B20(3) + Water Leak + PIR + MQ2
#define MY_DEBUG 
#define MY_RADIO_NRF24
#define MY_NODE_ID 20
#define MY_REPEATER_FEATURE
#include <SPI.h>
#include <MySensor.h>
#include <IRremote.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <DHT.h>  

#define CHILD_ID_REL1 1
#define CHILD_ID_REL2 2
#define CHILD_ID_REL3 3
#define CHILD_ID_REL4 4
#define CHILD_ID_WATER_LEAK 11
#define CHILD_ID_PIR 12
#define CHILD_ID_GAS 13
#define CHILD_ID_T1 21
#define CHILD_ID_T2 22
#define CHILD_ID_T3 23
#define CHILD_ID_HUM 31
#define CHILD_ID_TEMP 32

/*
PINs for the node built with Arduino NANO:
D0  - Free; Rx; may be used if do not use Serial
D1  - Free; Tx; may be used if do not use Serial
D2  - Free; INT: available
D3  - Free; INT, PWM; if not used for inclusion mode
D4  - Free; if not used for ERR LED
D5  - Free; PWM; if not used for Tx LED
D6  - Free; PWM; if not used for Rx LED
D7  - Free; if not used for signing
D8  - Free; 
D9  - Not free; PWM - NRF24 CE
D10 - Not free; PWM - NRF24 CS
D11 - Not free; PWM - NRF24 MOSI
D12 - Not free; NRF24 MISO
D13 - Not free; internal LED - NRF24 SCK
A0  - Free; 
A1  - Free; 
A2  - Free; 
A3  - Free; if not used for ATSHA204
A4  - Free; 
A5  - Free; 
A6  - Free; only analog: available
A7  - Free; only analog: available
*/

#define REL1_PIN 8
#define REL2_PIN 7
#define REL3_PIN 6
#define REL4_PIN 5
#define IR_PIN 4
#define WATER_LEAK_PIN A0
#define ONE_WIRE_BUS A1
#define PIR_PIN A2
#define HUMIDITY_SENSOR_DIGITAL_PIN A3
#define MQ_PIN A7

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

DeviceAddress addrTemp1 = { 0x28, 0xFF, 0xF1, 0x55, 0xA2, 0x15, 0x04, 0xB1 };
DeviceAddress addrTemp2 = { 0x28, 0xFF, 0xFB, 0x55, 0xA2, 0x15, 0x04, 0xB2 };
DeviceAddress addrTemp3 = { 0x28, 0xFF, 0xF1, 0x55, 0xA2, 0x15, 0x04, 0xB3 };
DHT dht;

bool StateREL=0, StateREL1=0, StateREL2=0, StateREL3=0, StateREL4=0;
const unsigned long tUpdate=30000; // update interval
unsigned long t0;
byte IrAct;
IRrecv irrecv(IR_PIN);
decode_results results;
float Temp1, Temp2, Temp3;
float Temp, Hum;
byte StateWATER=0;
byte oldStateWATER=0;
byte StatePIR=0;
byte oldStatePIR=0;
int InAnalog1;
int valMQ=0;
int refMQ=1023; // refference value will be the lowest value read from the sensor

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
  
  pinMode(IR_PIN, INPUT);
  irrecv.enableIRIn();
  
  pinMode(WATER_LEAK_PIN, INPUT_PULLUP);
  pinMode(PIR_PIN, INPUT_PULLUP);

  dht.setup(HUMIDITY_SENSOR_DIGITAL_PIN);
  
  t0=millis();
}

void loop() {
  IRr();

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

void IRr(){
  unsigned long IC1=0;
  if (irrecv.decode(&results)) {
    IC1 = results.value;
    switch (IC1) {
      case 16724175: IrAct=1; break; // 1
      case 16718055: IrAct=2; break; // 2
      case 16743045: IrAct=3; break; // 3
      case 16716015: IrAct=4; break; // 4
      case 16769055: IrAct=10; break; // -
      case 16754775: IrAct=20; break; // +
      default:IrAct=0; break;
      }
  irrecv.resume();

  switch(IrAct) {
      case 1: {
        StateREL1=!StateREL1;
        if (StateREL1) {
          digitalWrite(REL1_PIN, HIGH);
        }
        else {
          digitalWrite(REL1_PIN, LOW);
        }
        send(msgREL1.set(StateREL1 ? "ON":"OFF"));
      }
      break;
      case 2: {
        StateREL2=!StateREL2;
        if (StateREL2) {
          digitalWrite(REL2_PIN, HIGH);
        }
        else {
          digitalWrite(REL2_PIN, LOW);
        }
        send(msgREL2.set(StateREL2 ? "ON":"OFF"));
      }
      break;
      case 3: {
        StateREL3=!StateREL3;
        if (StateREL3) {
          digitalWrite(REL3_PIN, HIGH);
        }
        else {
          digitalWrite(REL3_PIN, LOW);
        }
        send(msgREL3.set(StateREL3 ? "ON":"OFF"));
      }
      break;
      case 4: {
        StateREL4=!StateREL4;
        if (StateREL4) {
          digitalWrite(REL4_PIN, HIGH);
        }
        else {
          digitalWrite(REL4_PIN, LOW);
        }
        send(msgREL4.set(StateREL4 ? "ON":"OFF"));
      }
      break;
      case 10: {
        StateREL1=0;
        StateREL2=0;
        StateREL3=0;
        StateREL4=0;
        digitalWrite(REL1_PIN, LOW);
        digitalWrite(REL2_PIN, LOW);
        digitalWrite(REL3_PIN, LOW);
        digitalWrite(REL4_PIN, LOW);
        send(msgREL1.set("OFF"));
        send(msgREL2.set("OFF"));
        send(msgREL3.set("OFF"));
        send(msgREL4.set("OFF"));
      }
      break;
      case 20: {
        StateREL1=1;
        StateREL2=1;
        StateREL3=1;
        StateREL4=1;
        digitalWrite(REL1_PIN, HIGH);
        digitalWrite(REL2_PIN, HIGH);
        digitalWrite(REL3_PIN, HIGH);
        digitalWrite(REL4_PIN, HIGH);
        send(msgREL1.set("ON"));
        send(msgREL2.set("ON"));
        send(msgREL3.set("ON"));
        send(msgREL4.set("ON"));
      }
      break;
    }
  }
}



/* IR Remote codes HEX:
 *  16753245  FFA25D  CH-
 *  16736925  FF629D  CH
 *  16769565  FFE21D  CH+
 *  16720605  FF22DD  <<
 *  16712445  FF02FD  >>
 *  16761405  FFC23D  >|
 *  16769055  FFE01F  -
 *  16754775  FFA857  +
 *  16748655  FF906F  EQ
 *  16738455  FF6897  0
 *  16750695  FF9867  100+
 *  16756815  FFB04F  200+
 *  16724175  FF30CF  1
 *  16718055  FF18E7  2
 *  16743045  FF7A85  3
 *  16716015  FF10EF  4
 *  16726215  FF38C7  5
 *  16734885  FF5AA5  6
 *  16728765  FF42BD  7
 *  16730805  FF4AB5  8
 *  16732845  FF52AD  9
 */
 
