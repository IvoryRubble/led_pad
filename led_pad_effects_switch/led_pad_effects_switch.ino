#include <SoftPWM.h>
#include <math.h>
#include "mapf.h"
#include "color.h"

const int ledPin = 12;
const int ledR1Pin = 3;
const int ledG1Pin = 4;
const int ledB1Pin = 5;
const int ledR2Pin = 8;
const int ledG2Pin = 9;
const int ledB2Pin = 10;
const int buttonPin = 2;

const int defaultColorsCount = 6;
ColorHSV defaultColorsHSV[defaultColorsCount] = {
  { h: 0, s: 100, v: 100 },
  { h: 120, s: 100, v: 100 },
  { h: 240, s: 100, v: 100 },
  { h: 60, s: 100, v: 100 },
  { h: 300, s: 100, v: 100 },
  { h: 180, s: 100, v: 100 }
};

const int customColorsCount = 5;
ColorHSV customColorsHSV[customColorsCount] = {
  { h: 17, s: 84, v: 100 },
  { h: 90, s: 80, v: 100 },
  { h: 57, s: 95, v: 100 },
  { h: 0, s: 22, v: 100 },
  { h: 293, s: 80, v: 100 }
};

const int effectsCount = defaultColorsCount + customColorsCount + 10;
int currentEffect = defaultColorsCount + customColorsCount + 9;

void setup() {
  Serial.begin(115200);
  delay(2000);

  SoftPWMBegin(SOFTPWM_INVERTED);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  SoftPWMSet(ledR1Pin, 0);
  SoftPWMSet(ledG1Pin, 0);
  SoftPWMSet(ledB1Pin, 0);
  SoftPWMSet(ledR2Pin, 0);
  SoftPWMSet(ledG2Pin, 0);
  SoftPWMSet(ledB2Pin, 0);

  pinMode(buttonPin, INPUT_PULLUP);

  writeLed1({ r: 255, g: 0, b: 0 });
  writeLed2({ r: 255, g: 0, b: 0 });
  delay(500);
  writeLed1({ r: 0, g: 255, b: 0 });
  writeLed2({ r: 0, g: 255, b: 0 });
  delay(500);
  writeLed1({ r: 0, g: 0, b: 255 });
  writeLed2({ r: 0, g: 0, b: 255 });
  delay(500);
  writeLed1({ r: 255, g: 255, b: 255 });
  writeLed2({ r: 255, g: 255, b: 255 });
  delay(500);
  writeLed1({ r: 0, g: 0, b: 0 });
  writeLed2({ r: 0, g: 0, b: 0 });
  delay(500);
}

void loop() {
  bool isButtonPressed = readButton();
  bool isSerialCommandRecieved = readSerial();

  if (isButtonPressed || isSerialCommandRecieved) {
    delay(100);
    writeLed(true);
    writeLed1({ r: 255, g: 255, b: 255 });
    writeLed2({ r: 255, g: 255, b: 255 });
    delay(5);
    writeLed(false);
    writeLed1({ r: 0, g: 0, b: 0 });
    writeLed2({ r: 0, g: 0, b: 0 });
    delay(100);
    currentEffect = (currentEffect + 1) % effectsCount;
    Serial.print("currentEffect = ");
    Serial.println(currentEffect);
  }

  if (currentEffect >= 0 && currentEffect < defaultColorsCount) {
    constColorBreathEffect(defaultColorsHSV[currentEffect]);
  }

  if (currentEffect >= defaultColorsCount && currentEffect < customColorsCount + defaultColorsCount) {
    constColorBreathEffect(customColorsHSV[currentEffect - defaultColorsCount]);
  }

  switch (currentEffect) {
    case defaultColorsCount + customColorsCount:
      rainbowEffect(0.7);
      break;
    case defaultColorsCount + customColorsCount + 1:
      rainbowEffect(1);
      break;
    case defaultColorsCount + customColorsCount + 2:
      rainbowBreathEffect(0.7);
      break;
    case defaultColorsCount + customColorsCount + 3:
      rainbowBreathEffect(1);
      break;
    case defaultColorsCount + customColorsCount + 4:
      rainbowTwoColorsEffect(0.2, 1);
      break;
    case defaultColorsCount + customColorsCount + 5:
      rainbowTwoColorsEffect(0.5, 1);
      break;
    case defaultColorsCount + customColorsCount + 6:
      rainbowBreathTwoColorsEffect(0.2, 1);
      break;
    case defaultColorsCount + customColorsCount + 7:
      rainbowBreathTwoColorsEffect(0.5, 1);
      break;
    case defaultColorsCount + customColorsCount + 8:
      rainbowTwoColorsAperiodicEffect(1);
      break;
    case defaultColorsCount + customColorsCount + 9:
      rainbowBreathTwoColorsAperiodicEffect(1);
      break;
  }

  delay(5);
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
  SoftPWMSet(ledR1Pin, c.r);
  SoftPWMSet(ledG1Pin, c.g);
  SoftPWMSet(ledB1Pin, c.b);
}

