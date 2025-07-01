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

const int defaultColorsCount = 6;
Color defaultColors[defaultColorsCount] = {
  { r: 255, g: 0, b: 0 },
  { r: 0, g: 255, b: 0 },
  { r: 0, g: 0, b: 255 },
  { r: 255, g: 255, b: 0 },
  { r: 255, g: 0, b: 255 },
  { r: 0, g: 255, b: 255 }
};

const int customColorsCount = 6;
Color customColors[customColorsCount] = {
  { r: 255, g: 100, b: 40 },
  { r: 150, g: 255, b: 50 },
  { r: 210, g: 200, b: 10 },
  { r: 200, g: 100, b: 0 },
  { r: 255, g: 200, b: 200 },
  { r: 230, g: 50, b: 255 }
};

const int effectsCount = defaultColorsCount + customColorsCount + 2;
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
    delay(100);
    writeLed(true);
    writeLed1({ r: 255, g: 255, b: 255 });
    writeLed2({ r: 255, g: 255, b: 255 });
    delay(100);
    writeLed(false);
    writeLed1({ r: 0, g: 0, b: 0 });
    writeLed2({ r: 0, g: 0, b: 0 });
    delay(100);
    currentEffect = (currentEffect + 1) % effectsCount;
    Serial.print("currentEffect = ");
    Serial.println(currentEffect);
  }
  
  if (currentEffect >= 0 && currentEffect < defaultColorsCount) {
    writeLed1(defaultColors[currentEffect]);
    writeLed2(defaultColors[currentEffect]);
  }

  if (currentEffect >= defaultColorsCount && currentEffect < customColorsCount + defaultColorsCount) {
    writeLed1(customColors[currentEffect - defaultColorsCount]);
    writeLed2(customColors[currentEffect - defaultColorsCount]);
  }

  switch (currentEffect) {
    case defaultColorsCount + customColorsCount:
      effect0();
      break;
    case defaultColorsCount + customColorsCount + 1:
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
    Serial.print("writeLed1({r: ");
    Serial.print(r);
    Serial.print(", g: ");
    Serial.print(g);
    Serial.print(", b: ");
    Serial.print(b);
    Serial.println("});");
    writeLed1({ r: r, g: g, b: b });
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

float rainbowEffect(unsigned long currentTime, unsigned long effectDuration, float effectOffset) {
  currentTime = currentTime % effectDuration;
  float effectTime = effectDuration > 0 ? (float)currentTime / effectDuration : 0;
  effectTime = effectTime + effectOffset;
  effectTime = effectTime - floor(effectTime);
  
  float effectHue = effectTime;
  return effectHue;
}

float breathEffect(unsigned long currentTime, unsigned long effectDuration, float effectOffset, float breathDeep, float valueMin, float valueMax) {
  currentTime = currentTime % effectDuration;
  float effectTime = effectDuration > 0 ? (float)currentTime / effectDuration : 0;
  effectTime = effectTime + effectOffset;
  effectTime = effectTime - floor(effectTime);

  float effectValue = 1;
  for (int i = 0; i < breathDeep; i++) {
    effectValue = breathX(effectTime);
  }
  effectValue = mapf(effectValue, 0, 1, valueMin, valueMax);
  return effectValue;
}

float breathX(float input) {
  if (input < 0.5) {
    float output = 2 * input;  
    return output;
  } else {
    float output = 2 * (1 - input);
    return output;
  }
}

void breath0() {
  float effectValue = breathEffect(millis(), 8000, 0, 4, 0.1, 1);

  Color led1Value = hsvToRgb(0.25, 1, effectValue);
  Color led2Value = led1Value;
  
  writeLed1(led1Value);
  writeLed2(led2Value);
}

void effect0() {
  float led1Hue = rainbowEffect(millis(), 5000, 0);
  float led2Hue = led1Hue; 
  Color led1Value = hsvToRgb(led1Hue, 1, 1);
  Color led2Value = hsvToRgb(led2Hue, 1, 1);

  writeLed1(led1Value);
  writeLed2(led2Value);
}

void effect1() {
  float led1Hue = rainbowEffect(millis(), 5000, 0);
  float led2Hue = rainbowEffect(millis(), 5000, 0.5);
  Color led1Value = hsvToRgb(led1Hue, 1, 1);
  Color led2Value = hsvToRgb(led2Hue, 1, 1);

  writeLed1(led1Value);
  writeLed2(led2Value);
}
