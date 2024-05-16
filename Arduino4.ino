
#include <SPI.h>
#include <MFRC522.h>
#include <pitches.h>

//ss ping (sda) - 10
// mosi - 11
// miso - 12
// sck - 13
// GND - GND 
// rst - 5
//buzzer - 8 and one end into GND

// Pin configuration for the MFRC522 and the buzzer
#define RST_PIN 5
#define SS_PIN 10
#define BUZZER_PIN 8  

int lowFoodMelody[] = {


  // Hedwig's theme fromn the Harry Potter Movies
  // Socre from https://musescore.com/user/3811306/scores/4906610
  
  REST, 2, NOTE_D4, 4,
  NOTE_G4, -4, NOTE_AS4, 8, NOTE_A4, 4,
  NOTE_G4, 2, NOTE_D5, 4,
  NOTE_C5, -2, 
  NOTE_A4, -2,
  NOTE_G4, -4, NOTE_AS4, 8, NOTE_A4, 4,
  NOTE_F4, 2, NOTE_GS4, 4,
  NOTE_D4, -1, 
  NOTE_D4, 4,

  NOTE_G4, -4, NOTE_AS4, 8, NOTE_A4, 4, //10
  NOTE_G4, 2, NOTE_D5, 4,
  NOTE_F5, 2, NOTE_E5, 4,
  NOTE_DS5, 2, NOTE_B4, 4,
  NOTE_DS5, -4, NOTE_D5, 8, NOTE_CS5, 4,
  NOTE_CS4, 2, NOTE_B4, 4,
  NOTE_G4, -1,
  NOTE_AS4, 4,
     
  NOTE_D5, 2, NOTE_AS4, 4,//18
  NOTE_D5, 2, NOTE_AS4, 4,
  NOTE_DS5, 2, NOTE_D5, 4,
  NOTE_CS5, 2, NOTE_A4, 4,
  NOTE_AS4, -4, NOTE_D5, 8, NOTE_CS5, 4,
  NOTE_CS4, 2, NOTE_D4, 4,
  NOTE_D5, -1, 
  REST,4, NOTE_AS4,4,  

  NOTE_D5, 2, NOTE_AS4, 4,//26
  NOTE_D5, 2, NOTE_AS4, 4,
  NOTE_F5, 2, NOTE_E5, 4,
  NOTE_DS5, 2, NOTE_B4, 4,
  NOTE_DS5, -4, NOTE_D5, 8, NOTE_CS5, 4,
  NOTE_CS4, 2, NOTE_AS4, 4,
  NOTE_G4, -1, 
  
};

// Notes and durations for the melody to be played
int melody[] = {
    NOTE_AS4, NOTE_AS4, NOTE_AS4,
    NOTE_F5, NOTE_C6, NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6,
    NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6, NOTE_AS5, NOTE_A5,
    NOTE_AS5, NOTE_G5, NOTE_C5, NOTE_C5, NOTE_C5, NOTE_F5, NOTE_C6,
    NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6, NOTE_AS5, NOTE_A5,
    NOTE_G5, NOTE_F6, NOTE_C6, NOTE_AS5, NOTE_A5, NOTE_AS5, NOTE_G5,
    NOTE_C5, NOTE_C5, NOTE_D5, NOTE_D5, NOTE_AS5, NOTE_A5, NOTE_G5,
    NOTE_F5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_D5, NOTE_E5,
    NOTE_C5, NOTE_C5, NOTE_D5, NOTE_D5, NOTE_AS5, NOTE_A5, NOTE_G5,
    NOTE_F5, NOTE_C6, NOTE_G5, NOTE_G5, REST, NOTE_C5, NOTE_D5,
    NOTE_D5, NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F5, NOTE_F5, NOTE_G5,
    NOTE_A5, NOTE_G5, NOTE_D5, NOTE_E5, NOTE_C6, NOTE_C6, NOTE_F6,
    NOTE_DS6, NOTE_CS6, NOTE_C6, NOTE_AS5, NOTE_GS5, NOTE_G5, NOTE_F5,
    NOTE_C6
};

int lowFoodNotes = sizeof(lowFoodMelody) / sizeof(lowFoodMelody[0]) / 2;
int lowFoodTempo = 144;
// this calculates the duration of a whole note in ms (60s/tempo)*4 beats
int lowFoodWholeNote = (60000 * 4) / lowFoodTempo;

int lowFoodDivider = 0, lowFoodNoteDuration = 0;

