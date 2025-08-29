#include "wifiCom.h"
#include "Camera.h"
#include "Servo_ctrl.h"

int angle(61);
bool increment(true);

void setup() {
  Serial.begin(115200);
  
  setup_servos();
  setupCam();
  //setupWifi();
}


void loop() {
  //reconnect();
  takeImage(true);
  update_angles(get_h_offset(), get_v_offset());
  //sendImageWifi(getFrame(), getLen());
}

/*
#include <Arduino.h>

// Variables partagées
volatile bool wifiConnected = false;
volatile int ballX = -1;
volatile int ballY = -1;

// Handles des tâches
TaskHandle_t TaskWiFi;
TaskHandle_t TaskCamera;
TaskHandle_t TaskServos;

// --- Tâche WiFi ---
void wifiTask(void *pvParameters) {
  for (;;) {
    // Ici tu mets ta logique de reconnexion WiFi
    wifiConnected = true; // ou false selon état
    vTaskDelay(1000 / portTICK_PERIOD_MS); // 1 Hz
  }
}

// --- Tâche Caméra ---
void cameraTask(void *pvParameters) {
  for (;;) {
    // Ici tu prends une image et détectes la balle rouge
    ballX = 100;  // Exemple position
    ballY = 50;
    vTaskDelay(100 / portTICK_PERIOD_MS); // toutes les 100 ms
  }
}

// --- Tâche Servos ---
void servoTask(void *pvParameters) {
  for (;;) {
    // Ici tu écris ton PWM avec ledcWrite
    // Exemple : bouger servo en fonction de ballX
    if (wifiConnected) {
      // Suivi actif
    } else {
      // Position neutre
    }
    vTaskDelay(20 / portTICK_PERIOD_MS); // 50 Hz
  }
}

void setup() {
  Serial.begin(115200);

  // Créer les tâches
  xTaskCreatePinnedToCore(wifiTask, "WiFi", 4096, NULL, 1, &TaskWiFi, 0);
  xTaskCreatePinnedToCore(cameraTask, "Camera", 8192, NULL, 1, &TaskCamera, 0);
  xTaskCreatePinnedToCore(servoTask, "Servos", 4096, NULL, 1, &TaskServos, 1);

  Serial.println("Tâches lancées !");
}

void loop() {
  // Vide : tout se fait dans les tâches
}

*/


