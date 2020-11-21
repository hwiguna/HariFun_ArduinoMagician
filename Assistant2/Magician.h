// Don't study this file unless you want to ruin the trick. :-)

const byte maxNumberOfGuesses = 6;
byte guesses[maxNumberOfGuesses];
byte numberOfGuesses;
byte sentinelHint;

byte sentinelDigit;
bool isSentinelOnLeft;
byte sentinelNumber;

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
  sentinelDigit = 9 - sentinelHintDigit;

  do {
    byte sentinelOtherDigit = random(0, 10);

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
  } while (firstDigit == secondDigit);
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
  guesses[0] = sentinelHint;
  guesses[numberOfGuesses - 2] = sentinelNumber;
  guesses[numberOfGuesses - 1] = chosenNumber;

  // Fill the guesses randomly
  for (byte i = 1; i < (numberOfGuesses - 2); i++) {
    guesses[i] = GoodRandom();
    PrintElement("guesses", i, guesses[i]);
  }
}

//------------------------------------

void PrepareMagic() {
  Print("\nPrepareMagic. chosenNumber", chosenNumber);
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
  currentState = WAITING_FOR_MAGICIAN;
}

void PerformMagic() {
  Serial.println("PerformMagic");
  // First, show "--" to indicate start of guesses...
  currentNumber = 0;
  byte pressedButtonIndex;
  do {
    pressedButtonIndex = WaitForButton();
   // if (pressedButtonIndex == 0) {currentState = WAITING_FOR_CHOSEN_NUMBER; break;}
   // if (pressedButtonIndex == 2) {currentState = WAITING_FOR_NEXT_GUESS; break;}
    if (pressedButtonIndex == 0) {currentState = WAITING_FOR_NEXT_GUESS; break;}
  } while (true);

  if (currentState == WAITING_FOR_NEXT_GUESS) {
    if (numberOfGuesses != 0) { // Make sure PrepareMagic() has filled the guesses array()
      for (byte i = 0; i < numberOfGuesses; i++) {
        Serial.print("Guess["); Serial.print(i); Print("]", guesses[i]);
      }
      for (byte guessIndex = 0; guessIndex < numberOfGuesses; guessIndex++) {
        // Wait for either buttons to be pressed
        do {
          delay(50);
        } while ((digitalRead(buttonPins[0]) != PRESSED) && (digitalRead(buttonPins[1]) != PRESSED));
        PrintElement("guesses", guessIndex, guesses[guessIndex]);

        // Keep showing random numbers while either button is pressed.
        do {
          currentNumber = random(1,100);
          delay(50);
        } while ((digitalRead(buttonPins[0]) == PRESSED) || (digitalRead(buttonPins[1]) == PRESSED));      
        currentNumber = guesses[guessIndex]; // When they released the button sneak in the real current#
        
        delay(500); // Let them release the button before checking for another keypress
      }

      Serial.println("all guesses shown");

      Serial.println("Waiting for button to applause");
      pressedButtonIndex = WaitForButton(); // does not matter which button they press.
      Print("Button pressed for applause. pressedButtonIndex", pressedButtonIndex);
  
      delay(500); // allow them to release the button long enough before checking to dismiss applause

      currentState = WAITING_FOR_NEXT_TRICK;
      Applause(); // until they press button 0 to get another random#
      numberOfGuesses = 0; // Get ready for next show
      chosenNumber = 0;
    }
  }
}
