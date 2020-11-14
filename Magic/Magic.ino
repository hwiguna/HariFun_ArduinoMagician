/*
 * Pick a number betweeen 1 and 99
 * by Hari Wiguna, 2020
 */

const byte segmentPins[] = {2, 3, 4, 5, 6, 7, 8};
const byte digitPins[] = {12, 13};
const byte maxNumber = 99; // Max # that could be displayed with two digits
const byte buttonPins[] = {9, 10, 11};
const byte OFF = HIGH;
const byte ON = LOW;
const byte PRESSED = LOW;

const byte numberMap[] = {
  //.GFEDCBA
  B00111111,  // 0
  B00000110,  // 1
  B01011011,  // 2
  B01001111,  // 3
  B01100110,  // 4
  B01101101,  // 5
  B01111101,  // 6
  B00000111,  // 7
  B01111111,  // 8
  B01101111,  // 9
  B01000000,  // -
  //B01010011,  // ?
};

byte currentNumber = 13;
byte currentDigitIndex = 0;

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
  for (byte i = 0; i < 3; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

void TestSegments() {
  digitalWrite(digitPins[0], ON); digitalWrite(digitPins[1], OFF);
  for (byte i = 0; i < 7; i++) {
    digitalWrite(segmentPins[i], OFF);
    delay(100);
    digitalWrite(segmentPins[i], ON);
    delay(100);
  }
}

void TestDigits() {
  digitalWrite(digitPins[0], ON); digitalWrite(digitPins[1], OFF); delay(500);
  digitalWrite(digitPins[0], OFF); digitalWrite(digitPins[1], ON); delay(500);
}

void TestButtons() {
  digitalWrite(segmentPins[6], ON);
  for (byte i = 0; i < 2; i++) {
    if (digitalRead(buttonPins[0]) == PRESSED) {
      digitalWrite(digitPins[0], ON); digitalWrite(digitPins[1], OFF);
    }

    if (digitalRead(buttonPins[1]) == PRESSED) {
      digitalWrite(digitPins[0], OFF); digitalWrite(digitPins[1], ON);
    }

    if (digitalRead(buttonPins[2]) == PRESSED) {
      digitalWrite(digitPins[0], OFF); digitalWrite(digitPins[1], OFF);
    }
  }
}

void TestNumbers() {
  digitalWrite(digitPins[0], ON); digitalWrite(digitPins[1], OFF);
  for (byte ni = 0; ni < 10; ni++) {
    for (byte i = 0; i < 7; i++) {
      digitalWrite(segmentPins[i], !bitRead(numberMap[ni], i));
    }
    delay(500);
  }
}

void Refresh() {
  digitalWrite(digitPins[currentDigitIndex], OFF); // Turn off last digit index
  currentDigitIndex = 1 - currentDigitIndex; // alternate digit index
  byte currentDigit = (currentDigitIndex == 0) ? currentNumber % 10 : currentNumber / 10;
  if (currentNumber==0) currentDigit = 10; // Dash
  for (byte i = 0; i < 7; i++) {
    digitalWrite(segmentPins[i], !bitRead(numberMap[currentDigit], i));
  }
  digitalWrite(digitPins[currentDigitIndex], ON);
  delay(10);
}

void ShowRandomNumber() {
  if (digitalRead(buttonPins[0])==PRESSED) {
    byte randomNumber = random(1,100);
    currentNumber = randomNumber;
  }
}


#include "Magician.h"

void NumberPicked() {
  if (digitalRead(buttonPins[1])==PRESSED) {
    PrepareMagic(currentNumber);
    currentNumber = 0;
  }
}

void TakeaBow() {
  Serial.println("TakeaBow");
}



void loop() {
  //TestSegments();
  //TestDigits();
  //TestButtons();
  //TestNumbers();
  Refresh();
  ShowRandomNumber();
  NumberPicked();
  PerformMagic();
}

/*
 * [A] User pick a secret number between 1 and 99
 * [B] Magician pick a random sentinel
 * [C] Magician decide how many guesses before showing secret#
 * [D] Magician fills guessNumbers array.  Last element is the secret#, before that is the sentinel
 * [E] Magician displays sentinel hint as first guess
 * [F] Magician waits for accomplice response in button 3
 * [G] Magician keeps displaying prepared guess array and waiting for response
 */
