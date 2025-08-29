#pragma once

#include <Arduino.h>

#define MAX_SERVO_MVMNT 2
#define ANGLE_MAX 180
#define ANGLE_MIN 0

void setup_servos();

void set_h_angle();
void set_v_angle();

void set_h_angle(int angle);
void set_v_angle(int angle);
void update_angles(float h_offset, float v_offset);