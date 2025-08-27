#include <Arduino.h>
#include <WiFi.h>
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

#define BAUD_RATE 115200 //921600

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

#define WIDTH  160
#define HEIGHT 120
#define FRAME_HEADER_WIFI 0xAABBCCDD
#define FRAME_FOOTER_WIFI 0xDDCCBBAA

bool pxlIsRed[HEIGHT*WIDTH];

// Couleur bleue en RGB565 (carré autour de la balle)
#define BLUE 0x001F  

// Détection "rouge"
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

// Paramètres WiFi
const char* ssid = "Redmi";
const char* password = "Ljhki23132";

// Socket TCP
WiFiServer server(8080);
WiFiClient client;

void setup() {
  Serial.begin(BAUD_RATE);
  delay(1000);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connecté");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.begin();

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


void loop() {

  //Attente de connexion
  if (!client || !client.connected()) {
    client = server.available();
    Serial.println("client is not available");
    delay(1000);
    return;
  }
  camera.oneFrame();

  uint16_t* frame = (uint16_t*) I2SCamera::frame; // RGB565
  size_t len = I2SCamera::frameBytes; // nombre de pixels
  
  
  //uint32_t header = FRAME_HEADER_WIFI;
  //uint32_t footer = FRAME_FOOTER_WIFI;

  uint16_t header = 0xABCD;
  uint16_t footer = 0xDCBA;

  //Serial.print("Image was captured successfully. Size of the picture: ");
  //Serial.println(len);

  // Taille totale
  size_t totalSize = sizeof(header) + len + sizeof(footer);
  uint8_t* buffer = (uint8_t*) malloc(totalSize);

  if (buffer) {
    memcpy(buffer, &header, sizeof(header));              // header = 2 octets
    memcpy(buffer + sizeof(header), frame, len);          // image
    memcpy(buffer + sizeof(header) + len, &footer, sizeof(footer)); // footer

    client.write(buffer, totalSize);
    free(buffer);
  }


  //client.write((uint8_t*)frame, len);
  //client.write((uint8_t*)&header, sizeof(header));
  //client.write((uint8_t*)&footer, sizeof(footer));

  delay(5); // petit délai pour ne pas saturer le flux




  /*

  float x_centroid(0), y_centroid(0);
  float count(0);
  

  for (int y = 0; y < HEIGHT; y++) 
  {
    for (int x = 0; x < WIDTH; x++) 
    {
      uint16_t pixel = frame[y * WIDTH + x];
      if (isRed(pixel)){
        pxlIsRed[y * WIDTH + x] = true;
        ++count;
        x_centroid += x; y_centroid += y;
      }else{
        pxlIsRed[y * WIDTH + x] = false;
        //frame[y * WIDTH + x] = 0xFFFF;  // Blanc en RGB565
      }
    }
  }

  if(count > 0){x_centroid /= count; y_centroid /= count;}

  float dist(0), dist2(0);

  for (int y = 0; y < HEIGHT; y++){
    for (int x = 0; x < WIDTH; x++){
      if(pxlIsRed[y * WIDTH + x]){
        float staff_d(sqrt(pow(x-x_centroid, 2)+pow(y-y_centroid, 2)));
        dist += staff_d;
        dist2 += staff_d*staff_d; 
      }
    }
  }

  float sigma = 0;
  
  if(count > 0){
    float mu = dist/count;
    float variance = (dist2/count) - mu*mu;
    sigma = sqrt(max(variance, 0.0f));
  }

  
  dist /= count;
  float x_ball(0), y_ball(0), r_ball(0);
  count = 0;

  for (int y = 0; y < HEIGHT; y++){
    for (int x = 0; x < WIDTH; x++){
      if(pxlIsRed[y * WIDTH + x]){
        float staff_dist(sqrt(pow(x-x_centroid, 2)+pow(y-y_centroid, 2)));

        if(fabs(staff_dist - dist) < 2*sigma){
          x_ball += x;
          y_ball += y;
          r_ball += staff_dist;
          ++count;
        }
      }
    }
  }

  if(count > 0)
  {
    x_ball /= count;
    y_ball /= count;
    r_ball /= count;
  }

  if(count > 0)
  {
    drawRect(frame, int(x_ball - r_ball), int(y_ball - r_ball), 
                    int(x_ball + r_ball), int(y_ball + r_ball));
  }
  */
  


  // Envoi vers PC
  //Serial.write(FRAME_HEADER, sizeof(FRAME_HEADER));
  //Serial.write((uint8_t*)frame, len * 2); // chaque pixel = 2 octets
  //Serial.write(FRAME_FOOTER, sizeof(FRAME_FOOTER));
}


//