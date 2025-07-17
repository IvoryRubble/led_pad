#include <EEPROM.h>
#include <math.h>
#include "mapf.h"
#include "color.h"
#include "isSerialPortOpened.h"
#include "buttonDebounce.h"

int currentEffectDataAddress = 8;

struct ColorData {
  ColorHSV color;
  unsigned long breathDuration;
  float breathDeep;
};

struct SerialResponse {
  bool changeEffect = false;
  bool showNotification = false;
  bool ledOn = false;
  bool ledOff = false;
  bool isCustomColor = false;
  ColorData customColor;
};

struct CurrentEffectData {
  int currentEffect;
  bool isCustomColor;
  ColorData customColor;
  int hash;

  void initHash() {
    hash = getHash();
  }
  bool isHashValid() {
    return getHash() == hash;
  }
  int getHash() {
    return 15 + currentEffect + isCustomColor + customColor.color.h + customColor.color.s + customColor.color.v;
  }
};

const int ledR1Pin = 5;
const int ledG1Pin = 9;
const int ledB1Pin = 6;
const int buttonPin = 8;

ButtonDebounce buttonDebounce(100, 1500);

const int defaultColorsCount = 8;
ColorData defaultColorsHSV[defaultColorsCount] = {
  { color: { h: 0, s: 0, v: 0 }, breathDuration: 0, breathDeep: 0 },
  { color: { h: 0, s: 100, v: 100 }, breathDuration: 8000, breathDeep: 0.6 },
  { color: { h: 120, s: 100, v: 100 }, breathDuration: 8000, breathDeep: 0.5 },
  { color: { h: 240, s: 100, v: 100 }, breathDuration: 8000, breathDeep: 0.5 },
  { color: { h: 60, s: 100, v: 100 }, breathDuration: 8000, breathDeep: 0.4 },
  { color: { h: 300, s: 100, v: 100 }, breathDuration: 8000, breathDeep: 0.4 },
  { color: { h: 180, s: 100, v: 100 }, breathDuration: 4000, breathDeep: 1.0 },
  { color: { h: 0, s: 0, v: 100 }, breathDuration: 0, breathDeep: 0 }
};

const int customColorsCount = 1;
ColorData customColorsHSV[customColorsCount] = {
  { color: { h: 348, s: 100, v: 100 }, breathDuration: 8000, breathDeep: 0.25 }
};

const int effectsCount = defaultColorsCount + customColorsCount + 2;
int currentEffect = 0;

bool isCustomColor = false;
ColorData customColor;

bool isLedOn = true;

void setup() {
  struct CurrentEffectData currentEffectData;
  EEPROM.get(currentEffectDataAddress, currentEffectData);
  if (currentEffectData.isHashValid()) {
    currentEffect = currentEffectData.currentEffect % effectsCount;
    isCustomColor = currentEffectData.isCustomColor;
    customColor = currentEffectData.customColor;
  }

  Serial.begin(115200);
  delay(2000);
  Serial.println("Begin...");
  Serial.print("currentEffect = ");
  Serial.println(currentEffect);

  pinMode(ledR1Pin, OUTPUT);
  digitalWrite(ledR1Pin, HIGH);
  pinMode(ledG1Pin, OUTPUT);
  digitalWrite(ledG1Pin, HIGH);
  pinMode(ledB1Pin, OUTPUT);
  digitalWrite(ledB1Pin, HIGH);

  pinMode(buttonPin, INPUT);

  writeLed({ r: 255, g: 0, b: 0 });
  delay(500);
  writeLed({ r: 0, g: 255, b: 0 });
  delay(500);
  writeLed({ r: 0, g: 0, b: 255 });
  delay(500);
  writeLed({ r: 255, g: 255, b: 255 });
  delay(500);
  writeLed({ r: 0, g: 0, b: 0 });
  delay(500);
}

