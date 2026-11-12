/*
  Model sygnalizacji swietlnej, klasyczne Nano / klon z logika 5 V.
  D7 -> 1 kOhm -> anoda LED zielonej; katoda -> GND.
  D8 -> 1 kOhm -> anoda LED czerwonej; katoda -> GND.
  D9 -> 1 kOhm -> anoda LED zoltej; katoda -> GND.
  Cykl: zielone 5 s, zolte 1 s, czerwone 5 s, czerwone+zolte 1 s.
*/
#include <Arduino.h>

const uint8_t GREEN_PIN = 7;
const uint8_t RED_PIN = 8;
const uint8_t YELLOW_PIN = 9;
const uint8_t PHASE_COUNT = 4;
const uint32_t PHASE_MS[PHASE_COUNT] = {5000UL, 1000UL, 5000UL, 1000UL};

uint8_t phase = 0;
uint32_t phaseStartedAt = 0;

void applyLights() {
  // Najpierw wygas wszystkie LED, aby nie laczyc czerwonego z zielonym.
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(RED_PIN, LOW);

  switch (phase) {
    case 0:
      digitalWrite(GREEN_PIN, HIGH);
      Serial.println(F("ZIELONE: 5 s"));
      break;
    case 1:
      digitalWrite(YELLOW_PIN, HIGH);
      Serial.println(F("ZOLTE: 1 s"));
      break;
    case 2:
      digitalWrite(RED_PIN, HIGH);
      Serial.println(F("CZERWONE: 5 s"));
      break;
    case 3:
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(YELLOW_PIN, HIGH);
      Serial.println(F("CZERWONE + ZOLTE: 1 s"));
      break;
  }
}

void setup() {
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(RED_PIN, LOW);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  Serial.begin(9600);
  phase = 0;
  applyLights();
  phaseStartedAt = millis();
}

void loop() {
  const uint32_t now = millis();
  bool changed = false;
  // Zachowaj planowany czas faz; odejmowanie uint32_t obsluguje zawiniecie millis().
  while (uint32_t(now - phaseStartedAt) >= PHASE_MS[phase]) {
    phaseStartedAt += PHASE_MS[phase];
    phase = (phase + 1) % PHASE_COUNT;
    changed = true;
  }
  if (changed) applyLights();
}
