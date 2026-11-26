# Karta pracy — Stacja mikroklimatu w klasie

Imię / grupa: ....................................  Data: ........................

[Instrukcja i schematy](README.md) · [Kod](stacja_mikroklimatu.ino)

## 1. Uruchom pomiar

Zmontuj układ przy odłączonym USB, uruchom go według instrukcji i sprawdź cztery widoki LCD. Zapisz własne wyniki — tabela jest celowo pusta.

| Obserwacja | Temperatura DHT [°C] | Wilgotność [% RH] | Temperatura DS [°C] |
|---|---:|---:|---:|
| Po ustaleniu temperatury | | | |
| Po 30 s | | | |
| Po 60 s | | | |
| Po 90 s | | | |

Który czujnik reaguje szybciej na delikatne ogrzanie dłonią jego obudowy? Nie dotykaj przewodzących nóżek ani nie zwieraj połączeń. Po ogrzaniu poczekaj na powrót do temperatury otoczenia.

Odpowiedź i obserwacje: ........................................................................

## 2. Porównaj dwa termometry

Umieść DHT11 i DS18B20 obok siebie, z dala od ciepłych elementów układu. Po ustaleniu temperatury zbierz pięć par wyników.

| Próba | T z DHT [°C] | T z DS [°C] | Różnica DHT − DS [°C] |
|---:|---:|---:|---:|
| 1 | | | |
| 2 | | | |
| 3 | | | |
| 4 | | | |
| 5 | | | |

Oblicz średnią różnicę. Czy wyniki muszą być identyczne? Wyjaśnij, dlaczego rozdzielczość i dokładność są różnymi pojęciami. Uwzględnij dane producentów przytoczone w README.

Wnioski: ..............................................................................................

## 3. Sprawdź histerezę

Domyślne ustawienia: temperatura włącza swoją przyczynę alarmu przy 26°C, wyłącza przy 25°C. Wilgotność włącza swoją przyczynę przy 70% RH, wyłącza przy 65% RH. LED świeci, jeśli aktywna jest przynajmniej jedna z nich.

Zakładając poprawne pomiary i początkowo wyłączony alarm, uzupełnij przewidywane stany. To dane do analizy programu, a nie zapis doświadczenia.

| Kolejny krok | T DHT [°C] | H [% RH] | LED: 0 czy 1? |
|---:|---:|---:|---:|
| 1 | 24 | 50 | |
| 2 | 26 | 50 | |
| 3 | 25,5 | 50 | |
| 4 | 25 | 50 | |
| 5 | 24 | 70 | |
| 6 | 24 | 67 | |
| 7 | 24 | 65 | |

W praktycznej próbie zmień tymczasowo `PROG_T` tak, aby móc przejść przez próg przy temperaturze w sali. Zapisz wybraną wartość, obserwacje i przywróć domyślne ustawienie po ćwiczeniu. Nie przegrzewaj czujników, aby wymusić odczyt.

Po co zastosowano dwa różne progi włączenia i wyłączenia?

...........................................................................................................

## 4. Odczytaj dane szeregowe

Otwórz Monitor portu szeregowego, **9600 baud**. Wskaż, co oznacza każda z trzech liczb w wierszu. Następnie zamknij Monitor i uruchom Kreślarz portu szeregowego z tą samą prędkością.

Dlaczego podczas błędu czujnika program nie powinien wpisywać zera zamiast pomiaru?

...........................................................................................................

Dlaczego pięciosekundowy wykres przy pomiarze co dwie sekundy zawiera tylko kilka punktów? Jakie okno obserwacji wybrałbyś do badania temperatury sali przez całą lekcję?

...........................................................................................................

## 5. Sprawdź brak czujnika

Wyłącz zasilanie. Odłącz jeden czujnik i włącz układ ponownie. Zapisz, co pokazują LCD oraz Serial i które statystyki nadal się aktualizują. Wyłącz zasilanie, przywróć połączenie i uruchom ponownie.

...........................................................................................................

Dlaczego zgaszona LED przy komunikacie `AL:?` nie oznacza potwierdzenia dobrych warunków?

...........................................................................................................
