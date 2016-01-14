// distance measurement with ultrasonic sensor HC-SR04, send distance to the gateway
#define MY_DEBUG 
#define MY_RADIO_NRF24
#define MY_NODE_ID 17
#define MY_REPEATER_FEATURE
#include <MySensor.h>
#include <SPI.h>

#define CHILD_ID_DIST 1

const byte trig = 3; // attach pin 3 to Trig
const byte echo = 4; //attach pin 4 to Echo

MyMessage msgDist(CHILD_ID_DIST, V_DISTANCE); // 13

const unsigned long tUpdate=1000; // update distance data time interval
unsigned long t0;
long duration, cm, inches;

void presentation() {
  sendSketchInfo("Distance", "1.0");
  present(CHILD_ID_DIST, S_DISTANCE);
}

void setup() {
  pinMode(trig, OUTPUT);
  pinMode(echo,INPUT);
  t0=millis();
}

void loop() {
  if ((millis()-t0) > tUpdate) ServerUpdate();
}

void ServerUpdate() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(5);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  //inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);
  if (cm>400) cm=399;
  send(msgDist.set(cm));
  t0=millis();
}

long microsecondsToInches(long microseconds)
{
return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds)
{
return microseconds / 29 / 2;
}

