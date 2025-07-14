#include <EEPROM.h>
#include <math.h>
#include "mapf.h"
#include "color.h"
#include "isSerialPortOpened.h"
#include "buttonDebounce.h"

struct CurrentEffectData {
  int currentEffect;
  int hash;

  void initHash() { hash = getHash(); }
  bool isHashValid() { return getHash() == hash; }
  int getHash() { return 12 + currentEffect; }
};

int currentEffectDataAddress = 8;

struct SerialResponse {
  bool changeEffect = false;
  bool showNotification = false;
};

const int ledR1Pin = 5;
const int ledG1Pin = 9;
const int ledB1Pin = 6;
const int buttonPin = 8;

ButtonDebounce buttonDebounce(100, 2000);

const int defaultColorsCount = 8;
ColorHSV defaultColorsHSV[defaultColorsCount] = {
  { h: 0, s: 0, v: 0 },
  { h: 0, s: 100, v: 100 },
  { h: 120, s: 100, v: 100 },
  { h: 240, s: 100, v: 100 },
  { h: 60, s: 100, v: 100 },
  { h: 300, s: 100, v: 100 },
  { h: 180, s: 100, v: 100 },
  { h: 0, s: 0, v: 100 }
};

const int customColorsCount = 5;
ColorHSV customColorsHSV[customColorsCount] = {
  { h: 17, s: 84, v: 100 },
  { h: 90, s: 80, v: 100 },
  { h: 57, s: 95, v: 100 },
  { h: 0, s: 22, v: 100 },
  { h: 293, s: 80, v: 100 }
};

const int effectsCount = defaultColorsCount /*+ customColorsCount*/ + 1;
int currentEffect = 0;

void setup() {
  struct CurrentEffectData currentEffectData;
  EEPROM.get(currentEffectDataAddress, currentEffectData);
  if (currentEffectData.isHashValid()) {
    currentEffect = currentEffectData.currentEffect % effectsCount;
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

  if (buttonDebounce.isBtnReleased || serialResponse.changeEffect) {
    writeLed({ r: 0, g: 0, b: 0 });
    delay(100);
    writeLed({ r: 255, g: 255, b: 255 });
    delay(50);
    writeLed({ r: 0, g: 0, b: 0 });
    delay(100);
    
    currentEffect = (currentEffect + 1) % effectsCount;
    
    struct CurrentEffectData currentEffectData;
    currentEffectData.currentEffect = currentEffect;
    currentEffectData.initHash();
    EEPROM.put(currentEffectDataAddress, currentEffectData);

    if (isSerialPortOpened()) {
      Serial.print("currentEffect = ");
      Serial.println(currentEffect);
    }
  }

  if (serialResponse.showNotification) {
    writeLed({ r: 0, g: 0, b: 0 });
    delay(200);
    writeLed({ r: 255, g: 255, b: 255 });
    delay(100);
    writeLed({ r: 0, g: 0, b: 0 });
    delay(100);
  }

  if (currentEffect >= 0 && currentEffect < defaultColorsCount) {
    constColorEffect(defaultColorsHSV[currentEffect]);
  }

  // if (currentEffect >= defaultColorsCount && currentEffect < customColorsCount + defaultColorsCount) {
  //   constColorBreathEffect(customColorsHSV[currentEffect - defaultColorsCount]);
  // }

  switch (currentEffect) {
    case defaultColorsCount /*+ customColorsCount*/:
      rainbowCustomEffect(120000);
      break;
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

SerialResponse readSerial() {
  SerialResponse response;
  if (Serial.available()) {
    int serialInput = Serial.read();
    if (serialInput == 'e') {
      Serial.print("echo id:led_pad ");  
    } else if (serialInput == 'o') {
      response.showNotification = true;
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
  writeLed(ledValue);
}

void constColorBreathEffect(ColorHSV c) {
  float effectValue = breathEffect(millis(), 8000, 0, 2, 0.1 * c.vF(), c.vF());
  Color ledValue = hsvToRgb(c.hF(), c.sF(), effectValue);
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
  writeLed(ledValue);
}
