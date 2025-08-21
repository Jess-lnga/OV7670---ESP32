#include <Arduino.h>
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

// Résolution = QQVGA = 160x120 RGB565
OV7670 camera(
  OV7670::QQVGA_RGB565,
  PIN_SIOD, PIN_SIOC,
  PIN_VSYNC, PIN_HREF,
  PIN_XCLK, PIN_PCLK,
  PIN_D0, PIN_D1, PIN_D2, PIN_D3, PIN_D4, PIN_D5, PIN_D6, PIN_D7
);

const uint8_t FRAME_HEADER[4] = {0xAA, 0xBB, 0xCC, 0xDD};
const uint8_t FRAME_FOOTER[4] = {0xDD, 0xCC, 0xBB, 0xAA};

void setup() 
{
  Serial.begin(921600);  //    115200);
  delay(1000);

  Serial.println("Initialisation de la caméra...");

  // Initialise I2S + VSYNC, etc.
  I2SCamera::init(
    160, 120,            // résolution choisie
    PIN_VSYNC, PIN_HREF, PIN_XCLK,
    PIN_PCLK,
    PIN_D0, PIN_D1, PIN_D2, PIN_D3,
    PIN_D4, PIN_D5, PIN_D6, PIN_D7
  );

  Serial.println("Caméra prête !");
}

void loop(){
  Serial.println("Capture image...");

  //unsigned long t0 = millis();
  // Capture une seule frame == Image complète
  camera.oneFrame();
  
  // Récupérer les données dans I2SCamera::frame
  uint8_t* frame = I2SCamera::frame;
  size_t len = I2SCamera::frameBytes;

  //unsigned long t1 = millis();
  //Serial.printf("Capture time: %lu ms\n", t1 - t0);

  

  // Envoie en binaire direct à l'ordinateur
  //Serial.println("FRAME_START");
  //Serial.write(frame, len);
  //Serial.println("FRAME_END");

  Serial.write(FRAME_HEADER, sizeof(FRAME_HEADER));
  Serial.write(frame, len);
  Serial.write(FRAME_FOOTER, sizeof(FRAME_FOOTER));
  
  delay(10);
}

/*
unsigned long t0 = millis();
// --- ton code ---
unsigned long t1 = millis();
Serial.printf("Temps écoulé: %lu ms\n", t1 - t0);
*/

/*
  Serial.printf("Frame capturée (%d octets)\n", len);

  // Exemple : afficher les 16 premiers octets
  for (int i = 0; i < 16 && i < len; i++) {
    Serial.printf("%02X ", frame[i]);
  }
  Serial.println();

  delay(1000);
*/