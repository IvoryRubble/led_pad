#ifndef ISSERIALPORTOPENED_H
#define ISSERIALPORTOPENED_H

bool isSerialPortOpened() {
  return Serial.availableForWrite() >= 32;
}

#endif
