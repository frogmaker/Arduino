/*
  Mini studio muzyczne — sekwencer osmiu krokow.
  Arduino Uno R3 / klasyczne Nano ATmega328P, 5 V.

  Klawiatura: R1..R4 = D2,D3,D4,D5; C1..C4 = D6,D7,D8,D9.
  Ustal rzedy/kolumny swojego zlacza — kolejnosc pinow nie jest uniwersalna.
  Piezo BEZ generatora: D10 -- 1 kOhm -- piezo -- GND.
  WS2812 x8: DIN = D11 przez 330 Ohm, zasilanie 5 V/GND,
  kondensator 100 uF przy listwie (plus do 5 V); DO niepodlaczone.
  Potencjometr: suwak A0, skrajne konce 5 V i GND.

  Biblioteki: Keypad, Adafruit NeoPixel. Szczegoly: README.md.
  1..8 = C6,D6,E6,F6,G6,A6,B6,C7; 0 = pauza; * = cofnij;
  A = graj od poczatku w petli; B = stop; C = wyczysc.
  9, # i D nie maja funkcji. Program przechowuje 1..8 krokow w RAM.
*/

#include <Arduino.h>
#include <Keypad.h>
#include <Adafruit_NeoPixel.h>

const byte LICZBA_KROKOW = 8;
const byte PIEZO_PIN = 10, LED_PIN = 11, TEMPO_PIN = A0;
const unsigned int CZESTOTLIWOSCI[8] = {
  1047, 1175, 1319, 1397, 1568, 1760, 1976, 2093
};
const char* const NAZWY_NUT[8] = {
  "C6", "D6", "E6", "F6", "G6", "A6", "B6", "C7"
};