void writeLed2(Color c) {
  SoftPWMSet(ledR2Pin, c.r);
  SoftPWMSet(ledG2Pin, c.g);
  SoftPWMSet(ledB2Pin, c.b);
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

void constColorEffect(ColorHSV c) {
  Color ledValue = hsvToRgb(c.hF(), c.sF(), c.vF());
  writeLed1(ledValue);
  writeLed2(ledValue);
}

void constColorBreathEffect(ColorHSV c) {
  float effectValue = breathEffect(millis(), 8000, 0, 2, 0.1 * c.vF(), c.vF());
  Color ledValue = hsvToRgb(c.hF(), c.sF(), effectValue);
  writeLed1(ledValue);
  writeLed2(ledValue);
}

void rainbowEffect(float saturation) {
  float ledHue = rainbowEffect(millis(), 60000, 0);
  Color ledValue = hsvToRgb(ledHue, saturation, 1);
  writeLed1(ledValue);
  writeLed2(ledValue);
}

void rainbowTwoColorsEffect(float effectOffset, float saturation) {
  float led1Hue = rainbowEffect(millis(), 60000, 0);
  float led2Hue = rainbowEffect(millis(), 60000, effectOffset);
  Color led1Value = hsvToRgb(led1Hue, saturation, 1);
  Color led2Value = hsvToRgb(led2Hue, saturation, 1);
  writeLed1(led1Value);
  writeLed2(led2Value);
}

void rainbowTwoColorsAperiodicEffect(float saturation) {
  float led1Hue = rainbowEffect(millis(), 60000, 0);
  float led2Hue = rainbowEffect(millis(), 53000, 0);
  Color led1Value = hsvToRgb(led1Hue, saturation, 1);
  Color led2Value = hsvToRgb(led2Hue, saturation, 1);
  writeLed1(led1Value);
  writeLed2(led2Value);
}

void rainbowBreathEffect(float saturation) {
  float ledHue = rainbowEffect(millis(), 60000, 0);
  float effectValue = breathEffect(millis(), 8000, 0, 2, 0.1, 1);
  Color ledValue = hsvToRgb(ledHue, saturation, effectValue);
  writeLed1(ledValue);
  writeLed2(ledValue);
}

void rainbowBreathTwoColorsEffect(float effectOffset, float saturation) {
  float led1Hue = rainbowEffect(millis(), 60000, 0);
  float led2Hue = rainbowEffect(millis(), 60000, effectOffset);
  float effectValue = breathEffect(millis(), 8000, 0, 2, 0.1, 1);
  Color led1Value = hsvToRgb(led1Hue, saturation, effectValue);
  Color led2Value = hsvToRgb(led2Hue, saturation, effectValue);
  writeLed1(led1Value);
  writeLed2(led2Value);
}

void rainbowBreathTwoColorsAperiodicEffect(float saturation) {
  float led1Hue = rainbowEffect(millis(), 60000, 0);
  float led2Hue = rainbowEffect(millis(), 53000, 0);
  float effect1Value = breathEffect(millis(), 8000, 0, 2, 0.1, 1);
  float effect2Value = breathEffect(millis(), 7900, 0, 2, 0.1, 1);
  Color led1Value = hsvToRgb(led1Hue, saturation, effect1Value);
  Color led2Value = hsvToRgb(led2Hue, saturation, effect2Value);
  writeLed1(led1Value);
  writeLed2(led2Value);
}
