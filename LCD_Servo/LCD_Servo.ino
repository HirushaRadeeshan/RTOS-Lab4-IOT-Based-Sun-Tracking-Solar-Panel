#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo myservo;  // create servo object to control a servo
int rotate_angle = 85;
int servoPin = 18;
int real_angle = 0;

void Subroutine_1(void *parameter);
void TaskADC(void *parameters);
void TaskROTATE(void *parameters);

static TaskHandle_t task_1 = NULL;
static TaskHandle_t task_ADC = NULL;
static TaskHandle_t task_Rotate = NULL;

int LDRValue1 = 0;     // result of reading the analog pin
int LDRValue2 = 0;     // result of reading the analog pin
int LDRpin1 = 25;      // pin where we connected the LDR and the resistor
int LDRpin2 = 26;      // pin where we connected the LDR and the resistor

int current_sensor = 35;
float adc_voltage = 0.0;
float Voltage = 0.0;
float Current = 0.0;
float R1 = 30000.0;
float R2 = 7500.0;
float Reference_Voltage = 3.3;


void setup(){
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);

  pinMode(current_sensor,INPUT);
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
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Current = ");
  lcd.print(Current);
  lcd.print("mA");
  lcd.setCursor(0, 1);
  lcd.print("Angle = ");
  lcd.print(real_angle);
  
  vTaskDelay(pdMS_TO_TICKS(1000));
  
  
  xTaskCreatePinnedToCore(Subroutine_1,
                          "Task DISPLAY Core1",
                          2500,
                          NULL,
                          1,
                          &task_1,
                          1);
  
  xTaskCreatePinnedToCore(TaskADC,
                          "Task ADC Core1",
                          2500,
                          NULL,
                          1,
                          &task_ADC,
                          1);
  
  xTaskCreatePinnedToCore(TaskROTATE,    
                          "Task Rotate Core1", 
                          6000,         
                          NULL,         
                          1,          
                          &task_Rotate,       
                          1); 
}

void loop(){
  
}

void Subroutine_1(void *parameter) {
  lcd.setCursor(3,0);
  lcd.print("RTOS LAB04");
  vTaskDelay(pdMS_TO_TICKS(1000));
  lcd.clear();
  
  while (1) {
    analogReadResolution(10);
    float current_sensor_adc_value = analogRead(current_sensor);
    adc_voltage  = (current_sensor_adc_value * Reference_Voltage) / 1024.0; 
    Voltage = adc_voltage / (R2/(R1+R2));
    Current = (Voltage / 1000)*1000;
    display_current();
  }
}

void display_current(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Current = ");
  lcd.print(Current);
  lcd.print("mA");
  lcd.setCursor(0, 1);
  lcd.print("Angle = ");
  lcd.print(real_angle);
  
  vTaskDelay(pdMS_TO_TICKS(500));
  lcd.clear();
}


void TaskADC(void *parameters) {

  while(1){
    analogReadResolution(10);
    LDRValue1 = analogRead(LDRpin1);      // read the value from the LDR
    analogReadResolution(10);
    LDRValue2 = analogRead(LDRpin2);      // read the value from the LDR
      
  }  
}


void TaskROTATE(void *parameters) {

  while(1){    
    
    if(LDRValue1 < LDRValue2){    //rotate left
        vTaskDelay(pdMS_TO_TICKS(10));
        while(( ((LDRValue2-50) < LDRValue1) & (LDRValue1 < (LDRValue2+50)) ) == 0){
            if(rotate_angle > 10){
              rotate_angle = rotate_angle - 3;
              myservo.write(rotate_angle);
              real_angle = 45 - (rotate_angle/2);
            }
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    if(LDRValue1 > LDRValue2){

        vTaskDelay(pdMS_TO_TICKS(10));
        while(( ((LDRValue1-50) < LDRValue2) & (LDRValue2 < (LDRValue1+50)) ) == 0){
            if(rotate_angle < 180){
              rotate_angle = rotate_angle + 3;
              myservo.write(rotate_angle);
              real_angle = 45 - (rotate_angle/2);
            }
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
  }  
}