void loop() {
  //setColorFromSerial();
  //return;

  buttonDebounce.update(digitalRead(buttonPin), millis());
  SerialResponse serialResponse = readSerial();

  if (isLedOn) {
    if ((buttonDebounce.isBtnReleased && !buttonDebounce.isBtnReleasedLongPress) || serialResponse.changeEffect) {
      currentEffect = (currentEffect + 1) % effectsCount;
      isCustomColor = false;

      saveCurrentEffectData();

      if (isSerialPortOpened()) {
        Serial.print("currentEffect = ");
        Serial.println(currentEffect);
      }
    }
    if (buttonDebounce.isBtnLongPressed || serialResponse.ledOff) {
      isLedOn = false;
    }
  } else {
    if ((buttonDebounce.isBtnReleased && !buttonDebounce.isBtnReleasedLongPress) || buttonDebounce.isBtnLongPressed || serialResponse.changeEffect || serialResponse.ledOn) {
      isLedOn = true;
    }
  }

  if (serialResponse.isCustomColor) {
    isLedOn = true;
    isCustomColor = true;
    customColor = serialResponse.customColor;

    if (isSerialPortOpened()) {
      Serial.print("customColor: ");
      Serial.print(customColor.color.h);
      Serial.print(" ");
      Serial.print(customColor.color.s);
      Serial.print(" ");
      Serial.print(customColor.color.v);
      Serial.print(" ");
      Serial.print(customColor.breathDuration);
      Serial.print(" ");
      Serial.println(customColor.breathDeep);
    }

    saveCurrentEffectData();
  }

  if ((buttonDebounce.isBtnReleased && !buttonDebounce.isBtnReleasedLongPress) || buttonDebounce.isBtnLongPressed || serialResponse.changeEffect || serialResponse.ledOn || serialResponse.ledOff) {
    writeLed({ r: 0, g: 0, b: 0 });
    delay(100);
    writeLed({ r: 255, g: 255, b: 255 });
    delay(50);
    writeLed({ r: 0, g: 0, b: 0 });
    delay(100);
  }

  if (serialResponse.showNotification) {
    writeLed({ r: 255, g: 255, b: 255 });
    delay(100);
    writeLed({ r: 0, g: 0, b: 0 });
    delay(100);
    writeLed({ r: 255, g: 255, b: 255 });
    delay(100);
    writeLed({ r: 0, g: 0, b: 0 });
    delay(100);
  }

  if (isLedOn) {
    if (isCustomColor) {
      constColorBreathEffect(customColor.color, customColor.breathDuration, customColor.breathDeep);
    } else {
      if (currentEffect >= 0 && currentEffect < defaultColorsCount) {
        constColorBreathEffect(defaultColorsHSV[currentEffect].color, defaultColorsHSV[currentEffect].breathDuration, defaultColorsHSV[currentEffect].breathDeep);
      }

      if (currentEffect >= defaultColorsCount && currentEffect < customColorsCount + defaultColorsCount) {
        constColorBreathEffect(customColorsHSV[currentEffect - defaultColorsCount].color, customColorsHSV[currentEffect - defaultColorsCount].breathDuration, customColorsHSV[currentEffect - defaultColorsCount].breathDeep);

      }

      switch (currentEffect) {
        case defaultColorsCount + customColorsCount:
          rainbowCustomEffect(120000);
          break;
        case defaultColorsCount + customColorsCount + 1:
          constColorBreathEffect(customColor.color, customColor.breathDuration, customColor.breathDeep);
          break;
      }
    }
  } else {
    writeLed({ r: 0, g: 0, b: 0 });
  }

  //delay(5);
}

void setColorFromSerial() {
  if (Serial.available()) {
    int r = Serial.parseInt();
    int g = Serial.parseInt();
    int b = Serial.parseInt();
    Serial.parseInt();
    Serial.print("writeLed({r: ");
    Serial.print(r);
    Serial.print(", g: ");
    Serial.print(g);
    Serial.print(", b: ");
    Serial.print(b);
    Serial.println("});");
    writeLed({ r: r, g: g, b: b });
  }
}

void saveCurrentEffectData() {
  struct CurrentEffectData currentEffectData;
  currentEffectData.currentEffect = currentEffect;
  currentEffectData.isCustomColor = isCustomColor;
  currentEffectData.customColor = customColor;
  currentEffectData.initHash();
  EEPROM.put(currentEffectDataAddress, currentEffectData);
}

SerialResponse readSerial() {
  SerialResponse response;
  if (Serial.available()) {
    int serialInput = Serial.read();
    if (serialInput == 'e') {
      Serial.print("echo id:led_pad ");
    } else if (serialInput == 'o') {
      response.showNotification = true;
    } else if (serialInput == 'L') {
      response.ledOn = true;
    } else if (serialInput == 'l') {
      response.ledOff = true;
    } else if (serialInput == 'C') {
      response.isCustomColor = true;
      response.customColor.color.h = Serial.parseInt();
      response.customColor.color.s = Serial.parseInt();
      response.customColor.color.v = Serial.parseInt();
      response.customColor.breathDuration = Serial.parseInt();
      response.customColor.breathDeep = Serial.parseFloat();
    } else if (serialInput == 'c') {
      Serial.print(isLedOn);
      Serial.print(" ");
      Serial.print(currentEffect);
      Serial.print(" ");
      Serial.print(isCustomColor);
      Serial.print(" ");
      Serial.print(customColor.color.h);
      Serial.print(" ");
      Serial.print(customColor.color.s);
      Serial.print(" ");
      Serial.print(customColor.color.v);
      Serial.print(" ");
      Serial.print(customColor.breathDuration);
      Serial.print(" ");
      Serial.print(customColor.breathDeep);
      Serial.print(" ");
    } else {
      response.changeEffect = true;
    }

    Serial.write(serialInput);
    while (Serial.available()) {
      Serial.write(Serial.read());
    }
    Serial.println();
  }
  return response;
}

