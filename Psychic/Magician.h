// States of the magic trick
#define HAS_NOTHING 0
#define HAS_HINT 1
#define HAS_SENTINEL 2
#define HAS_SEEN_SENTINEL 3
#define HAS_COMPLETED 4
byte magicState;

const byte maxNumberOfGuesses = 9;
byte guesses[maxNumberOfGuesses];
byte numberOfGuesses;
byte sentinelHint; // This is the clue to the sentinel

byte sentinelDigit; // The sentinel digit we're going to look for
bool isSentinelOnLeft; // Is sentinel digit on left or right?
byte sentinelNumber; // This is the sentinel (only one of the digits is relevant)

byte chosenNumber;

void Print(String label, byte number) {
  Serial.print(label);
  Serial.print(" = ");
  Serial.println(number);
}

void PrintElement(String var, byte index, byte value) {
  Serial.print(var);
  Serial.print("[");
  Serial.print(index);
  Serial.print("]=");
  Serial.println(value);
}

byte HintToSentinel(byte sentinelHint) {

  byte firstDigit = sentinelHint / 10;
  byte secondDigit = sentinelHint % 10;
  isSentinelOnLeft = (firstDigit > secondDigit);
  byte sentinelHintDigit =  isSentinelOnLeft ? firstDigit : secondDigit;
  sentinelDigit = 9 - sentinelHintDigit; // This is what indicates that next # is the secret #

  do {
    byte sentinelOtherDigit = random(0, 10); // any digit will do. Even same as sentinel digit.

    sentinelNumber = (isSentinelOnLeft) ?
                     sentinelDigit * 10 + sentinelOtherDigit :
                     sentinelOtherDigit * 10 + sentinelDigit;
  } while (sentinelNumber == 0);

  Print("HintToSentinel. sentinelHint", sentinelHint);
  Print("firstDigit", firstDigit);
  Print("secondDigit", secondDigit);
  Print("isSentinelOnLeft", isSentinelOnLeft);
  Print("sentinelHintDigit", sentinelHintDigit);
  Print("sentinelDigit", sentinelDigit);
  Print("sentinelNumber", sentinelNumber);

  return sentinelNumber;
}

byte PickHint() {
  byte firstDigit, secondDigit;
  do {
    sentinelHint = random(1, maxNumber + 1);
    firstDigit = sentinelHint / 10;
    secondDigit = sentinelHint % 10;
  } while (firstDigit == secondDigit); // Hint must be made of two different digits to indicate which one is the sentinel digit.
  return sentinelHint;
}

byte GoodRandom() {
  byte randomGuess;
  bool conflictWithSentinelDigit, conflictWithSentinelHint, conflictWithChosenNumber;
  do {
    randomGuess = random(1, maxNumber + 1);
    byte valueAtSentinelPosition = isSentinelOnLeft ? randomGuess / 10 : randomGuess % 10;
    conflictWithSentinelDigit = valueAtSentinelPosition == sentinelDigit;
    conflictWithSentinelHint = randomGuess == sentinelHint;
    conflictWithChosenNumber = randomGuess == chosenNumber;
  } while (conflictWithSentinelDigit || conflictWithSentinelHint || conflictWithChosenNumber );
  return randomGuess;
}

void GenerateGuesses() {
  Serial.println("GenerateGuesses");
  guesses[0] = sentinelHint; // First guess is the sentinel hint
  guesses[numberOfGuesses - 2] = sentinelNumber; // next to last guess is the sentinel #
  guesses[numberOfGuesses - 1] = chosenNumber; // last guess is the chosen#

  // Fill the rest of guesses randomly
  for (byte i = 1; i < (numberOfGuesses - 2); i++) {
    guesses[i] = GoodRandom();
    PrintElement("guesses", i, guesses[i]);
  }
}

//------------------------------------

//void PrepareMagic() {
//  Print("\nPrepareMagic. chosenNumber", chosenNumber);
//  numberOfGuesses = random(3, maxNumberOfGuesses + 1);
//  Print("numberOfGuesses", numberOfGuesses);
//
//  do {
//    sentinelHint = PickHint();
//    sentinelNumber = HintToSentinel(sentinelHint);
//  } while ((sentinelHint == chosenNumber) || (sentinelNumber == chosenNumber));
//
//  Print("chosenNumber", chosenNumber);
//  Print("sentinelHint", sentinelHint);
//  Print("sentinelNumber", sentinelNumber);
//
//  GenerateGuesses();
//  currentState = WAITING_FOR_GUESS;
//}

