#include <Arduino_LSM9DS1.h>

float threshold = 0.8;
float xavg, yavg, zavg;
int steps;
unsigned long start_t, elapsed_t;
unsigned long crit_t=100;
bool st_flag = false, t_flag=false;

void setup(){
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Started");

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println("Hz");
  calibrate();

}

void loop(){
    if(elapsed_time_check()) step_count();
}

bool elapsed_time_check(){
    if(!t_flag){
        start_t = millis();
        t_flag = true;
    }else{
        elapsed_t = millis()-start_t;
        if(elapsed_t>=crit_t){
            t_flag = false;
            return true;
        }
    }
    return false;
}

void step_count(){
    float totvect = 0, prev_totvect = 0;
    float totave = 0;
    float xaccl = 0, yaccl = 0, zaccl = 0;

    if (IMU.accelerationAvailable()) IMU.readAcceleration(xaccl, yaccl, zaccl);
    xaccl-=xavg; yaccl-=yavg; zaccl-=zavg;
    
    totvect = sqrt(xaccl*xaccl + yaccl*yaccl + zaccl*zaccl);
    totave = (totvect + prev_totvect) / 2 ;
    prev_totvect = totvect;

    Serial.println("totave");
    Serial.println(totave);

    if(totave>threshold && !st_flag){
        steps++;
        st_flag = true;
    }

    if(totave<threshold && st_flag) st_flag = false;
    if(steps<0) steps = 0;

    Serial.println('\n');
    Serial.print("steps: ");
    Serial.println(steps);
}

void calibrate(){
  float xval, yval, zval;
  
  for (int i = 0; i < 100; i++) {
    if (IMU.accelerationAvailable()) IMU.readAcceleration(xval, yval, zval);
    xavg+= xval; yavg+= yval; zavg+= zval;
  }
  
  delay(100);
  xavg /= 100.0; yavg /= 100.0; zavg /= 100.0;
  Serial.println(xavg); Serial.println(yavg); Serial.println(zavg);
}
