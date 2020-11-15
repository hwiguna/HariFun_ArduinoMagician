/*
   Wiring Test
   by Hari Wiguna, 2020
   Segments on each digit should light up one at a time (decimal point excluded).
*/

const byte segmentPins[] = {2, 3, 4, 5, 6, 7, 8};
const byte digitPins[] = {12, 13};
const byte OFF = HIGH;
const byte ON = LOW;

void setup() {
  Serial.begin(9600);

  for (byte i = 0; i < 7; i++) {
    pinMode(segmentPins[i], OUTPUT);
    digitalWrite(segmentPins[i], OFF);
  }
  for (byte i = 0; i < 2; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], ON);
  }
}

void TestSegments() {
  for (byte d = 0; d < 2; d++) {
    digitalWrite(digitPins[d], HIGH); // common anode on
    for (byte i = 0; i < 7; i++) {
      digitalWrite(segmentPins[i], ON);
      delay(150);
      digitalWrite(segmentPins[i], OFF);
      delay(150);
    }
    digitalWrite(digitPins[d], LOW); // common anode off
  }
}

void loop() {
  TestSegments();
}
