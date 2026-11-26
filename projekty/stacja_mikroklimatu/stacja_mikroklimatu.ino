/*
  Stacja mikroklimatu w klasie — Arduino Uno / klasyczne Nano, 5 V.
  LCD 16x2: RS=D12, E=D11, D4=D5, D5=D4, D6=D3, D7=D2.
  DHT11 DATA=D6 (10 kOhm do 5 V), DS18B20 DQ=D7 (4,7 kOhm do 5 V).
  Przycisk D8--GND, czerwona LED D9--1 kOhm--LED--GND.
  DS18B20 wymaga osobnego VDD=5 V, nie zasilania pasozytniczego.

  Biblioteki: LiquidCrystal, DHT sensor library (+ Adafruit Unified Sensor),
  OneWire, DallasTemperature. Szczegoly montazu i wersje: README.md.
  Serial 9600: temperatura_DHT,wilgotnosc,temperatura_DS (bez naglowka).
  Wiersz powstaje tylko dla kompletu poprawnych, nowych pomiarow.
*/

#include <LiquidCrystal.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <math.h>

const byte DHT_PIN = 6, DS_PIN = 7, PRZYCISK_PIN = 8, LED_PIN = 9;
const unsigned long OKRES_MS = 2000, KONWERSJA_DS_MS = 750;
const unsigned long DRGANIA_MS = 30;
const float PROG_T = 26.0, HISTEREZA_T = 1.0;
const float PROG_H = 70.0, HISTEREZA_H = 5.0;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
DHT dht(DHT_PIN, DHT11);
OneWire oneWire(DS_PIN);
DallasTemperature ds(&oneWire);
DeviceAddress adresDS;

struct Zakres {
  bool jest = false;
  float minimum = 0, maksimum = 0;

  void dodaj(float wartosc) {
    if (!jest || wartosc < minimum) minimum = wartosc;
    if (!jest || wartosc > maksimum) maksimum = wartosc;
    jest = true;
  }
};

Zakres zakresT, zakresH, zakresDS;
float temperaturaDHT = NAN, wilgotnosc = NAN, temperaturaDS = NAN;
bool temperaturaOK = false, wilgotnoscOK = false, dsOK = false;
bool dhtMierzony = false, dsMierzony = false;
bool adresZnany = false, trwaKonwersja = false;
bool alarmT = false, alarmH = false, odswiezLCD = true;
byte widok = 0;
bool ostatniOdczyt = HIGH, stanPrzycisku = HIGH;
unsigned long ostatniCykl = 0, startKonwersji = 0, zmianaPrzycisku = 0;

// Jawna deklaracja ulatwia kompilacje szkicu z typem zdefiniowanym powyzej.
void pokazZakres(const __FlashStringHelper* nazwa, const Zakres& zakres,
                 bool aktualnyPomiarOK, char jednostka);

bool poprawnaLiczba(float wartosc, float minimum, float maksimum) {
  return !isnan(wartosc) && !isinf(wartosc) &&
         wartosc >= minimum && wartosc <= maksimum;
}

void sprawdzPrzycisk(unsigned long teraz) {
  bool odczyt = digitalRead(PRZYCISK_PIN);
  if (odczyt != ostatniOdczyt) {
    ostatniOdczyt = odczyt;
    zmianaPrzycisku = teraz;
  }
  if (odczyt != stanPrzycisku && teraz - zmianaPrzycisku >= DRGANIA_MS) {
    stanPrzycisku = odczyt;
    if (stanPrzycisku == LOW) {
      widok = (widok + 1) % 4;
      odswiezLCD = true;
    }
  }
}

void aktualizujAlarm() {
  // Awaria DHT nie oznacza bezpiecznych warunkow — szczegoly widac na LCD.
  if (!temperaturaOK || !wilgotnoscOK) {
    alarmT = alarmH = false;
  } else {
    if (temperaturaDHT >= PROG_T) alarmT = true;
    if (temperaturaDHT <= PROG_T - HISTEREZA_T) alarmT = false;
    if (wilgotnosc >= PROG_H) alarmH = true;
    if (wilgotnosc <= PROG_H - HISTEREZA_H) alarmH = false;
  }
  digitalWrite(LED_PIN, alarmT || alarmH ? HIGH : LOW);
}

bool znajdzDS() {
  // Szukamy ponownie po bledzie, takze gdy czujnika nie bylo przy starcie.
  oneWire.reset_search();
  while (oneWire.search(adresDS)) {
    if (adresDS[0] == 0x28 && OneWire::crc8(adresDS, 7) == adresDS[7]) {
      if (ds.readPowerSupply(adresDS)) return false;  // zasilanie pasozytnicze
      return ds.setResolution(adresDS, 12);
    }
  }
  return false;
}

