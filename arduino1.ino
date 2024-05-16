

#include <LiquidCrystal.h>
#include <stdlib.h>  // Include for random functionality

// Initialize the LCD library with the numbers of the interface pins
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

// Pin setup for the buttons
const int buttonPins[] = {4, 5, 6, 7};  // Pin assignments: Increment, Decrease, Start/Pause, Reset (Left to right)
const int numButtons = 4;
int buttonState[numButtons];
int lastButtonState[numButtons] = {HIGH, HIGH, HIGH, HIGH};
String clearScr = "                "; // 16 spaces

// Timing and state management
unsigned long lastDebounceTime[numButtons] = {0, 0, 0, 0};
unsigned long debounceDelay = 100;  // milliseconds
unsigned long countdownTime = 900;  // 15 minutes in seconds
unsigned long lastInteractionTime; // Mainly used for transitioning between active and idle states
unsigned long lastScrollTime = 0;
int scrollDelay = 700;  // milliseconds for idle scroll
boolean isCounting = false;
String currentState = "active";  // Start with initial state as "active"

// Idle state animation
const String idleMessages[] = {
    "Cats can make over 100 vocal sounds.",
    "Cats sleep 70% of their lives.",
    "A group of cats is called a clowder.",
    "Cats have five toes on front paws."
};

int idleMessageIndex = 0;
int idlePosition = 0;


void setup() {
  lcd.begin(16, 2); // set up the LCD's number of columns and rows
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  lastInteractionTime = millis();
  displayWelcomeMessage();
}

void loop() {
  int idleTime = 10000; // 10 seconds; The screen will go to idle state after 10 seconds of inactivity (No button press)
  boolean anyButtonPressed = false;

  for (int i = 0; i < numButtons; i++) {
    int reading = digitalRead(buttonPins[i]);

    if (reading != lastButtonState[i]) {
      lastDebounceTime[i] = millis();
    }

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (reading != buttonState[i]) {
        buttonState[i] = reading;

        if (buttonState[i] == LOW) {
          anyButtonPressed = true;
          lastInteractionTime = millis();
          if (currentState.equals("idle")) {
            currentState = "active"; // Exit idle state on any button press
            displayTime();
          } else {
            handleButtonPress(i);
          }
        }
      }
    }
    lastButtonState[i] = reading;
  }

  // Check to enter idle state regardless of counting state
  if (!anyButtonPressed && millis() - lastInteractionTime > idleTime && currentState.equals("active")) {
    enterIdleState();
  }

  // Scroll idle messages in idle state and manage scrolling timing
  if (currentState.equals("idle") && millis() - lastScrollTime > scrollDelay) {
    scrollIdleMessage();
    lastScrollTime = millis();
  }

  // Update countdown independently of the current state, but only update display if in active state
  if (isCounting) {
    updateCountdown();
    if (currentState.equals("active")) {
      displayTime();  // Refresh time display only if not in idle state
    }
  }

  delay(100);
}

void updateCountdown() {
  static unsigned long lastUpdateTime = 0;
  if (millis() - lastUpdateTime >= 1000) {
    lastUpdateTime = millis();
    if (countdownTime > 0) {
      countdownTime--;
    } else {
      isCounting = false;  // Stop countdown when time reaches zero
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Time's Up!");
    }
  }
}

void displayWelcomeMessage() {
  String message = "Welcome 2 TONITO";
  String displayText = "";
  int length = message.length();
  
  // Fill the displayText with random alphanumeric characters
  for (int i = 0; i < length; i++) {
    displayText += (message[i] == ' ') ? ' ' : char(random(33, 126)); // Random char between '0' and 'z'
  }

  lcd.clear();
  lcd.setCursor(0, 0);

  //Deciphering animations; Aesthetic purposes
  // Gradually change random characters to the correct ones
  for (int i = 0; i < length; i++) {
    delay(150);  // Delay between transitions
    displayText[i] = message[i];
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(displayText);
  }

  delay(2000); // Hold the final message for 2 seconds
  currentState = "active";
  displayTime(); // Ensure display is refreshed from welcome message
}

void handleButtonPress(int buttonIndex) {
  if (currentState.equals("idle")) {
    displayTime(); // Ensure display is refreshed from idle state
  }

  switch (buttonIndex) {
    case 0: // Increase timer
      if (countdownTime < 43200) {
        countdownTime += 900;
      }
      break;
    case 1: // Decrease timer
      if (countdownTime > 900) {
        countdownTime -= 900;
      }
      break;
    case 2: // Start/Pause
      isCounting = !isCounting;  // Toggle counting state
      break;
    case 3: // Reset timer
      countdownTime = 900;
      isCounting = false;  // Stop counting when reset
      break;
  }

  displayTime(); // Refresh the display every time a button is pressed and handled
}

void displayTime() {
  lcd.clear();
  lcd.setCursor(1, 0);  // Center the top text
  lcd.print("Food Countdown");
  lcd.setCursor(4, 1);  // Position timer at the bottom center
  int seconds = countdownTime % 60;
  int minutes = (countdownTime / 60) % 60;
  int hours = (countdownTime / 3600);

  lcd.print((hours < 10 ? "0" : "") + String(hours) + ":");
  lcd.print((minutes < 10 ? "0" : "") + String(minutes) + ":");
  lcd.print(seconds < 10 ? "0" : "");
  lcd.print(seconds);
}

void enterIdleState() {
  lcd.clear();
  currentState = "idle";
  scrollIdleMessage();  // Start with the first message immediately
}

void scrollIdleMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  String displayText = idleMessages[idleMessageIndex].substring(idlePosition) + clearScr;
  if (displayText.length() < 16) {
    displayText += idleMessages[idleMessageIndex].substring(0, 16 - displayText.length());
  }
  lcd.print(displayText);

  idlePosition++;
  if (idlePosition >= idleMessages[idleMessageIndex].length() + 1) {
    idlePosition = 0;
    idleMessageIndex = (idleMessageIndex + 1) % (sizeof(idleMessages) / sizeof(idleMessages[0]));
  }
}
