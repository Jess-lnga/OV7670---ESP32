#pragma once

#include <Arduino.h>

#define MAX_SERVO_MVMNT 2
#define ANGLE_MAX 180
#define ANGLE_MIN 0

#define SERVOPIN1 15 //Vertical servo
#define SERVOPIN2 23 //Horizontal servo

#define CHANNEL1 8 //Vertical servo
#define CHANNEL2 9 //Horizontal servo

#define FREQ 50       // 50 Hz for servos
#define RESOLUTION 16 // 16 bits de résolution (0–65535)

void setup_servos();
