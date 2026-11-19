# Arduino — projekty

**Platforma:** klasyczne Nano / zgodny klon z logiką 5 V. Projekty wykonuje się kolejno, wykorzystując ponownie te same części.

## Wybierz projekt

| Projekt | Co zbudujesz i poznasz | Materiały |
|---|---|---|
| **Alarm odległości** | HC-SR04 mierzy odległość. Przy wyniku **poniżej 10 cm** czerwona LED miga, a brzęczyk nadaje przerywany alarm. Pomiar czasu echa i warunki w programie. | [Opis i schematy](projekty/alarm_odleglosci/README.md) · [Kod](projekty/alarm_odleglosci/alarm_odleglosci.ino) |
| **Lampka nocna** | LED płynnie rozjaśnia się po zasłonięciu fotorezystora. Potencjometr reguluje próg. Dzielnik napięcia, wejścia analogowe, PWM i histereza. | [Opis i schemat](projekty/lampka_nocna/README.md) · [Kod](projekty/lampka_nocna/lampka_nocna.ino) |
| **Refleksomierz** | Poczekaj na zielone światło i naciśnij przycisk. Program mierzy czas reakcji w milisekundach oraz wykrywa falstart. Przyciski, losowe oczekiwanie i odmierzanie czasu. | [Opis i schemat](projekty/refleksomierz/README.md) · [Kod](projekty/refleksomierz/refleksomierz.ino) |
| **Zamek szyfrowy** | Cztery przyciski i kod **1 → 3 → 2 → 4**. Zielona LED sygnalizuje otwarcie, czerwona zamknięcie. Tablice, sekwencje i obsługa błędów. | [Opis i schemat](projekty/zamek_szyfrowy/README.md) · [Kod](projekty/zamek_szyfrowy/zamek_szyfrowy.ino) |
| **Nadajnik Morse’a** | Jeden przycisk uruchamia SOS, drugi służy jako ręczny klucz. LED i brzęczyk nadają ten sam rytm. Kodowanie informacji i automat stanów. | [Opis i schematy](projekty/nadajnik_morsea/README.md) · [Kod](projekty/nadajnik_morsea/nadajnik_morsea.ino) |
| **Sygnalizacja świetlna** | Trzy LED przechodzą przez cztery fazy w pętli. Prosty kod: wyjścia cyfrowe, rezystory i opóźnienia `delay()`. | [Opis i schemat](projekty/sygnalizacja_swietlna/README.md) · [Kod](projekty/sygnalizacja_swietlna/sygnalizacja/sygnalizacja.ino) |


//////////////////////////////////
Przykład
//////////////////////////////////

### Cykl sygnalizacji świetlnej

🟢 **5 s** → 🟡 **2 s** → 🔴 **5 s** → 🔴🟡 **2 s** → ponownie 🟢

Pełny cykl trwa nominalnie **14 sekund**.

![Przykład schematu montażowego — sygnalizacja świetlna](projekty/sygnalizacja_swietlna/schemat_montazowy.png)

## Uruchomienie

1. Pobierz całe repozytorium przez **Code → Download ZIP** i rozpakuj je.
2. Wybierz projekt, przeczytaj jego README i przygotuj wymienione części.
3. Przy odłączonym USB zmontuj układ według schematu. W alarmie i nadajniku Morse’a wykonaj też osobny tor L293D i brzęczyka.
4. Otwórz plik `.ino` w Arduino IDE. W projektach z plikiem `Przycisk.h` pozostaw go obok szkicu, w tym samym folderze.
5. Wybierz **Arduino Nano**, właściwy port i procesor zgodny z oznaczeniem na swojej płytce. W starszych klonach może być potrzebna opcja **ATmega328P (Old Bootloader)**. Dodatkowe biblioteki nie są potrzebne.
6. Podłącz USB, wgraj kod i wykonaj próby z instrukcji projektu. W projektach korzystających z Monitora portu szeregowego ustaw **9600 baud**.

## Płytka, zasilanie i schematy

- Zasilanie projektów: **USB Nano, 5 V**. Wszystkie elementy mają wspólną masę GND. Bateria 9 V nie jest potrzebna do tych ćwiczeń.
- Zestaw rozpoznano jako format klasycznego Nano. Przed wyborem procesora odczytaj oznaczenie mikrokontrolera na swoim klonie; sprawdzone kody przygotowano dla **ATmega328P 16 MHz**.
- Każda LED ma osobny rezystor **1 kΩ**. Przyciski łączą wejścia z GND; podciąganie `INPUT_PULLUP` włącza program.
- Rysunki pokazują elementy i kolorowe przewody. Nano jest widoczne z góry, USB u dołu. Dobierz otwory breadboardu według jego rzeczywistych grup styków.
- Alarm i nadajnik Morse’a używają aktywnego brzęczyka oraz L293D. Wymagane napięcie konkretnego brzęczyka trzeba potwierdzić; sterownik powoduje spadek napięcia. Pełny opis i oba schematy są w instrukcjach tych projektów.

## Sprawdzenie kodu

Wszystkie sześć szkiców przeszło kompilację dla **ATmega328P 16 MHz**. W uproszczonej sygnalizacji sprawdzono kompilację i kolejność czterech faz; pozostałe pięć projektów przeszło również testy logiki z symulowanymi wejściami. Nie wykonano prób na fizycznym zestawie. Instrukcje projektów podają oczekiwane zachowanie i próby do wykonania po montażu.

Podstawą parametrów są [dokumentacja klasycznego Nano](https://docs.arduino.cc/hardware/nano/) i [oficjalny pinout](https://docs.arduino.cc/resources/pinouts/A000005-full-pinout.pdf). Źródła dotyczące czujnika, sterownika i pozostałych funkcji podano przy poszczególnych projektach.
