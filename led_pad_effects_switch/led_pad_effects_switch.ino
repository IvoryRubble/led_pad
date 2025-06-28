
const int ledPin = 12;
const int ledEffectPin = 13;
const int buttonPin = 2;

const int effectsCount = 2;
int currentEffect = 0;

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  pinMode(ledEffectPin, OUTPUT);
  digitalWrite(ledEffectPin, LOW);

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

void effect0() {
  const unsigned long effectDuration = 1500;
  unsigned long currentTime = millis() % effectDuration;
  int ledValue = map(currentTime, 0, effectDuration, 0, 255);
  analogWrite(ledEffectPin, ledValue);
}

void effect1() {
  const unsigned long effectDuration = 1500;
  unsigned long currentTime = millis() % effectDuration;
  int ledValue = map(currentTime, 0, effectDuration, 255, 0);
  analogWrite(ledEffectPin, ledValue);
}