void PerformMagic() {
  Serial.println("PerformMagic");
  //  // First, show "--" to indicate start of guesses...
  //  currentNumber = 0;
  //  byte pressedButtonIndex;
  //  do {
  //    pressedButtonIndex = WaitForButton();
  //    if (pressedButtonIndex == 0) {currentState = WAITING_FOR_CHOSEN_NUMBER; break;}
  //    if (pressedButtonIndex == 2) {currentState = WAITING_FOR_NEXT_GUESS; break;}
  //  } while (true);
  //
  //  if (currentState == WAITING_FOR_NEXT_GUESS) {
  //    if (numberOfGuesses != 0) { // Make sure PrepareMagic() has filled the guesses array()
  //      for (byte i = 0; i < numberOfGuesses; i++) {
  //        Serial.print("Guess["); Serial.print(i); Print("]", guesses[i]);
  //      }
  //      for (byte guessIndex = 0; guessIndex < numberOfGuesses; guessIndex++) {
  //        PrintElement("guesses", guessIndex, guesses[guessIndex]);
  //        currentNumber = guesses[guessIndex];
  //        do {
  //          //Refresh();
  //          delay(100);
  //        } while (digitalRead(buttonPins[2]) != PRESSED);
  //
  //        delay(500);
  //      }
  //
  //      currentState = WAITING_FOR_NEXT_TRICK;
  //      Applause(); // until they press button 0 to get another random#
  //      numberOfGuesses = 0; // Get ready for next show
  //      chosenNumber = 0;
  //    }
  //  }
}

void ProcessFirstDigit(char keypress) {

  if (keypress >= '0' && keypress <= '9') {
    currentNumber = (keypress - '0') * 10;
    isDisplayOn[0] = false; // ones
    isDisplayOn[1] = true;  // tens
    currentState = WAITING_FOR_SECOND_DIGIT;
  }
}

void SaveSentinel() {
  sentinelHint = currentNumber;
  magicState = HAS_HINT;
  Print("HAS_HINT. sentinelHint", sentinelHint);

  sentinelNumber = HintToSentinel(sentinelHint);

  magicState = HAS_SENTINEL;
  Print("HAS_SENTINEL. sentinelDigit", sentinelDigit);
  Print("isSentinelOnLeft", isSentinelOnLeft);  // We will be looking for sentinel on left/right
}

void CheckForSentinel(byte currentNumber) {
  Print("CheckForSentinel. currentNumber", currentNumber);
  byte potentialSentinelDigit = isSentinelOnLeft ? currentNumber / 10 : currentNumber % 10;
  if (potentialSentinelDigit == sentinelDigit) {
    magicState = HAS_SEEN_SENTINEL;
    Print("HAS_SEEN_SENTINEL! currentNumber", currentNumber);
  }
}

void ProcessSecondDigit(char keypress) {
  Print("ProcessSecondDigit. keypress char",keypress);
  if (keypress >= '0' && keypress <= '9') {
    currentNumber = currentNumber + (keypress - '0');
    isDisplayOn[0] = true; // ones
    isDisplayOn[1] = true;  // tens
    currentState = WAITING_FOR_FIRST_DIGIT;

    Print("magicState",magicState);
    switch (magicState) {
      case HAS_NOTHING: SaveSentinel(); break;
      case HAS_SENTINEL: CheckForSentinel(currentNumber); break;
      case HAS_SEEN_SENTINEL: Applause(); break;
    }
  }
}

void ProcessKeypress(char keypress) {
  switch (currentState) {
    case WAITING_FOR_FIRST_DIGIT:
      ProcessFirstDigit(keypress);
      break;
    case WAITING_FOR_SECOND_DIGIT:
      ProcessSecondDigit(keypress);
      break;
  }
}

/*
   [A] User pick a secret number between 1 and 99
   [B] Magician pick a random sentinel
   [C] Magician decide how many guesses before showing secret#
   [D] Magician fills guessNumbers array.  Last element is the secret#, before that is the sentinel
   [E] Magician displays sentinel hint as first guess
   [F] Magician waits for accomplice response in button 3
   [G] Magician keeps displaying prepared guess array and waiting for response
*/
