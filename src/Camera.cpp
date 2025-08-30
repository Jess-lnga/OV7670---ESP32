#include "Camera.h"

static SemaphoreHandle_t servoSemaphore = NULL;
static TaskHandle_t cameraTaskHandle = NULL;

static bool pxlIsRed[HEIGHT*WIDTH];

static uint16_t* frame;
static size_t len;
static float horizontal_offset, vertical_offset;
static float x_ball(0), y_ball(0), r_ball(0);

static float mvg_avg_pos_x[WEIGHT_WINDOW];
static float mvg_avg_pos_y[WEIGHT_WINDOW];
static int   pos_in_mvg_avg_vector;

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





uint16_t* getFrame(){
    return frame;
}

size_t getLen(){
    return len;
}

SemaphoreHandle_t getServoSemaphore(){
    return servoSemaphore;
}

float get_h_offset(){
    return horizontal_offset;
}

float get_v_offset(){
    return vertical_offset;
}

void compute_mean_pos(){
    
    x_ball = 0; y_ball = 0;

    for(int i(0); i < WEIGHT_WINDOW; ++i){
        x_ball += mvg_avg_pos_x[i];
        y_ball += mvg_avg_pos_y[i];
    }
    

    x_ball /= float(WEIGHT_WINDOW);
    y_ball /= float(WEIGHT_WINDOW);
    

    vertical_offset   = float(WIDTH/2.0)  - x_ball;
    horizontal_offset = float(HEIGHT/2.0) - y_ball ;

    //if(vertical_offset < MIN_ERROR){vertical_offset = 0;}
    //if(horizontal_offset < MIN_ERROR){horizontal_offset = 0;}


    //Serial.println("------------------------------");
    //Serial.println("Pos: ");
    //Serial.print("x = "); Serial.println(x_ball);
    //Serial.print("y = "); Serial.println(y_ball);

    //Serial.println("Offsets: ");
    //Serial.print("h_offset = "); Serial.println(horizontal_offset);
    //Serial.print("v_offset = "); Serial.println(vertical_offset);
}

void add_pos_to_mean(float x, float y){
    mvg_avg_pos_x[pos_in_mvg_avg_vector] = x;
    mvg_avg_pos_y[pos_in_mvg_avg_vector] = y;

    if(pos_in_mvg_avg_vector == WEIGHT_WINDOW - 1){
        pos_in_mvg_avg_vector = 0;
    }else{
        ++pos_in_mvg_avg_vector;
    }
}

void detection()
{
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
    //float x_ball(0), y_ball(0), r_ball(0);
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

    if(count > 0){
        x_ball /= count;
        y_ball /= count;
        r_ball /= count;
    }

    if(count > 0){
        add_pos_to_mean(x_ball, y_ball);
        compute_mean_pos();
    
        //Serial.println("------------------------------");
        //Serial.println("Pos: ");
        //Serial.print("x = "); Serial.println(x_ball);
        //Serial.print("y = "); Serial.println(y_ball);
        
    }else{
        x_ball = (WIDTH)/2.0; y_ball = HEIGHT/2.0; r_ball = 2;
        add_pos_to_mean(x_ball, y_ball);
        compute_mean_pos();

    }

    drawRect(frame, int(x_ball - r_ball), int(y_ball - r_ball), 
                        int(x_ball + r_ball), int(y_ball + r_ball));

}

void takeImage(bool detect){
    camera.oneFrame();
    frame = (uint16_t*) I2SCamera::frame; // RGB565
    len = I2SCamera::frameBytes; // nombre de pixels

    if(detect){detection();}
}

void cameraTask(void *pvParameters) {
    const TickType_t xFrequency = pdMS_TO_TICKS(80); // 80 ms
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (true) {
        takeImage(true);  // capture et traitement
        xSemaphoreGive(servoSemaphore); // signaler que les offsets sont prêts


        vTaskDelayUntil(&xLastWakeTime, xFrequency); // fréquence fixe
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

    for(int i(0); i < WEIGHT_WINDOW; ++i){
        mvg_avg_pos_x[i] = 0;
        mvg_avg_pos_y[i] = 0;
    }

    x_ball = 0; y_ball = 0; r_ball = 0;
    pos_in_mvg_avg_vector = 0;

    servoSemaphore = xSemaphoreCreateBinary();

    xTaskCreatePinnedToCore(
        cameraTask,      // fonction
        "Camera Task",   // nom
        4096,            // stack
        NULL,            // param
        2,               // priorité (plus haute que servos)
        &cameraTaskHandle,
        0                // Core 0 (laisser WiFi tourner sur core 1)
    );

  Serial.println("Caméra prête !");
}



