#include "Servo_ctrl.h"
#include "Camera.h"

TaskHandle_t servoTaskHandle = NULL;

const int servoPin1 = 15; //Vertical servo
const int servoPin2 = 23; //Horizontal servo

static float angle_h, angle_v;

// Canaux PWM pour ESP32
const int channel1 = 8; //Vertical servo
const int channel2 = 9; //Horizontal servo

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



void set_v_angle(){
    ledcWrite(channel1, usToDuty(angleToUs(angle_v)));
    //delay(20);
}

void set_h_angle(){
    ledcWrite(channel2, usToDuty(angleToUs(angle_h)));
    //delay(20);
}

void set_v_angle(int angle){
    angle_v = angle;
    ledcWrite(channel1, usToDuty(angleToUs(angle)));
    //delay(20);
}

void set_h_angle(int angle){
    angle_h = angle;
    ledcWrite(channel2, usToDuty(angleToUs(angle)));
    //delay(20);
}

void update_angles(float h_offset, float v_offset){
    if(fabs(v_offset) > MAX_SERVO_MVMNT){
        angle_h += float(MAX_SERVO_MVMNT)*(fabs(v_offset)/v_offset);
    }else{
        angle_h += v_offset;
    }

    if(fabs(h_offset) > MAX_SERVO_MVMNT){
        angle_v -= float(MAX_SERVO_MVMNT)*(fabs(h_offset)/h_offset);
    }else{
        angle_v -= h_offset;
    }

    if(angle_v > ANGLE_MAX){angle_v = ANGLE_MAX;}
    if(angle_v < ANGLE_MIN){angle_v = ANGLE_MIN;}

    if(angle_h > ANGLE_MAX){angle_h = ANGLE_MAX;}
    if(angle_h < ANGLE_MIN){angle_h = ANGLE_MIN;}
    
    //Serial.println("------------------------------");
    //Serial.println("Servo angles: ");
    //Serial.print("h_angle =  "); Serial.println(angle_h);
    //Serial.print("v_angle =  "); Serial.println(angle_v);
    set_h_angle();
    set_v_angle();
}

void servoTask(void *pvParameters){
    while (true) {
        if(xSemaphoreTake(getServoSemaphore(), portMAX_DELAY) == pdTRUE) {
            update_angles(get_h_offset(), get_v_offset());
        }
    }
}

void setup_servos(){
    // Config PWM
    ledcSetup(channel1, freq, resolution);
    ledcSetup(channel2, freq, resolution);

    // Associer pins
    ledcAttachPin(servoPin1, channel1);
    ledcAttachPin(servoPin2, channel2);
    
    angle_h = 90; angle_v = 90;
    set_h_angle(); set_v_angle();

    xTaskCreatePinnedToCore(
        servoTask,
        "Servo Task",
        2048,
        NULL,
        1,               // priorité plus basse
        &servoTaskHandle,
        1                // Core 1
    );

    Serial.println("PWM prêt sur GPIO 23 et 15");
}

