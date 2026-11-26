#include <LiquidCrystal.h>

// ============================================================
// LCD 1602
// ============================================================

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// ============================================================
// TEKSTY
// ============================================================

const char napis1[] = "BIOINFORMATYKA   bioinformatyka";
const char napis2[] = "druk3D arduino";

// ============================================================
// USTAWIENIA
// ============================================================

const int LCD_WIDTH = 16;

// Odstęp pomiędzy kolejnymi kopiami tekstu

const int ODSTEP = 3;

// Czas pomiędzy kolejnymi przesunięciami

const unsigned long SPEED = 500;

// ============================================================
// POZYCJE
// ============================================================

int pozycjaGorna = 0;

// Drugi napis zaczyna się 8 znaków dalej

int pozycjaDolna = 8;

// ============================================================
// CZAS
// ============================================================

unsigned long ostatniRuch = 0;

// ============================================================
// DŁUGOŚĆ TEKSTÓW
// ============================================================

const int DLUGOSC1 = sizeof(napis1) - 1;
const int DLUGOSC2 = sizeof(napis2) - 1;

// ============================================================
// RYSOWANIE TEKSTU W PĘTLI
// ============================================================

void pokazNapis(
    int wiersz,
    int pozycja,
    const char tekst[],
    int dlugosc)
{
  // Odległość od początku jednej kopii
  // do początku następnej.

  int krok = dlugosc + ODSTEP;

  // ----------------------------------------------------------
  // Rysujemy kilka kopii tekstu.
  //
  // Dzięki temu gdy jedna kopia wychodzi z lewej,
  // następna już znajduje się po prawej.
  // ----------------------------------------------------------

  for (int kopia = -2; kopia < 4; kopia++)
  {
    int start = pozycja + kopia * krok;

    for (int i = 0; i < dlugosc; i++)
    {
      int kolumna = start + i;

      if (kolumna >= 0 && kolumna < LCD_WIDTH)
      {
        lcd.setCursor(kolumna, wiersz);
        lcd.print(tekst[i]);
      }
    }
  }
}

// ============================================================
// CZYSZCZENIE WIERSZA
// ============================================================

void wyczyscWiersz(int wiersz)
{
  lcd.setCursor(0, wiersz);

  for (int i = 0; i < LCD_WIDTH; i++)
  {
    lcd.print(' ');
  }
}

// ============================================================
// SETUP
// ============================================================

void setup()
{
  lcd.begin(16, 2);

  lcd.clear();

  pokazNapis(
      0,
      pozycjaGorna,
      napis1,
      DLUGOSC1);

  pokazNapis(
      1,
      pozycjaDolna,
      napis2,
      DLUGOSC2);
}

// ============================================================
// LOOP
// ============================================================

void loop()
{
  unsigned long teraz = millis();

  // ----------------------------------------------------------
  // Przesuwamy dopiero po upływie SPEED
  // ----------------------------------------------------------

  if (teraz - ostatniRuch >= SPEED)
  {
    ostatniRuch = teraz;

    // --------------------------------------------------------
    // GÓRNY NAPIS
    // --------------------------------------------------------

    pozycjaGorna--;

    // Gdy przesunięcie osiągnie pełny krok,
    // cofamy pozycję o dokładnie jeden krok.
    //
    // Dzięki temu nie ma żadnego "resetu".

    int krok1 = DLUGOSC1 + ODSTEP;

    if (pozycjaGorna <= -krok1)
    {
      pozycjaGorna += krok1;
    }

    // --------------------------------------------------------
    // DOLNY NAPIS
    // --------------------------------------------------------

    pozycjaDolna--;

    int krok2 = DLUGOSC2 + ODSTEP;

    if (pozycjaDolna <= -krok2)
    {
      pozycjaDolna += krok2;
    }

    // --------------------------------------------------------
    // Czyścimy tylko zawartość obu wierszy
    // --------------------------------------------------------

    wyczyscWiersz(0);
    wyczyscWiersz(1);

    // --------------------------------------------------------
    // Rysujemy nowe pozycje
    // --------------------------------------------------------

    pokazNapis(
        0,
        pozycjaGorna,
        napis1,
        DLUGOSC1);

    pokazNapis(
        1,
        pozycjaDolna,
        napis2,
        DLUGOSC2);
  }
}
