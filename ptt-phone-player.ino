/***********Notice and Trouble shooting***************
 1.Connection and Diagram can be found here
 <https://www.dfrobot.com/wiki/index.php/DFPlayer_Mini_SKU:DFR0299#Connection_Diagram>
 2.This code is tested on Arduino Uno, Leonardo, Mega boards.
 ****************************************************/

#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"

#include <SoftwareSerial.h>
SoftwareSerial softSerial(/*rx =*/4, /*tx =*/5);
#define FPSerial softSerial

#define LIFT_PIN 6
#define DIAL_PIN 2
#define PULSE_PIN 3
#define DEBOUNCE_DELAY 50 // ms

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
void handleLift();
void handleDial();
void handlePulse();

// Debounce lift contact
int liftContactState = HIGH;
int lastLiftContactState = HIGH;
unsigned long lastLiftDebounceTime = 0;

// Debounce dial contact
int dialContactState = HIGH;
int lastDialContactState = HIGH;
unsigned long lastDialDebounceTime = 0;

// Debounce pulse contact
int pulseContactState = LOW;
int lastPulseContactState = LOW;
unsigned long lastPulseDebounceTime = 0;

unsigned int pulseCount = 0;
unsigned int inputCount = 0;
unsigned long code = 0;

bool dialStarted = false;
bool dialEnded = false;
bool shouldStartTone = false;
bool shouldStopAnyAudio = false;

//// Included audio files on the SD Card
// mp3/0001.mp3 → 425 Hz tone (line)
// mp3/0002.mp3 → busy tone
// mp3/0003.mp3 → Wrong number tone
// mp3/0004.mp3 → audio file 1
// mp3/0005.mp3 → audio file 2

void setup()
{
  FPSerial.begin(9600);

  Serial.begin(115200);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(FPSerial, /*isACK = */true, /*doReset = */true)) {  //Use serial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.volume(10);  //Set volume value. From 0 to 30

  pinMode(LIFT_PIN, INPUT_PULLUP);
  pinMode(DIAL_PIN, INPUT_PULLUP);
  pinMode(PULSE_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(DIAL_PIN), handleDial, RISING);
  attachInterrupt(digitalPinToInterrupt(PULSE_PIN), handlePulse, RISING);
}

void loop()
{
  // Detect Lift/Hangup
  handleLift();

  if (shouldStartTone) {
    shouldStartTone = false;
    myDFPlayer.playMp3Folder(1);
  }

  if (shouldStopAnyAudio) {
    shouldStopAnyAudio = false;
    myDFPlayer.stop();
  }

  if (dialStarted) {
    myDFPlayer.stop();
  }

  if (dialEnded) {
    dialStarted = false;
    dialEnded = false;
    Serial.print(F("Dialed "));Serial.println(pulseCount % 10);

    inputCount++;
    code = code * 10 + pulseCount % 10;

    if (inputCount == 4) {
       // Test the code
       if (code == 9574) {
        myDFPlayer.playMp3Folder(4);
       } else {
        Serial.print(F("Wrong "));Serial.println(code);
        myDFPlayer.playMp3Folder(3);
       }
       inputCount = 0;
       code = 0;
    }

    // Reset the pulse count
    pulseCount = 0;
  }
  
  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
}

void handleLift() {
  // Polling, Not used as an interrupt 
  unsigned int currentState = digitalRead(LIFT_PIN);
  unsigned long now = millis();

  if (lastLiftContactState != currentState && (now - lastLiftDebounceTime) > DEBOUNCE_DELAY) {
    Serial.print("L");Serial.println(currentState);
    if (currentState == LOW) {
      shouldStartTone = true;
    } else {
      // Hang up, reset all
      pulseCount = 0;
      dialStarted = false;
      dialEnded = false;
      shouldStopAnyAudio = true;
    }
    lastLiftContactState = currentState;
    lastLiftDebounceTime = now;
  }
}

void handleDial() {
  unsigned long now = millis();

  if ((now - lastDialDebounceTime) > DEBOUNCE_DELAY) {
    if (pulseCount > 0) {
      dialEnded = true;
    }
    lastDialDebounceTime = now;
  }
}

void handlePulse() {
  unsigned long now = millis();

  if ((now - lastPulseDebounceTime) > DEBOUNCE_DELAY) {
    if (digitalRead(DIAL_PIN) == LOW) {
      dialStarted = true;
      pulseCount++;
      lastPulseDebounceTime = now;
    }
  }
}

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println(F("USB Inserted!"));
      break;
    case DFPlayerUSBRemoved:
      Serial.println(F("USB Removed!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  
}