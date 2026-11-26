# Karta pracy — Laboratorium wahadła

Imię / grupa: .....................................  Data: .........................

[Instrukcja i schematy](README.md) · [Kod](laboratorium_wahadla.ino)

## 1. Przygotuj doświadczenie

Zamocuj magnes na ciężarku i nieruchomy kontaktron obok toru ruchu. Sprawdź LED: jedno przejście powinno dawać jedno zwarcie. Zmierz L od punktu zawieszenia do środka masy ciężarka wraz z magnesem, wpisz `DLUGOSC_M` w metrach i wgraj program.

L = ........................ m. Początkowe wychylenie = ........................ °.

Dlaczego nie wystarczy zmierzyć długości samego sznurka?

...........................................................................................................

## 2. Policz pełne okresy

Zaznacz na rysunku toru ruchu, dlaczego dwa przejścia przez środek odpowiadają pełnemu okresowi. Program liczy czas od pierwszego do 21. przejścia.

Ile okresów upływa od pierwszego do trzeciego przejścia? ........................

Ile od pierwszego do 21.? ........................

Dlaczego sam odstęp między pierwszym i drugim przejściem nie jest pełnym okresem?

...........................................................................................................

## 3. Zbadaj powtarzalność

Wykonaj trzy próby przy tej samej długości i małym wychyleniu. Każda próba obejmuje dziesięć pełnych okresów. Tabela jest przeznaczona na Twoje rzeczywiste wyniki.

| Próba | L [m] | Średni okres z próby [s] | Obliczone g [m/s²] | Obserwacje |
|---:|---:|---:|---:|---|
| 1 | | | | |
| 2 | | | | |
| 3 | | | | |

Oblicz średnią z trzech uzyskanych okresów oraz rozstęp: największy wynik minus najmniejszy. Co może odpowiadać za różnice?

...........................................................................................................

## 4. Zmień długość

Zbadaj trzy różne długości w zakresie 0,10–2,00 m. Za każdym razem zmierz nowe L, zmień stałą i ponownie wgraj kod. Zachowaj podobne małe wychylenie i ten sam ciężarek.

| L [m] | T średni [s] | T² [s²] | g = 4π²L/T² [m/s²] |
|---:|---:|---:|---:|
| | | | |
| | | | |
| | | | |

Sporządź wykres T² w zależności od L. Czy punkty układają się w przybliżeniu na prostej? Jak zmienia się okres po zwiększeniu długości?

...........................................................................................................

## 5. Oceń jakość pomiaru

Wskaż przynajmniej trzy źródła błędu. Uwzględnij ustawienie kontaktronu, brakujące lub podwójne impulsy, pomiar L, wychylenie i opory ruchu. Oddziel liczbę miejsc po przecinku od rzeczywistej dokładności.

...........................................................................................................

Wyjaśnij, dlaczego dla tej samej długości dwukrotnie za duży zmierzony okres daje czterokrotnie za małe obliczone g.

...........................................................................................................

## 6. Sprawdź obsługę przerwania próby

Uruchom próbę bez ruchu, a następnie sprawdź anulowanie przyciskiem i ponowny START. Zapisz komunikaty. Nie zmieniaj połączeń pod napięciem.

...........................................................................................................
