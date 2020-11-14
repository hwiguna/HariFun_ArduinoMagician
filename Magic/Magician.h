const byte maxNumberOfGuesses = 5;
byte guesses[maxNumberOfGuesses];
byte numberOfGuesses;
byte sentinelHint; // This is the clue to the sentinel
byte sentinelNumber; // This is the sentinel itself


void Print(String label, byte number) {
  Serial.print(label);
  Serial.print(" = ");
  Serial.println(number);
}

byte HintToSentinel(byte sentinelHint) {
  byte firstDigit = sentinelHint / 10;
  byte secondDigit = sentinelHint % 10;
  bool isSentinelOnLeft = (firstDigit > secondDigit);
  byte sentinelHintDigit =  isSentinelOnLeft ? firstDigit : secondDigit;
  byte sentinelDigit = 9 - sentinelHintDigit; // This is what indicates that next # is the secret #
  byte sentinelOtherDigit = random(0, 10); // any digit will do.

  sentinelNumber = (isSentinelOnLeft) ?
                   sentinelDigit * 10 + sentinelOtherDigit :
                   sentinelOtherDigit * 10 + sentinelDigit;
}

byte PickHint() {
  byte firstDigit, secondDigit;
  do {
    sentinelHint = random(1, 99);
    firstDigit = sentinelHint / 10;
    secondDigit = sentinelHint % 10;
  } while (firstDigit == secondDigit);
  sentinelNumber = HintToSentinel(sentinelHint);
  return sentinelHint;
}

//------------------------------------

void PrepareMagic(byte chosenNumber) {
  Serial.println("PrepareMagic");
  numberOfGuesses = random(3, maxNumberOfGuesses + 1);
  Print("numberOfGuesses", numberOfGuesses);

  do {
    sentinelHint = PickHint();
  } while (sentinelHint != chosenNumber && sentinelNumber != chosenNumber);

}

void PerformMagic() {
  if (numberOfGuesses != 0) {
    Serial.println("PerformMagic");
    for (byte guessIndex = 0; guessIndex < numberOfGuesses; guessIndex++) {
      Print("GuessIndex", guessIndex);
      currentNumber = guessIndex; // Temporary
      do {
        Refresh();
      } while (digitalRead(buttonPins[2]) != PRESSED);

      delay(500);
    }
    
    TakeaBow();
    numberOfGuesses = 0; // Get ready for next show
  }
}
