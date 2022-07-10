#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x3F for a 16 chars and 2 line display

//static TaskHandle_t task;
 
void printLCD(void *parameter){
  
  lcd.backlight();      // Make sure backlight is on
  lcd.init();
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on
  
  while(1){
    // Print a message on both lines of the LCD.
    lcd.setCursor(2,0);   //Set cursor to character 2 on line 0
    lcd.print("Hello world");
  
    lcd.setCursor(2,1);   //Move cursor to character 2 on line 1
    lcd.print("LCD Tutorial");


  } 
}

void setup(){
  //Serial.begin(115200);
  lcd.init();
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on

  xTaskCreatePinnedToCore(printLCD,
                          "printLCD",
                          2000,
                          NULL,
                          1,
                          NULL,
                          0);
}

void loop(){
   
}
