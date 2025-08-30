#include "Servo_ctrl.h"
#include "Camera.h"

TaskHandle_t servoTaskHandle = NULL;

static float angle_h, angle_v;


// Convertit un angle [0–180] en largeur d’impulsion (µs)
int angleToUs(int angle) {
    int usMin = 500;   // impulsion min typique
    int usMax = 2400;  // impulsion max typique
    return usMin + (usMax - usMin) * angle / 180;
}

// Convertit largeur d’impulsion (µs) → valeur 16 bits PWM
uint32_t usToDuty(int us) {
    uint32_t dutyMax = (1 << RESOLUTION) - 1; // 65535
    return (uint32_t)((float)us / 20000.0 * dutyMax); // période = 20 ms
}


void set_v_angle(){
    ledcWrite(CHANNEL1, usToDuty(angleToUs(angle_v)));
}

void set_h_angle(){
    ledcWrite(CHANNEL2, usToDuty(angleToUs(angle_h)));
}

void set_v_angle(int angle){
    angle_v = angle;
    ledcWrite(CHANNEL1, usToDuty(angleToUs(angle)));
}

void set_h_angle(int angle){
    angle_h = angle;
    ledcWrite(CHANNEL2, usToDuty(angleToUs(angle)));
}

void update_angles(float h_offset, float v_offset){
    //if(vertical_offset < MIN_ERROR){vertical_offset = 0;}
    //if(horizontal_offset < MIN_ERROR){horizontal_offset = 0;}
    
    if(fabs(h_offset) > MAX_SERVO_MVMNT){
        angle_h += float(MAX_SERVO_MVMNT)*(fabs(h_offset)/h_offset);
    }else{
        angle_h += h_offset;
    }

    if(fabs(v_offset) > MAX_SERVO_MVMNT){
        angle_v -= float(MAX_SERVO_MVMNT)*(fabs(v_offset)/v_offset);
    }else{
        angle_v -= v_offset;
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
        if(xSemaphoreTake(getServoSemaphore(), portMAX_DELAY) == pdTRUE){
            update_angles(get_h_offset(), get_v_offset());
        }
    }
}

void setup_servos(){
    // Config PWM
    ledcSetup(CHANNEL1, FREQ, RESOLUTION);
    ledcSetup(CHANNEL2, FREQ, RESOLUTION);

    // Associer pins
    ledcAttachPin(SERVOPIN1, CHANNEL1);
    ledcAttachPin(SERVOPIN2, CHANNEL2);
    
    set_h_angle(90); set_v_angle(90);

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

