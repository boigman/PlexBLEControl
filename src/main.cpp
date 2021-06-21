#include <Arduino.h>
/**
 * This example turns the ESP32 into a Bluetooth LE keyboard that writes the words, presses Enter, presses a media key and then Ctrl+Alt+Delete
 */
#include <BleKeyboard.h>
#define ARYSIZE 5;

BleKeyboard bleKeyboard;
// int rstate = 0;
// int lstate = 0;
// int last_rstate = 0;
// int last_lstate = 0;
int threshold = 25;
// int leftRead = 0;
// int rightRead = 0;
int arySize = 5;


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
  delay(1000); // give me time to bring up serial monitor
  Serial.println("Starting Plex BLE Control!");
  bleKeyboard.begin();

  for(int i=0;i<arySize;i++) {
    touchStarted[i] = false;
  }
}

void loop() {
  if(bleKeyboard.isConnected()) {
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
    delay(10);
  }
}