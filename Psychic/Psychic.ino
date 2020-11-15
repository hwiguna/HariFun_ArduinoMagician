#include <TimerOne.h>

/*
   Knows what was picked by another Arduino!
   by Hari Wiguna, 2020
*/

const byte segmentPins[] = {2, 3, 4, 5, 6, 7, 8};
const byte digitPins[] = {12, 13};
const byte maxNumber = 99; // Max # that could be displayed with two digits
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

byte currentNumber = random(1, maxNumber + 1);
volatile byte currentDigitIndex = 0;
volatile bool isDisplayOn = true;

#define WAITING_FOR_HINT  0
#define WAITING_FOR_GUESS 1
#define WAITING_FOR_NEXT_TRICK  2
byte currentState = WAITING_FOR_HINT;

//=== Keypad ===
const byte rowPowerPins[] = {A4, A5, 9, 10};
const byte colReadPins[] = {A0, A1, A2, A3};
const char keypadMap[] = {'1', '2', '3', 'A', '4', '5', '6', 'B', '7', '8', '9', 'C', '*', '0', '#', 'D'};

void DisplayCurrentNumber(void)
{
  digitalWrite(digitPins[currentDigitIndex], OFF); // Turn off last digit index
  currentDigitIndex = 1 - currentDigitIndex; // alternate digit index
  byte currentDigit = (currentDigitIndex == 0) ? currentNumber % 10 : currentNumber / 10;
  if (currentNumber == 0) currentDigit = 10; // Dash
  for (byte i = 0; i < 7; i++) {
    bool isOn = !(bitRead(numberMap[currentDigit], i) && isDisplayOn);
    digitalWrite(segmentPins[i], isOn);
  }
  digitalWrite(digitPins[currentDigitIndex], ON);
}

void SetupKeypad() {
  for (byte r = 0; r < 4; r++) {
    pinMode(rowPowerPins[r], OUTPUT);
    digitalWrite(rowPowerPins[r], HIGH);
  }
  for (byte c = 0; c < 4; c++) {
    pinMode(colReadPins[c], INPUT_PULLUP);
  }
}


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

  SetupKeypad();

  Timer1.initialize(10000); // call every 0.01 seconds
  Timer1.attachInterrupt(DisplayCurrentNumber);
}

void TestSegments() {
  digitalWrite(digitPins[0], LOW); // common anode off
  digitalWrite(digitPins[1], LOW); // common anode off
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

void TestDigits() {
  digitalWrite(digitPins[0], ON); digitalWrite(digitPins[1], OFF); delay(500);
  digitalWrite(digitPins[0], OFF); digitalWrite(digitPins[1], ON); delay(500);
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
  //  digitalWrite(digitPins[currentDigitIndex], OFF); // Turn off last digit index
  //  currentDigitIndex = 1 - currentDigitIndex; // alternate digit index
  //  byte currentDigit = (currentDigitIndex == 0) ? currentNumber % 10 : currentNumber / 10;
  //  if (currentNumber == 0) currentDigit = 10; // Dash
  //  for (byte i = 0; i < 7; i++) {
  //    digitalWrite(segmentPins[i], !bitRead(numberMap[currentDigit], i));
  //  }
  //  digitalWrite(digitPins[currentDigitIndex], ON);
  DisplayCurrentNumber();
  delay(10);
}

void ShuffleRandomNumber() {
  //  if (digitalRead(buttonPins[0]) == PRESSED) {
  //    byte randomNumber = random(1, 100);
  //    currentNumber = randomNumber;
  //  }
}


#include "Magician.h"


byte ReadKeypad() {
  byte keyPressed = 99;
  for (byte r = 0; r < 4; r++) {
    //Print("\nr", r);
    digitalWrite(rowPowerPins[r], LOW);
    for (byte c = 0; c < 4; c++) {
      //Print("c", c);
      if (digitalRead(colReadPins[c]) == 0) {
        keyPressed = r * 4 + c;
        //Print("keyPressed", keyPressed);
      }
    }
    digitalWrite(rowPowerPins[r], HIGH);
    //if (keyPressed != 99) break;
  }
  return keyPressed;
}

//bool NumberPicked() {
//  if ((currentNumber != 0) && (digitalRead(buttonPins[1]) == PRESSED)) {
//    chosenNumber = currentNumber;
//  }
//  return chosenNumber != 0;
//}

void Applause() {
  Serial.println("Applause");
  //  bool buttonPressed = false;
  //  do {
  //    isDisplayOn = !isDisplayOn;
  //    for (byte i = 0; i < 5; i++) {
  //      if (digitalRead(buttonPins[0]) == PRESSED) {
  //        buttonPressed = true;
  //      }
  //      delay(50);
  //    }
  //  } while (!buttonPressed);
  isDisplayOn = true;

  currentState = WAITING_FOR_HINT;
}

void TestKeypad() {
  byte keyPressedIndex = ReadKeypad();
  currentNumber = keyPressedIndex;
  if (keyPressedIndex != 99) {
    char ch = keypadMap[keyPressedIndex];
    //Print("ch", ch);
    Serial.println(ch);

    while (ReadKeypad() == keyPressedIndex)
      delay(250);
  }
}


void loop() {
  //TestSegments();
  TestKeypad();
  //TestDigits();
  //TestButtons();
  //TestNumbers();
  //Refresh();
  //
  //  switch (currentState) {
  //    case WAITING_FOR_CHOSEN_NUMBER:
  //      ShuffleRandomNumber();
  //      if (NumberPicked()) PrepareMagic();
  //      break;
  //    case WAITING_FOR_MAGICIAN:
  //      PerformMagic();
  //      break;
  //  }
}
