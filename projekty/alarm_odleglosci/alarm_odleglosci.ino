/*
  Alarm odleglosci: klasyczne Nano 5 V, HC-SR04, czerwona LED, buzzer aktywny.
  TRIG=D6, ECHO=D7, LED=D8 przez 1 kOhm, EN brzeczyka przez L293D=D9.
  Pelny tor L293D: patrz tor_brzeczyka.pdf. Brak dodatkowych bibliotek.
*/
#include <Arduino.h>

const uint8_t TRIG_PIN = 6;
const uint8_t ECHO_PIN = 7;
const uint8_t LED_PIN = 8;
const uint8_t BUZZER_EN_PIN = 9;
const float ALARM_CM = 10.0f;          // Alarm wylacznie PONIZEJ 10 cm.
const uint32_t READ_INTERVAL_MS = 70;
const uint32_t BLINK_INTERVAL_MS = 200;
const unsigned long ECHO_TIMEOUT_US = 25000UL;

uint32_t lastReadAt = 0;
uint32_t lastBlinkAt = 0;
bool validMeasurement = false;
bool alarmActive = false;
bool signalOn = false;
float distanceCm = 0;

void setSignal(bool on) {
  digitalWrite(LED_PIN, on ? HIGH : LOW);
  // HIGH wlacza EN; 1A ukladu L293D jest na stale polaczone z GND.
  digitalWrite(BUZZER_EN_PIN, on ? HIGH : LOW);
}

void measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  const unsigned long echoUs = pulseIn(ECHO_PIN, HIGH, ECHO_TIMEOUT_US);
  distanceCm = echoUs / 58.0f;
  // Zero oznacza timeout, a nie odleglosc 0 cm. Ponizej 2 cm jest strefa martwa.
  validMeasurement = echoUs != 0 && distanceCm >= 2.0f && distanceCm <= 400.0f;
  const bool newAlarm = validMeasurement && distanceCm < ALARM_CM;
  if (newAlarm != alarmActive) {
    alarmActive = newAlarm;
    signalOn = alarmActive;           // Pierwszy sygnal od razu po wykryciu.
    lastBlinkAt = millis();
  }
  if (!alarmActive) signalOn = false;

  if (validMeasurement) {
    Serial.print(F("Odleglosc [cm]: "));
    Serial.print(distanceCm, 2);
    Serial.println(alarmActive ? F("  ALARM") : F("  OK"));
  } else {
    Serial.println(F("Brak poprawnego pomiaru. Sprawdz ECHO i zakres 2-400 cm."));
  }
}

void setup() {
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_EN_PIN, LOW);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_EN_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);
  setSignal(false);
  Serial.begin(9600);
  lastReadAt = millis() - READ_INTERVAL_MS;
}

void loop() {
  uint32_t now = millis();
  if (uint32_t(now - lastReadAt) >= READ_INTERVAL_MS) {
    measureDistance();
    // Odstep co najmniej 70 ms od zakonczenia poprzedniego pomiaru.
    lastReadAt = millis();
  }
  now = millis();
  if (alarmActive && uint32_t(now - lastBlinkAt) >= BLINK_INTERVAL_MS) {
    lastBlinkAt = now;
    signalOn = !signalOn;
  }
  setSignal(alarmActive && signalOn);
  // Osobna sygnalizacja diagnostyczna: brak pomiaru, nigdy "0 cm".
  digitalWrite(LED_BUILTIN, !validMeasurement && ((now / 500UL) % 2UL) ? HIGH : LOW);
}
