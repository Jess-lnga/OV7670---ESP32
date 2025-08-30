#pragma once
#include "OV7670.h"

#define PIN_SIOD 21
#define PIN_SIOC 22
#define PIN_VSYNC 26
#define PIN_HREF 27
#define PIN_XCLK 0
#define PIN_PCLK 25
#define PIN_D0 32
#define PIN_D1 33
#define PIN_D2 34
#define PIN_D3 35
#define PIN_D4 36
#define PIN_D5 39
#define PIN_D6 18
#define PIN_D7 19

#define CAM_FREQ 80  //ms
#define WIDTH  160
#define HEIGHT 120

#define BLUE 0x001F

#define WEIGHT_WINDOW 2
#define MIN_ERROR 5
/////////////////////////////////////////////////

//bool isRed(uint16_t pixel);
//void drawRect(uint16_t* frame, int x0, int y0, int x1, int y1);

void setupCam();
//void takeImage(bool detect);
//void detection();

uint16_t* getFrame();
size_t getLen();
SemaphoreHandle_t getServoSemaphore();

float get_h_offset();
float get_v_offset();