#include "wifiCom.h"
#include "Camera.h"
#include "Servo_ctrl.h"

void setup() {
  Serial.begin(115200);

  setupCam();
  setup_servos();
  //setupWifi();
}

void loop() {}

