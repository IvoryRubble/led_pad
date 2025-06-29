#include <math.h>
#include "mapf.h"
#include "color.h"

const int ledPin = 12;
const int ledR1Pin = 3;
const int ledG1Pin = 5;
const int ledB1Pin = 6;
const int ledR2Pin = 9;
const int ledG2Pin = 10;
const int ledB2Pin = 11;
const int buttonPin = 2;

const int effectsCount = 2;
int currentEffect = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  pinMode(ledR1Pin, OUTPUT);
  digitalWrite(ledR1Pin, HIGH);
  pinMode(ledG1Pin, OUTPUT);
  digitalWrite(ledG1Pin, HIGH);
  pinMode(ledB1Pin, OUTPUT);
  digitalWrite(ledB1Pin, HIGH);
  pinMode(ledR2Pin, OUTPUT);
  digitalWrite(ledR2Pin, HIGH);
  pinMode(ledG2Pin, OUTPUT);
  digitalWrite(ledG2Pin, HIGH);
  pinMode(ledB2Pin, OUTPUT);
  digitalWrite(ledB2Pin, HIGH);

  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {

  bool isButtonPressed = readButton();
  bool isSerialCommandRecieved = readSerial();

  if (isButtonPressed || isSerialCommandRecieved) {
    writeLed(true);
    delay(200);
    writeLed(false);
    delay(100);
    currentEffect = (currentEffect + 1) % effectsCount;
    Serial.print("currentEffect = "); 
    Serial.println(currentEffect);
  }

  switch (currentEffect) {
    case 0:
      effect0();
      break;
    case 1:
      effect1();
      break;
  }
}

bool readSerial() {
  bool isSerialCommandRecieved = false;
  while (Serial.available()) {
    Serial.read();
    isSerialCommandRecieved = true;
  }
  return isSerialCommandRecieved;
}

bool readButton() {
  bool isButtonPressed = !digitalRead(buttonPin);
  if (isButtonPressed) {
    delay(250);
  }
  return isButtonPressed;
}

void writeLed(bool isLedOn) {
  digitalWrite(ledPin, isLedOn);
}

void writeLed1(Color c) {
  analogWrite(ledR1Pin, 255 - c.r);
  analogWrite(ledG1Pin, 255 - c.g);
  analogWrite(ledB1Pin, 255 - c.b);
}

void writeLed2(Color c) {
  analogWrite(ledR2Pin, 255 - c.r);
  analogWrite(ledG2Pin, 255 - c.g);
  analogWrite(ledB2Pin, 255 - c.b);
}

void effect0() {
  const unsigned long effectDuration = 5000;
  unsigned long currentTime = millis() % effectDuration;

  float led1Hue = mapf(currentTime, 0, effectDuration, 0, 1);
  float led2Hue = led1Hue;
  Color led1Value = hsvToRgb(led1Hue, 1, 1);
  Color led2Value = hsvToRgb(led2Hue, 1, 1);

  writeLed1(led1Value);
  writeLed2(led2Value);
}

void effect1() {
  const unsigned long effectDuration = 5000;
  unsigned long currentTime = millis() % effectDuration;

  float led1Hue = mapf(currentTime, 0, effectDuration, 0, 1);
  float led2Hue = led1Hue + 0.5;
  led2Hue = led2Hue - floor(led2Hue);
  Color led1Value = hsvToRgb(led1Hue, 1, 1);
  Color led2Value = hsvToRgb(led2Hue, 1, 1);

  writeLed1(led1Value);
  writeLed2(led2Value);
}
