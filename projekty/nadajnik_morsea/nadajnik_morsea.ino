/* Nano 5 V: SOS=D2--GND, klucz reczny=D3--GND, LED=D8 przez 1kOhm, L293D EN=D9. */
#include <Arduino.h>
#include "Przycisk.h"

const uint8_t LED_PIN = 8;
const uint8_t BUZZER_EN_PIN = 9;
const uint32_t UNIT_MS = 180;
const uint8_t PULSE_UNITS[] = {1, 1, 1, 3, 3, 3, 1, 1, 1};
Przycisk sosButton(2);
Przycisk manualKey(3);
bool automatic = false;
bool signalOn = false;
uint8_t pulseIndex = 0;
uint32_t phaseAt = 0;
uint32_t phaseLength = 0;

void setSignal(bool on) {
  signalOn = on;
  digitalWrite(LED_PIN, on ? HIGH : LOW);
  digitalWrite(BUZZER_EN_PIN, on ? HIGH : LOW);
}

void startSOS(uint32_t now) {
  automatic = true;
  pulseIndex = 0;
  phaseAt = now;
  phaseLength = UNIT_MS * PULSE_UNITS[0];
  setSignal(true);
  Serial.println(F("SOS: ...---... (jeden ciagly znak)"));
}

void setup() {
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_EN_PIN, LOW);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_EN_PIN, OUTPUT);
  setSignal(false);
  sosButton.begin();
  manualKey.begin();
  Serial.begin(9600);
  Serial.println(F("D2: start/stop SOS. D3: klucz reczny, przerywa SOS."));
}

void loop() {
  const uint32_t now = millis();
  sosButton.update(now);
  manualKey.update(now);

  if (manualKey.down()) {
    automatic = false;
    setSignal(true);
    return;
  }
  if (sosButton.pressedEvent) {
    if (automatic) { automatic = false; setSignal(false); }
    else startSOS(now);
  }
  if (!automatic) {
    setSignal(false);
    return;
  }
  if (uint32_t(now - phaseAt) < phaseLength) return;
  phaseAt = now;
  if (signalOn) {
    setSignal(false);
    if (pulseIndex == 8) {
      automatic = false;
      Serial.println(F("Koniec SOS."));
    } else {
      // SOS jest jednym ciaglym znakiem, wiec kazda przerwa ma 1 jednostke.
      phaseLength = UNIT_MS;
    }
  } else {
    ++pulseIndex;
    phaseLength = UNIT_MS * PULSE_UNITS[pulseIndex];
    setSignal(true);
  }
}
