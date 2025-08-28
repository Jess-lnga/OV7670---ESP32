#include "wifiCom.h"
#include "Camera.h"
//#include <ESP32Servo.h>

//Servo servo1;
//Servo servo2;

const int servoPin1 = 15;
const int servoPin2 = 23;

// Canaux PWM pour ESP32
const int channel1 = 8;
const int channel2 = 9;

// Fréquence et résolution PWM
const int freq = 50;        // 50 Hz pour servos
const int resolution = 16;  // 16 bits de résolution (0–65535)

// Convertit un angle [0–180] en largeur d’impulsion (µs)
int angleToUs(int angle) {
  int usMin = 500;   // impulsion min typique
  int usMax = 2400;  // impulsion max typique
  return usMin + (usMax - usMin) * angle / 180;
}

// Convertit largeur d’impulsion (µs) → valeur 16 bits PWM
uint32_t usToDuty(int us) {
  uint32_t dutyMax = (1 << resolution) - 1; // 65535
  return (uint32_t)((float)us / 20000.0 * dutyMax); // période = 20 ms
}

int angle(61);
bool increment(true);


void setup() {
  Serial.begin(115200);

  // Config PWM
  ledcSetup(channel1, freq, resolution);
  ledcSetup(channel2, freq, resolution);

  // Associer pins
  ledcAttachPin(servoPin1, channel1);
  ledcAttachPin(servoPin2, channel2);

  Serial.println("PWM prêt sur GPIO 23 et 15");

  setupCam();
  setupWifi();
}


void loop() {
  /*  
  // Balayage servo 1
  for (int angle = 60; angle <= 120; angle += 5) {
    ledcWrite(channel1, usToDuty(angleToUs(angle)));
    delay(20);
  }
  for (int angle = 120; angle >= 60; angle -= 5) {
    ledcWrite(channel1, usToDuty(angleToUs(angle)));
    delay(20);
  }

  // Balayage servo 2
  for (int angle = 60; angle <= 120; angle += 5) {
    ledcWrite(channel2, usToDuty(angleToUs(angle)));
    delay(20);
  }
  for (int angle = 120; angle >= 60; angle -= 5) {
    ledcWrite(channel2, usToDuty(angleToUs(angle)));
    delay(20);
  }
  */
  
  reconnect();
  Serial.println("Taking Img");
  takeImage();
  Serial.println("Img taken");
  sendImageWifi(getFrame(), getLen());
  Serial.println("Img sent");
  
  ledcWrite(channel2, usToDuty(angleToUs(angle)));
  delay(5);
  Serial.println("Angle set");
  
  if(increment){angle+=10;}else{angle-=10;}
  if(angle<=60){increment = true;}
  if(angle >= 120){increment = false;}

  //delay(5);
}


