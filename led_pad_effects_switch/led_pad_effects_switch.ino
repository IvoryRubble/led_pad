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

const int customColorsCount = 6;
Color customColors[customColorsCount] = {
  {r: 255, g: 100, b: 40},
  {r: 150, g: 255, b: 50},
  {r: 210, g: 200, b: 10},
  {r: 200, g: 100, b: 0},
  {r: 255, g: 200, b: 200},
  {r: 230, g: 50, b: 255}
};

const int effectsCount = customColorsCount + 2;
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

  if (currentEffect >= 0 && currentEffect < customColorsCount) {
    writeLed1(customColors[currentEffect]);
    writeLed2(customColors[currentEffect]);
  }

  switch (currentEffect) {
    case customColorsCount:
      effect0();
      break;
    case customColorsCount + 1:
      effect1();
      break;
  }
}

void setColorFromSerial() {
  if (Serial.available()) {
    int r = Serial.parseInt();
    int g = Serial.parseInt();
    int b = Serial.parseInt();
    Serial.parseInt();
    Serial.print("writeLed1({r: "); Serial.print(r); Serial.print(", g: "); Serial.print(g); Serial.print(", b: "); Serial.print(b); Serial.println("});");
    writeLed1({r: r, g: g, b: b});
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
