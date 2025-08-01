// https://gist.github.com/IvoryRubble/4eb6355ca0a115232f57452e6ee2bdb1
//
// Author: Yaroslav Metelev <yaroslav.metelev@outlook.com>
//
// Button debounce class for Arduino
// Usage:
// Construct global instance. Optionally set custom debounceDelay and longPressTimeout in constructor:
//    ButtonDebounce btnDebounce(25);
// Call ButtonDebounce::updateState() method in loop() procedure. Send button state as input param:
//    btnDebounce.updateState(digitalRead(A1));
// Get state flags from ButtonDebounce object to get debounsed button sates and events:
//    Serial.println(btnDebounce.btnState);
//    Serial.println(btnDebounce.isBtnReleased);
//    Serial.println(btnDebounce.isBtnReleasedLongPress);
//    Serial.println(btnDebounce.isBtnPressed);

#ifndef ButtonDebounce_h
#define ButtonDebounce_h

class ButtonDebounce {
  public:
    ButtonDebounce(unsigned long debounceDelay = 100, unsigned long longPressTimeout = 1500) : debounceDelay(debounceDelay), longPressTimeout(longPressTimeout) { }

    bool btnState = false;
    bool isBtnPressed = false;
    bool isBtnReleased = false;
    bool isBtnReleasedLongPress = false;
    
    bool isBtnLongPressed = false;

    void update(bool btnStateInput, unsigned long currentTime) {
      btnStateInternal = btnStateInput;
      isBtnPressed = false;
      isBtnReleased = false;
      isBtnReleasedLongPress = false;
      isBtnLongPressed = false;

      if (!debounceDelayPassed && currentTime - previousStateChangeTime >= debounceDelay) {
        debounceDelayPassed = true;
      }

      if (!longPressTimeoutPassed && currentTime - previousStateChangeTime >= longPressTimeout) {
        longPressTimeoutPassed = true;
      }

      if (btnStateInternal != previousState && debounceDelayPassed) {
        btnState = btnStateInternal;
        isBtnPressed = btnStateInternal;
        isBtnReleased = !btnStateInternal;

        if (isBtnReleased && longPressTimeoutPassed) {
          isBtnReleasedLongPress = true;
        }

        debounceDelayPassed = false;
        longPressTimeoutPassed = false;
        isButtonLongPressedEventTriggered = false;
        previousStateChangeTime = currentTime;
        previousState = btnStateInternal;
      }

      if (btnState && longPressTimeoutPassed && !isButtonLongPressedEventTriggered) {
        isBtnLongPressed = true;
        isButtonLongPressedEventTriggered = true;
      }
    }
  private:
    bool debounceDelayPassed = false;
    unsigned long debounceDelay;

    bool longPressTimeoutPassed = false;
    unsigned long longPressTimeout;

    uint32_t previousStateChangeTime = 0;

    bool btnStateInternal = false;
    bool previousState = false;

    bool isButtonLongPressedEventTriggered = false;
};

#endif