char klawisze[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte pinyRzedow[4] = {2, 3, 4, 5};
byte pinyKolumn[4] = {6, 7, 8, 9};
Keypad klawiatura(makeKeymap(klawisze), pinyRzedow, pinyKolumn, 4, 4);
Adafruit_NeoPixel listwa(LICZBA_KROKOW, LED_PIN, NEO_GRB + NEO_KHZ800);

byte sekwencja[LICZBA_KROKOW] = {};  // 0 = pauza, 1..8 = numer nuty
byte liczbaKrokow = 0, aktualnyKrok = 0;
bool gra = false, dzwiekTrwa = false;
unsigned int tempoBPM = 60;
unsigned long poczatekKroku = 0, czasKroku = 1000, czasDzwieku = 800;

unsigned int odczytajTempo() {
  // Jeden krok trwa jedna cwiercnute. 0..1023 daje 60..240 BPM.
  return 60 + (unsigned long)analogRead(TEMPO_PIN) * 180 / 1023;
}

void wypiszSekwencje() {
  Serial.print(F("Program ("));
  Serial.print(liczbaKrokow);
  Serial.print(F("/8): ["));
  for (byte i = 0; i < LICZBA_KROKOW; i++) {
    if (i > 0) Serial.print(' ');
    if (i >= liczbaKrokow) Serial.print('_');
    else if (sekwencja[i] == 0) Serial.print(F("pauza"));
    else Serial.print(NAZWY_NUT[sekwencja[i] - 1]);
  }
  Serial.println(']');
}

void pokazKrok(byte numer, byte nuta) {
  // Transfer do WS2812 wykonujemy, kiedy piezo juz nie generuje tonu.
  noTone(PIEZO_PIN);
  dzwiekTrwa = false;
  listwa.clear();
  if (numer < LICZBA_KROKOW) {
    uint32_t kolor = nuta == 0 ? listwa.Color(0, 0, 255)
                               : listwa.Color(0, 255, 0);
    listwa.setPixelColor(numer, kolor);  // tylko jeden swiecacy piksel
  }
  listwa.show();
}

void zatrzymaj() {
  gra = false;
  pokazKrok(LICZBA_KROKOW, 0);  // numer poza listwa oznacza wszystkie LED zgaszone
}

void rozpocznijKrok() {
  tempoBPM = odczytajTempo();
  czasKroku = 60000UL / tempoBPM;
  czasDzwieku = czasKroku * 4 / 5;  // ostatnie 20% kroku to przerwa
  byte nuta = sekwencja[aktualnyKrok];
  pokazKrok(aktualnyKrok, nuta);
  poczatekKroku = millis();
  if (nuta != 0) {
    tone(PIEZO_PIN, CZESTOTLIWOSCI[nuta - 1], czasDzwieku);
    dzwiekTrwa = true;
  }
}

void obsluzKlawisz(char klawisz) {
  if (klawisz == 'B') {
    zatrzymaj();
    Serial.println(F("STOP. Mozesz edytowac sekwencje."));
    return;
  }
  if (klawisz == 'A') {
    if (liczbaKrokow == 0) {
      Serial.println(F("Program jest pusty. Wpisz nute 1..8 lub pauze 0."));
      return;
    }
    // Komunikaty wypisujemy przed startem odmierzania pierwszego kroku.
    zatrzymaj();
    Serial.print(F("GRAM W PETLI. Tempo na starcie: "));
    Serial.print(odczytajTempo());
    Serial.println(F(" BPM. Pokretlo zmienia tempo od nastepnego kroku."));
    wypiszSekwencje();
    aktualnyKrok = 0;
    gra = true;
    rozpocznijKrok();
    return;
  }
  bool polecenieEdycji = (klawisz >= '0' && klawisz <= '8') ||
                         klawisz == '*' || klawisz == 'C';
  if (!polecenieEdycji) {
    Serial.println(F("Ten klawisz nie ma funkcji (9, #, D)."));
    return;
  }
  if (gra) {
    Serial.println(F("Najpierw nacisnij B (STOP), potem edytuj program."));
    return;
  }
  if (klawisz == 'C') {
    zatrzymaj();
    for (byte i = 0; i < LICZBA_KROKOW; i++) sekwencja[i] = 0;
    liczbaKrokow = 0;
    aktualnyKrok = 0;
    Serial.println(F("Program wyczyszczony."));
  } else if (klawisz == '*') {
    if (liczbaKrokow > 0) {
      liczbaKrokow--;
      sekwencja[liczbaKrokow] = 0;
      pokazKrok(liczbaKrokow > 0 ? liczbaKrokow - 1 : LICZBA_KROKOW,
                liczbaKrokow > 0 ? sekwencja[liczbaKrokow - 1] : 0);
    } else {
      Serial.println(F("Nie ma kroku do usuniecia."));
    }
  } else {
    if (liczbaKrokow == LICZBA_KROKOW) {
      Serial.println(F("Pamiec pelna: 8 krokow. Uzyj * lub C."));
      return;
    }
    byte nuta = klawisz - '0';
    sekwencja[liczbaKrokow] = nuta;
    liczbaKrokow++;
    wypiszSekwencje();
    pokazKrok(liczbaKrokow - 1, nuta);
    if (nuta != 0) tone(PIEZO_PIN, CZESTOTLIWOSCI[nuta - 1], 150);
    return;
  }
  wypiszSekwencje();
}

void setup() {
  pinMode(PIEZO_PIN, OUTPUT);
  noTone(PIEZO_PIN);
  Serial.begin(9600);
  klawiatura.setDebounceTime(30);
  listwa.begin();
  listwa.setBrightness(16);  // mala jasnosc; maksymalnie jeden aktywny LED
  pokazKrok(LICZBA_KROKOW, 0);
  Serial.println(F("SEKWENCER 8 KROKOW — C6 D6 E6 F6 G6 A6 B6 C7"));
  Serial.println(F("1..8: nuta, 0: pauza, *: cofnij, A: graj, B: stop, C: wyczysc."));
  Serial.println(F("9/#/D: bez funkcji. Tempo: pokretlo 60..240 BPM."));
  Serial.println(F("Jeden klawisz naraz. Po wylaczeniu zasilania program znika."));
  wypiszSekwencje();
}

void loop() {
  // getKey() oddaje klawisz po nowym nacisnieciu, nie powtarza go przy trzymaniu.
  char klawisz = klawiatura.getKey();
  if (klawisz) obsluzKlawisz(klawisz);
  if (!gra) return;

  unsigned long teraz = millis();
  if (dzwiekTrwa && teraz - poczatekKroku >= czasDzwieku) {
    noTone(PIEZO_PIN);
    dzwiekTrwa = false;
  }
  if (teraz - poczatekKroku >= czasKroku) {
    aktualnyKrok = (aktualnyKrok + 1) % liczbaKrokow;
    // Po dlugim zatrzymaniu wykonaj jeden krok, bez lawiny nadrabiania.
    rozpocznijKrok();
  }
}
