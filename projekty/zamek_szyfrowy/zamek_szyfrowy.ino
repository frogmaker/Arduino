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
uint8_t entered[CODE_LENGTH] = {};
uint8_t enteredCount = 0;
uint32_t stateAt = 0;
uint32_t lastKeyAt = 0;

// Jawne deklaracje zapobiegaja problemom generatora prototypow z typem LockState.
void setState(LockState next, uint32_t now);
void rejectCode(uint32_t now);
void resetCode();
void printStatus();
void printHeldButtons();
void logButtons();

void resetCode() {
  progress = 0;
  enteredCount = 0;
}

void printStatus() {
  Serial.print(F("Stan: "));
  if (state == OPEN) Serial.print(F("OTWARTY"));
  else if (state == ERROR_STATE) Serial.print(F("BLAD"));
  else Serial.print(F("ZAMKNIETY"));
  Serial.print(F(" | Wpisano: ["));
  for (uint8_t i = 0; i < CODE_LENGTH; ++i) {
    if (i > 0) Serial.print(' ');
    if (i < enteredCount) Serial.print(entered[i]);
    else Serial.print('_');
  }
  Serial.println(F("]"));
}

void printHeldButtons() {
  Serial.print(F("Trzymane: ["));
  bool first = true;
  for (uint8_t i = 0; i < 4; ++i) {
    if (!keys[i].down()) continue;
    if (!first) Serial.print(' ');
    Serial.print('K');
    Serial.print(i + 1);
    first = false;
  }
  if (first) Serial.print(F("brak"));
  Serial.println(F("]"));
}

void logButtons() {
  for (uint8_t i = 0; i < 4; ++i) {
    if (keys[i].pressedEvent) {
      Serial.print(F("Nacisnieto K"));
      Serial.print(i + 1);
      Serial.print(F(" (D"));
      Serial.print(i + 2);
      Serial.println(F(")."));
    }
    if (keys[i].releasedEvent) {
      Serial.print(F("Zwolniono K"));
      Serial.print(i + 1);
      Serial.println(F("."));
    }
  }
  printHeldButtons();
}

void setState(LockState next, uint32_t now) {
  state = next;
  stateAt = now;
  digitalWrite(GREEN_PIN, next == OPEN ? HIGH : LOW);
  digitalWrite(RED_PIN, next == OPEN ? LOW : HIGH);
  printStatus();
}

void rejectCode(uint32_t now) {
  resetCode();
  setState(ERROR_STATE, now);
  Serial.println(F("Proba skasowana. Pusc przyciski i sprobuj ponownie."));
}

void setup() {
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  for (uint8_t i = 0; i < 4; ++i) keys[i].begin();
  Serial.begin(9600);
  Serial.println(F("Model zamka. Kod poczatkowy: 1-3-2-4."));
  Serial.println(F("Przyciski: K1=D2, K2=D3, K3=D4, K4=D5."));
  resetCode();
  setState(LOCKED, millis());
  printHeldButtons();
}

void loop() {
  const uint32_t now = millis();
  uint8_t downCount = 0;
  uint8_t eventCount = 0;
  uint8_t keyNumber = 0;
  bool buttonsChanged = false;
  for (uint8_t i = 0; i < 4; ++i) {
    keys[i].update(now);
    if (keys[i].down()) ++downCount;
    if (keys[i].pressedEvent) { ++eventCount; keyNumber = i + 1; }
    if (keys[i].pressedEvent || keys[i].releasedEvent) buttonsChanged = true;
  }
  if (buttonsChanged) logButtons();
  if (state == OPEN) {
    if (eventCount > 0) {
      Serial.println(F("Nacisniecie pominiete: zamek jest otwarty."));
      printStatus();
    }
    if (uint32_t(now - stateAt) >= 5000UL && downCount == 0) {
      resetCode();
      setState(LOCKED, now);
      Serial.println(F("Ponownie zamkniety."));
    }
    return;
  }
  if (state == ERROR_STATE) {
    if (eventCount > 0) {
      Serial.println(F("Nacisniecie pominiete: trwa sygnalizacja bledu."));
      printStatus();
    }
    digitalWrite(RED_PIN, (now - stateAt) / 125UL % 2UL ? LOW : HIGH);
    if (uint32_t(now - stateAt) >= 1000UL && downCount == 0) setState(LOCKED, now);
    return;
  }
  if (progress > 0 && uint32_t(now - lastKeyAt) >= 10000UL) {
    resetCode();
    Serial.println(F("Minelo 10 s bez kolejnego klawisza. Proba skasowana."));
    printStatus();
  }
  if (eventCount == 0) return;
  if (eventCount > 1 || downCount > 1) {
    Serial.println(F("Blad: naciskaj jeden przycisk naraz."));
    rejectCode(now);
    return;
  }
  lastKeyAt = now;
  if (enteredCount < CODE_LENGTH) entered[enteredCount++] = keyNumber;
  if (keyNumber != SECRET[progress]) {
    Serial.println(F("Bledny kod. Proba przed skasowaniem:"));
    printStatus();
    rejectCode(now);
    return;
  }
  ++progress;
  if (progress == CODE_LENGTH) {
    progress = 0;
    setState(OPEN, now);
    Serial.println(F("Kod poprawny. Zielona LED = otwarty przez co najmniej 5 s."));
  } else printStatus();
}
