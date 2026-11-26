/*
  Laboratorium wahadla — Arduino Uno / klasyczne Nano, ATmega328P, 5 V.
  LCD 16x2: RS=D12, E=D11, D4=D7, D5=D6, D6=D5, D7=D4.
  Kontaktron CMD14: D2--GND; START / ANULUJ: D3--GND.
  Zielona LED: D9--1 kOhm--anoda; katoda do GND.
  Magnes na ciezarku; przewodowa czesc kontaktronu pozostaje nieruchoma.

  21 przejsc = 10 pelnych okresow: 1->3, 3->5, ... 19->21.
  g = 4*pi*pi*L/Tsr^2, dla prostego wahadla o malym wychyleniu.
  Zmien DLUGOSC_M na odleglosc od zawieszenia do srodka masy
  ciezarka wraz z magnesem. To doswiadczenie, nie miernik wzorcowy.

  Biblioteka: LiquidCrystal. Instrukcja i ograniczenia: README.md.
  Monitor portu szeregowego: 9600 baud.
*/

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <math.h>

const byte KONTAKTRON_PIN = 2, PRZYCISK_PIN = 3, LED_PIN = 9;
const float DLUGOSC_M = 0.50f;  // zakres cwiczenia: 0.10--2.00 m
const byte LICZBA_OKRESOW = 10, LICZBA_PRZEJSC = 2 * LICZBA_OKRESOW + 1;
const uint32_t DRGANIA_STYKU_MS = 10, DRGANIA_PRZYCISKU_MS = 30;
const uint32_t MIN_ODSTEP_US = 100000;  // odrzucamy zwarcia blizsze niz 100 ms
const uint32_t CZEKANIE_MS = 15000, PRZERWA_MS = 5000, SESJA_MS = 60000;
const float PI_WAHADLA = 3.14159265358979323846f;

LiquidCrystal lcd(12, 11, 7, 6, 5, 4);

enum Stan { GOTOWY, CZEKANIE, POMIAR, WYNIK, BLAD };
Stan stan = GOTOWY;
byte przejscia = 0, okresy = 0;
float ostatniOkres = NAN, sredniOkres = NAN, przyspieszenie = NAN;
uint32_t startSesjiMs = 0, ostatniePrzejscieMs = 0;
uint32_t pierwszeUs = 0, poczatekOkresuUs = 0, ostatnieZwarcieUs = 0;
bool odswiezLCD = true, gotowNaZwarcie = false;
const __FlashStringHelper* powodBledu = nullptr;

bool odczytStyku = HIGH, stanStyku = HIGH;
bool odczytPrzycisku = HIGH, stanPrzycisku = HIGH;
uint32_t zmianaStykuMs = 0, zmianaStykuUs = 0, zmianaPrzyciskuMs = 0;

bool poprawnaDlugosc(float dlugosc) {
  return !isnan(dlugosc) && !isinf(dlugosc) && dlugosc >= 0.10f && dlugosc <= 2.00f;
}

bool trwaPomiar() {
  return stan == CZEKANIE || stan == POMIAR;
}

void wyzerujDane() {
  przejscia = okresy = 0;
  ostatniOkres = sredniOkres = przyspieszenie = NAN;
  powodBledu = nullptr;
}

void zglosBlad(const __FlashStringHelper* powod) {
  stan = BLAD;
  powodBledu = powod;
  sredniOkres = przyspieszenie = NAN;  // nie pokazuj wyniku z niepelnej proby
  odswiezLCD = true;
  Serial.print(F("BLAD: "));
  Serial.println(powod);
}

void rozpocznijPomiar(uint32_t terazMs) {
  wyzerujDane();
  if (!poprawnaDlugosc(DLUGOSC_M)) {
    zglosBlad(F("Dlugosc 0.1-2m"));
    return;
  }
  stan = CZEKANIE;
  startSesjiMs = terazMs;
  // Zwarcie obecne przed START nie jest nowym przejsciem magnesu.
  gotowNaZwarcie = stanStyku == HIGH && odczytStyku == HIGH;
  odswiezLCD = true;
  Serial.print(F("START: L="));
  Serial.print(DLUGOSC_M, 3);
  Serial.println(F(" m. Czekam na 21 przejsc (10 okresow)."));
}

void obsluzPrzejscie(uint32_t czasUs, uint32_t terazMs) {
  if (!trwaPomiar()) return;
  if (stan == POMIAR && uint32_t(czasUs - ostatnieZwarcieUs) < MIN_ODSTEP_US) return;
  ostatnieZwarcieUs = czasUs;
  ostatniePrzejscieMs = terazMs;
  ++przejscia;
  if (stan == CZEKANIE) {
    stan = POMIAR;
    pierwszeUs = poczatekOkresuUs = czasUs;
    Serial.println(F("Przejscie 1: poczatek pomiaru czasu."));
  } else if (przejscia % 2 == 1) {
    ++okresy;
    ostatniOkres = uint32_t(czasUs - poczatekOkresuUs) / 1000000.0f;
    poczatekOkresuUs = czasUs;
    Serial.print(F("Okres "));
    Serial.print(okresy);
    Serial.print(F("/10: "));
    Serial.print(ostatniOkres, 4);
    Serial.println(F(" s"));
    if (przejscia == LICZBA_PRZEJSC) {
      sredniOkres = uint32_t(czasUs - pierwszeUs) / (1000000.0f * LICZBA_OKRESOW);
      przyspieszenie = 4.0f * PI_WAHADLA * PI_WAHADLA * DLUGOSC_M /
                       (sredniOkres * sredniOkres);
      if (!isfinite(sredniOkres) || sredniOkres <= 0 || !isfinite(przyspieszenie)) {
        zglosBlad(F("Niepoprawny czas"));
        return;
      }
      stan = WYNIK;
      Serial.print(F("WYNIK: Tsr="));
      Serial.print(sredniOkres, 4);
      Serial.print(F(" s, g="));
      Serial.print(przyspieszenie, 3);
      Serial.println(F(" m/s^2. To oszacowanie dla malych wychylen."));
    }
  }
  odswiezLCD = true;
}

