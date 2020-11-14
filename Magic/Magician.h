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

void PrepareMagic(byte chosenNum) {
  Print("PrepareMagic. chosenNum=", chosenNum);
  chosenNumber = chosenNum;
  
  numberOfGuesses = random(3, maxNumberOfGuesses + 1);
  Print("numberOfGuesses", numberOfGuesses);

  do {
    sentinelHint = PickHint();
    sentinelNumber = HintToSentinel(sentinelHint);
  } while ((sentinelHint == chosenNumber) || (sentinelNumber == chosenNumber));

  Print("chosenNumber", chosenNumber);
  Print("sentinelHint", sentinelHint);
  Print("sentinelNumber", sentinelNumber);

  GenerateGuesses();
}

void PerformMagic() {
  if (numberOfGuesses != 0) {
    Serial.println("PerformMagic");
    for (byte i = 0; i < numberOfGuesses; i++) {
      Serial.print("Guess["); Serial.print(i); Print("]", guesses[i]);
    }
    for (byte guessIndex = 0; guessIndex < numberOfGuesses; guessIndex++) {
      PrintElement("guesses", guessIndex, guesses[guessIndex]);
      currentNumber = guesses[guessIndex];
      do {
        Refresh();
      } while (digitalRead(buttonPins[2]) != PRESSED);

      delay(500);
    }

    TakeaBow();
    numberOfGuesses = 0; // Get ready for next show
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
