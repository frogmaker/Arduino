# Karta pracy — Mini studio muzyczne

Imię / grupa: .....................................  Data: .........................

[Instrukcja i schematy](README.md) · [Kod](sekwencer_muzyczny.ino)

## 1. Zaprogramuj własną melodię

Po uruchomieniu sprawdź klawisze w Monitorze portu szeregowego, 9600 baud. Wpisz osiem kroków, w tym przynajmniej dwie pauzy. Użyj A, aby odtwarzać, i B, aby zatrzymać.

| Krok | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
|---|---|---|---|---|---|---|---|---|
| Klawisz 1–8 lub 0 | | | | | | | | |
| Nuta albo pauza | | | | | | | | |

Jak pauza zmienia rytm? Co zobaczysz na listwie podczas pauzy?

...........................................................................................................

## 2. Oblicz czas

Jeden krok to ćwierćnuta. Korzystaj ze wzoru `czas kroku [ms] = 60000 / BPM`.

| Tempo [BPM] | Czas kroku [ms] | Czas dźwięku, około 80% [ms] | Czas pętli ośmiu kroków [s] |
|---:|---:|---:|---:|
| 60 | | | |
| 120 | | | |
| 240 | | | |

Sprawdź skrajne ustawienia potencjometru. Zmiana ustawienia w trakcie kroku obowiązuje od następnego — wyjaśnij, dlaczego pomaga to zachować jego długość.

...........................................................................................................

## 3. Częstotliwość i wysokość dźwięku

Porównaj klawisze 1 i 8. W kodzie odpowiadają im 1047 Hz i 2093 Hz. Oblicz ich przybliżony stosunek. Jaki interwał muzyczny łączy C6 i C7?

...........................................................................................................

Czy wszystkie nuty brzmią równie głośno? Oddziel zmianę częstotliwości od zmiany głośności; uwzględnij właściwości piezo.

...........................................................................................................

## 4. Sprawdź pamięć i sterowanie

Wykonaj: pusty program + A; osiem kroków + próba dziewiątego; przytrzymanie klawisza; B i *; C; ponowny start; restart Arduino. Zapisz zachowanie i porównaj z instrukcją.

...........................................................................................................

Dlaczego po restarcie sekwencja znika? Co przechowuje tablica `sekwencja`?

...........................................................................................................

## 5. Rozszerzenie programowe

Zmień udział dźwięku z 80% na 50%, zachowując czas całego kroku. Sprawdź ten sam program przy tym samym tempie. Opisz różnicę w brzmieniu, a następnie przywróć ustawienie.

...........................................................................................................