void sprawdzKontaktron(uint32_t terazMs) {
  bool nowy = digitalRead(KONTAKTRON_PIN);
  if (nowy != odczytStyku) {
    odczytStyku = nowy;
    zmianaStykuMs = terazMs;
    zmianaStykuUs = micros();
  }
  if (odczytStyku != stanStyku && uint32_t(terazMs - zmianaStykuMs) >= DRGANIA_STYKU_MS) {
    stanStyku = odczytStyku;
    digitalWrite(LED_PIN, stanStyku == LOW ? HIGH : LOW);
    if (stanStyku == HIGH) {
      gotowNaZwarcie = true;
    } else if (gotowNaZwarcie) {
      gotowNaZwarcie = false;
      // Zapisujemy poczatek zbocza, ktore utrzymalo sie przez 10 ms.
      // To czas zaobserwowany przez petle, nie sprzetowy pomiar bez bledu.
      obsluzPrzejscie(zmianaStykuUs, terazMs);
    }
  }
}

void sprawdzPrzycisk(uint32_t terazMs) {
  bool nowy = digitalRead(PRZYCISK_PIN);
  if (nowy != odczytPrzycisku) {
    odczytPrzycisku = nowy;
    zmianaPrzyciskuMs = terazMs;
  }
  if (odczytPrzycisku != stanPrzycisku && uint32_t(terazMs - zmianaPrzyciskuMs) >= DRGANIA_PRZYCISKU_MS) {
    stanPrzycisku = odczytPrzycisku;
    if (stanPrzycisku == LOW) {
      if (trwaPomiar()) {
        wyzerujDane();
        stan = GOTOWY;
        odswiezLCD = true;
        Serial.println(F("ANULOWANO. Nacisnij START, aby wykonac nowa probe."));
      } else {
        rozpocznijPomiar(terazMs);
      }
    }
  }
}

void sprawdzLimity(uint32_t terazMs) {
  if (!trwaPomiar()) return;
  if (uint32_t(terazMs - startSesjiMs) >= SESJA_MS) zglosBlad(F("Limit 60s"));
  else if (stan == CZEKANIE && uint32_t(terazMs - startSesjiMs) >= CZEKANIE_MS) zglosBlad(F("Brak impulsu"));
  else if (stan == POMIAR && uint32_t(terazMs - ostatniePrzejscieMs) >= PRZERWA_MS) zglosBlad(F("Przerwa >=5s"));
}

void pustyWiersz(byte wiersz) {
  lcd.setCursor(0, wiersz);
  lcd.print(F("                "));
  lcd.setCursor(0, wiersz);
}

void pokazLCD() {
  pustyWiersz(0);
  pustyWiersz(1);
  lcd.setCursor(0, 0);
  if (stan == GOTOWY) {
    lcd.print(F("L:")); lcd.print(DLUGOSC_M, 3); lcd.print('m');
    lcd.setCursor(0, 1); lcd.print(F("START: przycisk"));
  } else if (stan == CZEKANIE) {
    lcd.print(F("Czekam na ruch"));
    lcd.setCursor(0, 1); lcd.print(F("L:")); lcd.print(DLUGOSC_M, 3); lcd.print(F("m P:0"));
  } else if (stan == POMIAR) {
    lcd.print(F("P:")); lcd.print(przejscia); lcd.print(F("/21 N:")); lcd.print(okresy); lcd.print(F("/10"));
    lcd.setCursor(0, 1); lcd.print(F("T:"));
    if (isnan(ostatniOkres)) lcd.print(F("---"));
    else { lcd.print(ostatniOkres, 3); lcd.print('s'); }
  } else if (stan == WYNIK) {
    lcd.print(F("Tsr:")); lcd.print(sredniOkres, 3); lcd.print('s');
    lcd.setCursor(0, 1); lcd.print(F("g:")); lcd.print(przyspieszenie, 2); lcd.print(F("m/s2"));
  } else {
    lcd.print(F("BLAD pomiaru"));
    lcd.setCursor(0, 1); lcd.print(powodBledu);
  }
  odswiezLCD = false;
}

void setup() {
  pinMode(KONTAKTRON_PIN, INPUT_PULLUP);
  pinMode(PRZYCISK_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  odczytStyku = stanStyku = digitalRead(KONTAKTRON_PIN);
  odczytPrzycisku = stanPrzycisku = digitalRead(PRZYCISK_PIN);
  digitalWrite(LED_PIN, stanStyku == LOW ? HIGH : LOW);
  wyzerujDane();
  stan = GOTOWY;
  odswiezLCD = true;
  gotowNaZwarcie = stanStyku == HIGH;
  Serial.begin(9600);
  Serial.println(F("Laboratorium wahadla. START / ANULUJ: D3."));
  Serial.println(F("LED pokazuje stan kontaktronu. Sprawdz DLUGOSC_M przed pomiarem."));
  lcd.begin(16, 2);
  pokazLCD();
}

void loop() {
  uint32_t terazMs = millis();
  sprawdzLimity(terazMs);
  sprawdzKontaktron(terazMs);
  sprawdzPrzycisk(terazMs);
  if (odswiezLCD) pokazLCD();
}
