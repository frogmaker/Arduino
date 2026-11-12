#pragma once
#include <Arduino.h>

// Jeden przycisk miedzy pinem a GND; programowe tlumienie drgan przez 30 ms.
class Przycisk {
 public:
  explicit Przycisk(uint8_t pin) : pin_(pin) {}
  void begin() {
    pinMode(pin_, INPUT_PULLUP);
    raw_ = stable_ = digitalRead(pin_);
    changedAt_ = millis();
  }
  void update(uint32_t now) {
    pressedEvent = releasedEvent = false;
    const uint8_t reading = digitalRead(pin_);
    if (reading != raw_) { raw_ = reading; changedAt_ = now; }
    if (stable_ != raw_ && uint32_t(now - changedAt_) >= 30UL) {
      stable_ = raw_;
      if (stable_ == LOW) { pressedEvent = true; pressedAt = changedAt_; }
      else releasedEvent = true;
    }
  }
  bool down() const { return stable_ == LOW; }
  bool pressedEvent = false;
  bool releasedEvent = false;
  uint32_t pressedAt = 0;
 private:
  uint8_t pin_;
  uint8_t raw_ = HIGH;
  uint8_t stable_ = HIGH;
  uint32_t changedAt_ = 0;
};
