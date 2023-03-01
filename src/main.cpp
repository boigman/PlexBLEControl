#include <Arduino.h>
/**
 * This example turns the ESP32 into a Bluetooth LE keyboard that writes the words, presses Enter, presses a media key and then Ctrl+Alt+Delete
 */
#include <BleKeyboard.h>
#include <ezButton.h>
#define ARYSIZE 5;

BleKeyboard bleKeyboard("Plex BLE Control","Staufware",80);  //("ESP32_BLE", "Plex BLE Control", 80);
// int rstate = 0;
// int lstate = 0;
// int last_rstate = 0;
// int last_lstate = 0;
int threshold = 25;
// int leftRead = 0;
// int rightRead = 0;
int redLed = 13;
int greenLed = 9;
int arySize = 5;
ezButton button(36);  // create ezButton object that attach to pin 36;
//const int btnPin =  36;
int btnState;
int btnLastState;
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers
int kbdMode = 0;
int connState = 0;
int lastConnState = 0;



// boolean rTouchStarted = false;
// boolean lTouchStarted = false;
boolean touchStarted[5];
unsigned long touchTime[5];
//int touchPorts[] = {32, 33, 27, 14, 12};;
int touchPorts[] = {12, 14, 27, 33, 32};


// unsigned long rTouchTime = 0;
// unsigned long lTouchTime = 0;
int touchMinDuration = 100;

void setup() {
  Serial.begin(115200);
//  pinMode(btnPin, INPUT_PULLDOWN);
//  pinMode(btnPin, INPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  btnLastState = 1;
  delay(1000); // give me time to bring up serial monitor
  Serial.println("Starting Plex BLE Control!");
  bleKeyboard.begin();
  btnState = btnLastState;
  if(btnState) {
    Serial.println("button is high");
  } else {
    Serial.println("button is low");
  }

  for(int i=0;i<arySize;i++) {
    touchStarted[i] = false;
  }
  for(int i=0; i<5; i++) {
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, LOW);
    delay(200);
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);
    delay(200);
  }
  if(kbdMode) {
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);
    Serial.println("green is on");
  } else {
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, LOW);
    Serial.println("red is on");
  }
  Serial.print("kbdMode=");
  Serial.println(kbdMode);
}

void loop() {
  if(bleKeyboard.isConnected()) {
      connState = 1;
      if (connState!=lastConnState) {
        Serial.println("Connected!");
      }
      lastConnState = connState;
     for(int i=0;i<arySize;i++) {
      int t = touchRead(touchPorts[i]);
      if (t < threshold && !touchStarted[i]) { // on fresh touch
        touchStarted[i] = true;
        touchTime[i] = millis();
      } else if (t >= threshold && touchStarted[i]) { // untouched
        if (millis() - touchTime[i] > touchMinDuration) {
          switch (i) {
            case 0:
              // back 1 minute statements
              Serial.println("Back 1 minute");
              for(int j=0;j<6;j++) {
                bleKeyboard.write(KEY_LEFT_ARROW);
                delay(10);
              }
              break;
            case 1:
              // back 10 seconds statements
              Serial.println("Back 10 seconds");
              bleKeyboard.write(KEY_LEFT_ARROW);
              break;
            case 2:
              // Play/pause statements
              Serial.println("Play/Pause");
              bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
              break;
            case 3:
              // ahead 30 seconds statements
              Serial.println("Ahead 30 seconds");
              bleKeyboard.write(KEY_RIGHT_ARROW);
              break;
            case 4:
              // ahead 3 minutes statements
              Serial.println("Ahead 3 minutes");
              for(int j=0;j<6;j++) bleKeyboard.write(KEY_RIGHT_ARROW);
              break;
          }        
        }
        touchStarted[i] = false;
      }
    }
    button.loop(); // MUST call the loop() function first
    btnState = button.getState();
    if(btnState!=btnLastState) {
      if(btnState) {
//        Serial.println("button is high");
        kbdMode++;
        kbdMode = kbdMode % 2;
        Serial.print("kbdMode=");
        Serial.println(kbdMode);
        if(kbdMode) {
          digitalWrite(redLed, LOW);
          digitalWrite(greenLed, HIGH);
//          Serial.println("red is off");
          Serial.println("green is on");
        } else {
          digitalWrite(redLed, HIGH);
          digitalWrite(greenLed, LOW);
          Serial.println("red is on");
//          Serial.println("green is off");
        }
      } else {
//        Serial.println("button is low");
      }
    }
    btnLastState = btnState;
  }
//    delay(10);
}