int durations[] = {
    8, 8, 8, 2, 2, 8, 8, 8, 2, 4, 8, 8, 8, 2, 4, 8, 8, 8, 2, 8,
    8, 8, 2, 2, 8, 8, 8, 2, 4, 8, 8, 8, 2, 4, 8, 8, 8, 2, 8, 16,
    4, 8, 8, 8, 8, 8, 8, 8, 8, 4, 8, 4, 8, 16, 4, 8, 8, 8, 8, 8,
    8, 16, 2, 8, 8, 4, 8, 8, 8, 8, 8, 8, 8, 8, 4, 8, 4, 8, 16, 4,
    8, 4, 8, 4, 8, 4, 8, 1
};

int fsrPin = 0;     // the FSR and 10K pulldown are connected to a0
int fsrReading;     // the analog reading from the FSR resistor divider

// RFID reader object initialization
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Variable to track last play time and serial input for low food melody
const unsigned long delayBetweenPlaysLowFood = 900000; // 15 minutes in milliseconds
unsigned long lastPlayTimeLowFood = 0 - delayBetweenPlaysLowFood;
char receivedChar; // Variable to hold the received character over serial

// Variable to track last play time and serial input
const unsigned long delayBetweenPlays = 600000; // 10 minutes in milliseconds
unsigned long lastPlayTime = 0 - delayBetweenPlays;


void setup() {
    Serial.begin(9600);
    while (!Serial);  // Wait for the serial port to connect. Needed for native USB only
    SPI.begin();      // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522
    pinMode(BUZZER_PIN, OUTPUT); // Setup buzzer pin as an output
    Serial.println(F("System ready, scan RFID or send 'T' over serial to play melody."));
}

void loop() {
    readRFID();
    readSerial();
}

void playMelody() {
    unsigned long startTime = millis(); // Get the start time
    int notes = sizeof(melody) / sizeof(melody[0]); // Calculate the number of notes
    for (int note = 0; note < notes; note++) {
        int duration = 1000 / durations[note];
        tone(BUZZER_PIN, melody[note], duration);  // Play the note on the buzzer pin
        int pauseBetweenNotes = duration * 1.30;   // Pause between notes
        delay(pauseBetweenNotes);
        noTone(BUZZER_PIN); // Stop the tone
        if ((millis() - startTime) >= 10000) break; // Stop playing if 10 seconds have passed
    }
    lastPlayTime = millis(); // Update last play time after the melody is played
}

void readRFID() {
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        if (millis() - lastPlayTime >= delayBetweenPlays) {
            Serial.println(F("RFID tag detected. Playing melody."));
            playMelody(); // Play melody when RFID tag is detected and time condition is met
        } else {
            Serial.println(F("RFID tag detected, but waiting for timeout."));
        }
        mfrc522.PICC_DumpToSerial(&(mfrc522.uid)); // Dump the UID to serial even if melody is not played
    }
}


void playLowFoodMelody() {
  unsigned long lowFoodStartTime = millis();
  for (int thisNote = 0; thisNote < lowFoodNotes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    lowFoodDivider = lowFoodMelody[thisNote + 1];
    if (lowFoodDivider > 0) {
      // regular note, just proceed
      lowFoodNoteDuration = (lowFoodWholeNote) / lowFoodDivider;
    } else if (lowFoodDivider < 0) {
      // dotted notes are represented with negative durations!!
      lowFoodNoteDuration = (lowFoodWholeNote) / abs(lowFoodDivider);
      lowFoodNoteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(BUZZER_PIN, lowFoodMelody[thisNote], lowFoodNoteDuration*0.9);

    // Wait for the specief duration before playing the next note.
    delay(lowFoodNoteDuration);
    
    // stop the waveform generation before the next note.
    noTone(BUZZER_PIN);

    if ((millis() - lowFoodStartTime) >= 10000) break; // Stop playing if 10 seconds have passed
  }
    
    lastPlayTimeLowFood = millis(); // Update last play time after the low food melody is played
}

void readSerial() {
    if (Serial.available() > 0) {
        receivedChar = Serial.read(); // Read the incoming character
        if (receivedChar == 'T') {
          Serial.println(F("Serial 'T' received. Playing low food melody."));
            if (millis() - lastPlayTimeLowFood >= delayBetweenPlaysLowFood) {
                playLowFoodMelody(); // Play low food melody if 'T' received and time condition is met
            } else {
                Serial.println(F("Serial 'T' received, but waiting for timeout."));
            }
        }
    }
}