void writeLed(Color c) {
  analogWrite(ledR1Pin, 255 - c.r);
  analogWrite(ledG1Pin, 255 - c.g);
  analogWrite(ledB1Pin, 255 - c.b);
}

float rainbowEffect(unsigned long currentTime, unsigned long effectDuration, float effectOffset) {
  currentTime = currentTime % effectDuration;
  float effectTime = effectDuration > 0 ? (float)currentTime / effectDuration : 0;
  effectTime = effectTime + effectOffset;
  effectTime = effectTime - floor(effectTime);

  float effectHue = effectTime;
  return effectHue;
}

float breathEffect(unsigned long currentTime, unsigned long effectDuration, float effectOffset, int breathDeep, float valueMin, float valueMax) {
  currentTime = currentTime % effectDuration;
  float effectTime = effectDuration > 0 ? (float)currentTime / effectDuration : 0;
  effectTime = effectTime + effectOffset;
  effectTime = effectTime - floor(effectTime);

  float effectValue = breathX(effectTime);
  for (int i = 0; i < breathDeep; i++) {
    effectValue = effectValue * effectValue;
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
  writeLed(ledValue);
}

void constColorBreathEffect(ColorHSV c, unsigned long breathDuration, float breathDeep) {
  float effectValue = breathEffect(millis(), breathDuration, 0, 1, c.sF(), (1 - breathDeep) * c.sF());
  Color ledValue = hsvToRgb(c.hF(), effectValue, c.vF());
  writeLed(ledValue);
}

void rainbowEffect(float saturation) {
  float ledHue = rainbowEffect(millis(), 20000, 0);
  Color ledValue = hsvToRgb(ledHue, saturation, 1);
  writeLed(ledValue);
}

void rainbowBreathEffect(float saturation) {
  float ledHue = rainbowEffect(millis(), 60000, 0);
  float effectValue = breathEffect(millis(), 8000, 0, 2, 0.1, 1);
  Color ledValue = hsvToRgb(ledHue, saturation, effectValue);
  writeLed(ledValue);
}

void rainbowCustomEffect(unsigned long effectDuration) {
  unsigned long currentTime = millis() % effectDuration;
  float effectTime = (float)currentTime / effectDuration;
  const int stagesCount = 22;
  int i = floor(effectTime * stagesCount);
  int f = (effectTime * stagesCount - i) * 256;
  Color ledValue;
  switch (i % stagesCount) {
    case 0:
      ledValue.r = 255 - f;
      ledValue.g = 255;
      ledValue.b = 255;
      break;
    case 1 ... 4:
      ledValue.r = 0;
      ledValue.g = 255;
      ledValue.b = 255;
      break;
    case 5:
      ledValue.r = f;
      ledValue.g = 255;
      ledValue.b = 255 - f;
      break;
    case 6 ... 9:
      ledValue.r = 255;
      ledValue.g = 255;
      ledValue.b = 0;
      break;
    case 10:
      ledValue.r = 255 - f;
      ledValue.g = 255;
      ledValue.b = 0;
      break;
    case 11 ... 14:
      ledValue.r = 0;
      ledValue.g = 255;
      ledValue.b = 0;
      break;
    case 15:
      ledValue.r = f;
      ledValue.g = 255;
      ledValue.b = f;
      break;
    case 16:
      ledValue.r = 255;
      ledValue.g = 255 - f;
      ledValue.b = 255;
      break;
    case 17 ... 20:
      ledValue.r = 255;
      ledValue.g = 0;
      ledValue.b = 255;
      break;
    case 21:
      ledValue.r = 255;
      ledValue.g = f;
      ledValue.b = 255;
      break;
  }

  //float breathEffectValue = breathEffect(millis(), 8000, 0, 1, c.sF(), 0.85 * c.sF());

  writeLed(ledValue);
}
