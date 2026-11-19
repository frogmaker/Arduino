// D7: zielona, D8: czerwona, D9: żółta. Każda LED przez 1 kΩ; katody do GND.
void setup()
{
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
}
void loop()
{
  digitalWrite(7, HIGH);
  delay(5000); // zielona: 5 s
  digitalWrite(7, LOW);
  digitalWrite(9, HIGH);
  delay(2000); // żółta: 2 s
  digitalWrite(9, LOW);
  digitalWrite(8, HIGH);
  delay(5000); // czerwona: 5 s
  digitalWrite(9, HIGH);
  delay(2000); // czerwona + żółta: 2 s
  digitalWrite(9, LOW);
  digitalWrite(8, LOW); // powrót do początku
}
