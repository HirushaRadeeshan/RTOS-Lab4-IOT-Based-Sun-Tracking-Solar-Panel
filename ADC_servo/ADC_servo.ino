#include <ESP32Servo.h>

#define LDRpin1 25 // pin where we connected the LDR and the resistor
#define LDRpin2 26 // pin where we connected the LDR and the resistor

// Task handles
static TaskHandle_t task_ADC = NULL;
static TaskHandle_t task_Rotate = NULL;

Servo myservo;  // create servo object to control a servo
int rotate_angle = 10;
int servoPin = 18;

int LDRValue1 = 0;     // result of reading the analog pin
int LDRValue2 = 0;     // result of reading the analog pin

void TaskADC(void *parameters) {

  while(1){             //starting an infinity loop
    analogReadResolution(10);
    LDRValue1 = analogRead(LDRpin1); // read the value from the LDR
    LDRValue2 = analogRead(LDRpin2); // read the value from the LDR
  }  
}

void TaskROTATE(void *parameters) {

  while(1){    
    
    if(LDRValue1 < LDRValue2){    //rotate left
        Serial.println("LDRValue1 < LDRValue2");
        vTaskDelay(pdMS_TO_TICKS(10));
        while(( ((LDRValue2-50) < LDRValue1) & (LDRValue1 < (LDRValue2+50)) ) == 0){
          Serial.print("LDRValue1 = ");
          Serial.println(LDRValue1);
          Serial.print("LDRValue2 = ");
          Serial.println(LDRValue2);
          Serial.println("(LDRValue2-50) < LDRValue1) & (LDRValue1 < (LDRValue2+50)");
            if(rotate_angle > 10){
              Serial.println("rotate_angle > 10");
              rotate_angle = rotate_angle - 3;
              myservo.write(rotate_angle);
            }
            vTaskDelay(pdMS_TO_TICKS(10));
            
        }
    
    }
    if(LDRValue1 > LDRValue2){
        Serial.println("LDRValue1 > LDRValue2");
        vTaskDelay(pdMS_TO_TICKS(10));
        while(( ((LDRValue1-50) < LDRValue2) & (LDRValue2 < (LDRValue1+50)) ) == 0){
          Serial.print("LDRValue1 = ");
          Serial.println(LDRValue1);
          Serial.print("LDRValue2 = ");
          Serial.println(LDRValue2);
          Serial.println("(LDRValue1-50) < LDRValue2) & (LDRValue2 < (LDRValue1+50)");
            if(rotate_angle < 180){
              Serial.println("rotate_angle < 180");
              rotate_angle = rotate_angle + 3;
              myservo.write(rotate_angle);
            }
            vTaskDelay(pdMS_TO_TICKS(10));
            
        }
    
    }
    //vTaskDelay(pdMS_TO_TICKS( 2000 ));    //giving a delay
  }  
}


void setup() {
  Serial.begin(115200);

  pinMode(LDRpin1,INPUT);
  pinMode(LDRpin2,INPUT);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 1000, 2000); // attaches the servo on pin 18 to the servo object
  // using default min/max of 1000us and 2000us
  // different servos may require different min/max settings
  // for an accurate 0 to 180 sweep
  myservo.write(rotate_angle);

  // Start task 1
  xTaskCreatePinnedToCore(TaskADC,    //function
                          "Task ADC Core0", //name of the task
                          1024,         //stack size(bytes)
                          NULL,         //parameter to pass to function
                          1,            //giving lower priority
                          &task_ADC,       //task handle
                          0);           //Core ID

  xTaskCreatePinnedToCore(TaskROTATE,    //function
                          "Task Rotate Core0", //name of the task
                          6000,         //stack size(bytes)
                          NULL,         //parameter to pass to function
                          1,            //giving lower priority
                          &task_Rotate,       //task handle
                          0);           //Core ID
}

void loop() {

}