void rozpocznijPomiar() {
  wilgotnosc = dht.readHumidity();
  temperaturaDHT = dht.readTemperature();  // ta sama ramka DHT z cache biblioteki
  temperaturaOK = poprawnaLiczba(temperaturaDHT, 0, 50);
  wilgotnoscOK = poprawnaLiczba(wilgotnosc, 0, 100);
  dhtMierzony = true;
  if (temperaturaOK) zakresT.dodaj(temperaturaDHT);
  if (wilgotnoscOK) zakresH.dodaj(wilgotnosc);
  aktualizujAlarm();

  if (!adresZnany) adresZnany = znajdzDS();
  if (adresZnany && ds.requestTemperaturesByAddress(adresDS)) {
    startKonwersji = millis();
    trwaKonwersja = true;
  } else {
    adresZnany = dsOK = false;
    dsMierzony = true;
  }
  odswiezLCD = true;
}

void zakonczPomiarDS() {
  trwaKonwersja = false;
  dsMierzony = true;
  // Odczyt nastapi dopiero po pelnych 750 ms i zakonczeniu konwersji.
  temperaturaDS = ds.isConversionComplete() ? ds.getTempC(adresDS) : NAN;
  dsOK = poprawnaLiczba(temperaturaDS, -55, 125);
  if (dsOK) {
    zakresDS.dodaj(temperaturaDS);
    // Nie odrzucamy samej liczby 85: moze byc prawdziwa temperatura.
    if (temperaturaOK && wilgotnoscOK) {
      Serial.print(temperaturaDHT, 1);
      Serial.print(',');
      Serial.print(wilgotnosc, 1);
      Serial.print(',');
      Serial.println(temperaturaDS, 1);
    }
  } else {
    adresZnany = false;
  }
  odswiezLCD = true;
}

void pustyWiersz(byte wiersz) {
  lcd.setCursor(0, wiersz);
  lcd.print(F("                "));
  lcd.setCursor(0, wiersz);
}

void pokazZakres(const __FlashStringHelper* nazwa, const Zakres& zakres,
                 bool aktualnyPomiarOK, char jednostka) {
  for (byte wiersz = 0; wiersz < 2; wiersz++) {
    pustyWiersz(wiersz);
    lcd.print(nazwa);
    lcd.print(wiersz == 0 ? F(" min:") : F(" max:"));
    if (zakres.jest) {
      lcd.print(wiersz == 0 ? zakres.minimum : zakres.maksimum, 1);
      lcd.print(jednostka);
    } else {
      lcd.print(F("---"));
    }
    if (!aktualnyPomiarOK) {
      lcd.setCursor(15, wiersz);
      lcd.print('!');  // brak aktualnego poprawnego pomiaru; extrema sa historyczne
    }
  }
}

void pokazLCD() {
  if (widok == 1) {
    pokazZakres(F("T"), zakresT, temperaturaOK, 'C');
  } else if (widok == 2) {
    pokazZakres(F("H"), zakresH, wilgotnoscOK, '%');
  } else if (widok == 3) {
    pokazZakres(F("DS"), zakresDS, dsOK, 'C');
  } else {
    pustyWiersz(0);
    if (!dhtMierzony) {
      lcd.print(F("DHT: czekam"));
    } else {
      lcd.print(F("T:"));
      if (temperaturaOK) lcd.print(temperaturaDHT, 1);
      else lcd.print(F("ERR"));
      lcd.print(F("C H:"));
      if (wilgotnoscOK) lcd.print(wilgotnosc, 0);
      else lcd.print(F("ERR"));
      lcd.print('%');
    }
    pustyWiersz(1);
    lcd.print(F("DS:"));
    if (dsOK) {
      lcd.print(temperaturaDS, 1);
      lcd.print('C');
    } else if (!dsMierzony) {
      lcd.print(trwaKonwersja ? F("pomiar") : F("czekam"));
    } else {
      lcd.print(F("ERR"));
    }
    lcd.print(F(" AL:"));
    lcd.print(temperaturaOK && wilgotnoscOK ? (alarmT || alarmH ? '1' : '0') : '?');
  }
  odswiezLCD = false;
}

void setup() {
  pinMode(PRZYCISK_PIN, INPUT_PULLUP);
  ostatniOdczyt = stanPrzycisku = digitalRead(PRZYCISK_PIN);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.begin(9600);
  lcd.begin(16, 2);
  dht.begin();
  ds.begin();
  ds.setAutoSaveScratchPad(false);  // zmiana rozdzielczosci tylko w RAM czujnika
  ds.setWaitForConversion(false);  // nie czekaj 750 ms wewnatrz funkcji biblioteki
  ostatniCykl = millis();          // pierwsze odczyty po 2 s od inicjalizacji
  pokazLCD();
}

void loop() {
  unsigned long teraz = millis();
  sprawdzPrzycisk(teraz);
  if (trwaKonwersja && teraz - startKonwersji >= KONWERSJA_DS_MS) {
    zakonczPomiarDS();
  }
  if (!trwaKonwersja && teraz - ostatniCykl >= OKRES_MS) {
    ostatniCykl = teraz;
    rozpocznijPomiar();
  }
  if (odswiezLCD) pokazLCD();
}
