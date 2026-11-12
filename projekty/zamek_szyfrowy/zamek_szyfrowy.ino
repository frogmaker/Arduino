/* Model zamka: 4 przyciski na D2-D5 do GND; LED zielona D7, czerwona D8 przez 1kOhm. */
#include <Arduino.h>
#include "Przycisk.h"

const uint8_t GREEN_PIN = 7;
const uint8_t RED_PIN = 8;
const uint8_t SECRET[] = {1, 3, 2, 4};
const uint8_t CODE_LENGTH = sizeof(SECRET) / sizeof(SECRET[0]);
Przycisk keys[] = {Przycisk(2), Przycisk(3), Przycisk(4), Przycisk(5)};
enum LockState { LOCKED, OPEN, ERROR_STATE };
LockState state = LOCKED;
uint8_t progress = 0;
uint32_t stateAt = 0;
uint32_t lastKeyAt = 0;

// Jawne deklaracje zapobiegaja problemom generatora prototypow z typem LockState.
void setState(LockState next, uint32_t now);
void rejectCode(uint32_t now);

void setState(LockState next, uint32_t now) {
  state = next;
  stateAt = now;
  digitalWrite(GREEN_PIN, next == OPEN ? HIGH : LOW);
  digitalWrite(RED_PIN, next == OPEN ? LOW : HIGH);
}

void rejectCode(uint32_t now) {
  progress = 0;
  setState(ERROR_STATE, now);
  Serial.println(F("Bledny kod. Pusc przyciski i sprobuj ponownie."));
}

void setup() {
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  for (uint8_t i = 0; i < 4; ++i) keys[i].begin();
  Serial.begin(9600);
  setState(LOCKED, millis());
  Serial.println(F("Model zamka. Kod poczatkowy: 1-3-2-4."));
}

void loop() {
  const uint32_t now = millis();
  uint8_t downCount = 0;
  uint8_t eventCount = 0;
  uint8_t keyNumber = 0;
  for (uint8_t i = 0; i < 4; ++i) {
    keys[i].update(now);
    if (keys[i].down()) ++downCount;
    if (keys[i].pressedEvent) { ++eventCount; keyNumber = i + 1; }
  }
  if (state == OPEN) {
    if (uint32_t(now - stateAt) >= 5000UL && downCount == 0) {
      progress = 0;
      setState(LOCKED, now);
      Serial.println(F("Ponownie zamkniety."));
    }
    return;
  }
  if (state == ERROR_STATE) {
    digitalWrite(RED_PIN, (now - stateAt) / 125UL % 2UL ? LOW : HIGH);
    if (uint32_t(now - stateAt) >= 1000UL && downCount == 0) setState(LOCKED, now);
    return;
  }
  if (progress > 0 && uint32_t(now - lastKeyAt) >= 3000UL) {
    progress = 0;
    Serial.println(F("Przerwa ponad 3 s: zacznij kod od poczatku."));
  }
  if (eventCount == 0) return;
  if (eventCount > 1 || downCount > 1) { rejectCode(now); return; }
  lastKeyAt = now;
  if (keyNumber != SECRET[progress]) { rejectCode(now); return; }
  ++progress;
  Serial.print(F("Poprawne klawisze: ")); Serial.println(progress);
  if (progress == CODE_LENGTH) {
    progress = 0;
    setState(OPEN, now);
    Serial.println(F("Kod poprawny. Zielona LED = otwarty przez co najmniej 5 s."));
  }
}
