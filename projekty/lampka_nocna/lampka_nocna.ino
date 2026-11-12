/*
  Lampka nocna. Nano 5 V; LED=D3 przez 1 kOhm.
  5V--LDR--A0--1kOhm--GND. Potencjometr 5kOhm: 5V/suwak=A1/GND.
*/
#include <Arduino.h>

const uint8_t LIGHT_PIN = A0;
const uint8_t POT_PIN = A1;
const uint8_t LED_PIN = 3;             // Pin PWM w klasycznym Nano.
const int HYSTERESIS = 15;
const uint32_t SAMPLE_MS = 20;

uint32_t lastSampleAt = 0;
uint32_t lastPrintAt = 0;
int filteredLight = 0;
int threshold = 500;
int brightness = 0;
bool night = false;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, 0);
  Serial.begin(9600);
  filteredLight = analogRead(LIGHT_PIN);
}

void loop() {
  const uint32_t now = millis();
  if (uint32_t(now - lastSampleAt) >= SAMPLE_MS) {
    lastSampleAt = now;
    filteredLight = (filteredLight * 7 + analogRead(LIGHT_PIN)) / 8;
    threshold = map(analogRead(POT_PIN), 0, 1023, 20, 1000);

    if (!night && filteredLight < threshold - HYSTERESIS) night = true;
    if (night && filteredLight > threshold + HYSTERESIS) night = false;

    int target = 0;
    if (night) {
      const int darkness = constrain(threshold - filteredLight, 0, threshold);
      target = 40 + (215L * darkness) / threshold;
    }
    // Powolne rozjasnianie i wygaszanie zamiast naglego skoku.
    if (brightness < target) brightness = min(brightness + 5, target);
    if (brightness > target) brightness = max(brightness - 5, target);
    analogWrite(LED_PIN, brightness);
  }
  if (uint32_t(now - lastPrintAt) >= 250UL) {
    lastPrintAt = now;
    Serial.print(F("Swiatlo ADC=")); Serial.print(filteredLight);
    Serial.print(F(" prog=")); Serial.print(threshold);
    Serial.print(F(" PWM=")); Serial.println(brightness);
  }
}
