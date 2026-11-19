# Sygnalizacja świetlna w pętli

**Poziom:** podstawowy · **Czas orientacyjny:** 30–45 min · **Płytka:** klasyczne Nano / zgodny klon z logiką 5 V.

Zbuduj sygnalizator z trzech LED. Powtarzana kolejność: zielone 5 s → żółte 2 s → czerwone 5 s → czerwone razem z żółtym 2 s → ponownie zielone. Cały cykl trwa nominalnie 14 sekund.

## Elementy z zestawu

- Nano 5 V, USB, płytka stykowa i przewody
- LED: zielona, żółta i czerwona — po 1 szt.
- rezystory 1 kΩ — 3 szt., osobny do każdej LED

Projekty wykonujesz kolejno, wykorzystując ponownie tę samą płytkę i części. Przewód USB oraz komputer to stanowisko do zasilania i wgrywania kodu.

## Schemat i połączenia

[Otwórz schemat w pełnej rozdzielczości](schemat_montazowy.png).

![Schemat montażowy](schemat_montazowy.png)

Nano pokazano z góry, z USB u dołu i kolejnością pinów klasycznego Nano. Połączenia wykonuj według oznaczeń na własnej płytce. To widok elementów i przewodów; grupy otworów płytki stykowej trzeba dobrać samodzielnie.

**Czerwony przewód: +5V. Czarny: GND. Pozostałe kolory: sygnały.** Kropka oznacza połączenie, a przerwa przy skrzyżowaniu — brak połączenia. A to anoda LED, K — katoda. Każda LED ma własny rezystor 1 kΩ.

| Od | Do | Uwagi |
|---|---|---|
| Nano D7 | R_Z 1 kΩ → anoda zielonej LED | Światło zielone |
| Katoda zielonej LED | Nano GND | Wspólna masa |
| Nano D8 | R_C 1 kΩ → anoda czerwonej LED | Światło czerwone |
| Katoda czerwonej LED | Nano GND | Wspólna masa |
| Nano D9 | R_Y 1 kΩ → anoda żółtej LED | Światło żółte |
| Katoda żółtej LED | Nano GND | Wspólna masa |

## Montaż i uruchomienie

Przepinaj elementy przy odłączonym USB. Obie nóżki LED muszą należeć do różnych węzłów; każda LED ma własny rezystor. Sprawdź, czy szyny płytki stykowej są ciągłe, i w razie potrzeby połącz ich segmenty.

1. Ustaw trzy LED w kolejności fizycznej: u góry czerwona, pośrodku żółta, na dole zielona. Każda nóżka musi należeć do właściwego, oddzielnego węzła płytki stykowej.
2. Połącz każdą anodę z jej pinem Nano przez osobny rezystor 1 kΩ. Połącz wszystkie katody z GND.
3. Wgraj [sygnalizacja.ino](sygnalizacja/sygnalizacja.ino). Dodatkowe biblioteki nie są potrzebne. Po uruchomieniu jako pierwsza świeci zielona LED.
4. Obserwuj kilka pełnych cykli i porównaj czasy świecenia z tabelą poniżej.

Otwórz [sygnalizacja.ino](sygnalizacja/sygnalizacja.ino) z podfolderu `sygnalizacja` w Arduino IDE. Pozostaw szkic w tym folderze, zgodnie z nazwą pliku. Wybór płytki i portu opisano w [instrukcji uruchomienia](../../README.md#uruchomienie).

## Sprawdzenie na stanowisku

Poniższe testy należy wykonać na złożonym układzie. Oczekiwane wyniki nie oznaczają, że zestaw ze zdjęcia został fizycznie przetestowany.

| Próba | Oczekiwany wynik |
|---|---|
| Od startu do 5 s | Świeci tylko zielona LED. |
| Od 5 s do 7 s | Świeci tylko żółta LED. |
| Od 7 s do 12 s | Świeci tylko czerwona LED. |
| Od 12 s do 14 s | Świecą równocześnie czerwona i żółta LED, a zielona jest wyłączona. |
| Po 14 s i w kolejnych cyklach | Wraca zielona LED na 5 s; sekwencja powtarza się bez naciskania przycisku. |

## Co wyjaśnić uczniom

Wyjścia cyfrowe, rezystor szeregowy i cztery fazy sygnalizacji zapisane kolejno w programie.

`setup()` wykonuje się raz i ustawia piny D7, D8 oraz D9 jako wyjścia przez `pinMode()`. Następnie `loop()` powtarza sekwencję. `digitalWrite(pin, HIGH)` zapala LED, a `digitalWrite(pin, LOW)` ją gasi. Funkcja `delay()` czeka przez podaną liczbę milisekund: `delay(5000)` to 5 sekund, a `delay(2000)` to 2 sekundy.

Po resecie świeci zielona. Następnie program przechodzi przez żółtą, czerwoną i czerwoną z żółtą. Na końcu gasi żółtą i czerwoną, po czym zaczyna kolejny cykl od zielonej. Podczas `delay()` program czeka, więc ten prosty szkic zajmuje się wyłącznie sekwencją świateł. Jest to szkolny model; rzeczywiste czasy zależą także od zegara płytki i czasu wykonania poleceń.

## Zadania dodatkowe

- Zmień wartości w `delay(5000)` i `delay(2000)` i oblicz nową długość pełnego cyklu.
- Wyjaśnij, dlaczego faza czerwona+żółta ma dwie zapalone LED, a nie dwa kolejne pojedyncze światła.

## Gdy coś nie działa

- Sprawdź zgodność modelu i logiki 5 V oraz wybraną płytkę i port w IDE.
- Sprawdź wspólną masę i rzeczywiste grupy po pięć otworów w breadboardzie.
- Sprawdź polaryzację LED, wartość rezystora i zgodność numeru pinu z kodem.

## Weryfikacja i źródła

Uproszczony kod przeszedł kompilację i linkowanie dla Nano / ATmega328P 16 MHz z Arduino AVR Boards 1.8.8, bez ostrzeżeń szkicu. Sprawdzono kolejność poleceń i czasy czterech faz. Nie wykonano prób na fizycznym zestawie. Próby stanowiskowe powyżej wykonaj po złożeniu układu.

[Arduino delay()](https://docs.arduino.cc/language-reference/en/functions/time/delay/) · [Arduino Blink](https://docs.arduino.cc/built-in-examples/basics/Blink/)

[← Wszystkie projekty](../../README.md)
