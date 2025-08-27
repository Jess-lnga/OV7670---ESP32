#include "wifiCom.h"
#include "Camera.h"

void setup() {
  setupCam();
  setupWifi();
}


void loop() {
  reconnect();
  takeImage();
  sendImageWifi(getFrame(), getLen());
  delay(5);


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
}
