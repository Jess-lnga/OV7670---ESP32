#include "Camera.h"

bool pxlIsRed[HEIGHT*WIDTH];
uint16_t* frame;
size_t len;

// Résolution = QQVGA = 160x120 RGB565
OV7670 camera(
  OV7670::QQVGA_RGB565,
  PIN_SIOD, PIN_SIOC,
  PIN_VSYNC, PIN_HREF,
  PIN_XCLK, PIN_PCLK,
  PIN_D0, PIN_D1, PIN_D2, PIN_D3, PIN_D4, PIN_D5, PIN_D6, PIN_D7
);

bool isRed(uint16_t pixel) {
    uint8_t r = (pixel >> 11) & 0x1F;  // [0..31]
    uint8_t g = (pixel >> 5) & 0x3F;   // [0..63]
    uint8_t b = pixel & 0x1F;          // [0..31]

    // Critère souple
    return (r > 15 &&                 // Assez de rouge
            r > g / 2 + 8 &&          // Rouge > Vert (avec tolérance)
            r > b / 2 + 8);           // Rouge > Bleu (avec tolérance)
}

void drawRect(uint16_t* frame, int x0, int y0, int x1, int y1) {
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 >= WIDTH) x1 = WIDTH - 1;
    if (y1 >= HEIGHT) y1 = HEIGHT - 1;

    for (int x = x0; x <= x1; x++) {
        frame[y0 * WIDTH + x] = BLUE;
        frame[y1 * WIDTH + x] = BLUE;
    }
    for (int y = y0; y <= y1; y++) {
        frame[y * WIDTH + x0] = BLUE;
        frame[y * WIDTH + x1] = BLUE;
    }
}

void setupCam(){
    Serial.println("Initialisation de la caméra...");

    I2SCamera::init(
        WIDTH, HEIGHT,
        PIN_VSYNC, PIN_HREF, PIN_XCLK,
        PIN_PCLK,
        PIN_D0, PIN_D1, PIN_D2, PIN_D3,
        PIN_D4, PIN_D5, PIN_D6, PIN_D7
    );

  Serial.println("Caméra prête !");
}

void takeImage(){
    camera.oneFrame();
    frame = (uint16_t*) I2SCamera::frame; // RGB565
    len = I2SCamera::frameBytes; // nombre de pixels
}

uint16_t* getFrame(){
    return frame;
}

size_t getLen(){
    return len;
}



