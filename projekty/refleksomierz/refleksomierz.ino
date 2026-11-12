/* Nano 5 V: przycisk D2--GND; czerwona LED D8, zielona LED D7, kazda przez 1kOhm. */
#include <Arduino.h>
#include "Przycisk.h"

const uint8_t RED_PIN = 8;
const uint8_t GREEN_PIN = 7;
Przycisk button(2);

enum Phase { IDLE, WAIT_RELEASE, WAIT_GO, GO, RESULT, FALSE_START };
Phase phase = IDLE;
uint32_t phaseAt = 0;
uint32_t waitingMs = 0;
uint32_t goAt = 0;
uint32_t reactionMs = 0;

// Jawne deklaracje zachowuja typ Phase przy przetwarzaniu szkicu przez Arduino IDE.
void enterPhase(Phase next, uint32_t now);
void falseStart(uint32_t now);

void enterPhase(Phase next, uint32_t now) {
  phase = next;
  phaseAt = now;
  digitalWrite(RED_PIN, next == GO || next == RESULT ? LOW : HIGH);
  digitalWrite(GREEN_PIN, next == GO || next == RESULT ? HIGH : LOW);
}

void falseStart(uint32_t now) {
  Serial.println(F("FALSTART. Poczekaj na zielona diode."));
  enterPhase(FALSE_START, now);
}

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  button.begin();
  Serial.begin(9600);
  // Niepodlaczone A7 daje prosty zarodek losowania; nie jest to losowosc kryptograficzna.
  randomSeed((uint32_t(analogRead(A7)) << 16) ^ micros());
  enterPhase(IDLE, millis());
  Serial.println(F("Nacisnij i pusc przycisk, aby rozpoczac."));
}

void loop() {
  const uint32_t now = millis();
  button.update(now);
  switch (phase) {
    case IDLE:
      if (button.pressedEvent) enterPhase(WAIT_RELEASE, now);
      break;
    case WAIT_RELEASE:
      if (!button.down()) {
        waitingMs = random(1500, 5001);
        enterPhase(WAIT_GO, now);
        Serial.println(F("Czekaj..."));
      }
      break;
    case WAIT_GO:
      if (button.pressedEvent) {
        falseStart(now);
      } else if (uint32_t(now - phaseAt) >= waitingMs) {
        goAt = now;
        enterPhase(GO, now);
      }
      break;
    case GO:
      if (button.pressedEvent) {
        // Odrzuc takze nacisniecie sprzed zielonego potwierdzone dopiero po debounce.
        if (int32_t(button.pressedAt - goAt) < 0) {
          falseStart(now);
        } else {
          reactionMs = uint32_t(button.pressedAt - goAt);
          Serial.print(F("Czas reakcji [ms]: ")); Serial.println(reactionMs);
          enterPhase(RESULT, now);
        }
      } else if (uint32_t(now - goAt) >= 5000UL) {
        Serial.println(F("Brak reakcji przez 5 sekund."));
        enterPhase(RESULT, now);
      }
      break;
    case RESULT:
    case FALSE_START:
      if (phase == FALSE_START) digitalWrite(RED_PIN, (now - phaseAt) / 125UL % 2UL ? LOW : HIGH);
      if (uint32_t(now - phaseAt) >= 1800UL && !button.down()) {
        enterPhase(IDLE, now);
        Serial.println(F("Nacisnij i pusc przycisk, aby rozpoczac."));
      }
      break;
  }
}
