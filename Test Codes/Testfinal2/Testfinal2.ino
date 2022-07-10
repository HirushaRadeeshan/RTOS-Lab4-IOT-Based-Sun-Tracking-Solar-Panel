#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"


// Network credentials
const char* ssid = "EN20417284";
const char* password = "12345678";

AsyncWebServer server(80);
LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo myservo;  // create servo object to control a servo
int rotate_angle = 85;
int servoPin = 18;
int real_angle = 0;

void TaskCURRENT_MEASURING(void *parameter);
void TaskDISPLAY(void *parameter);
void TaskADC(void *parameters);
void TaskROTATE(void *parameters);

static TaskHandle_t task_DISPLAY = NULL;
static TaskHandle_t task_CURRENT_MEASURING = NULL;
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
const char index_html[];

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);

  pinMode(current_sensor, INPUT);
  pinMode(LDRpin1, INPUT);
  pinMode(LDRpin2, INPUT);

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

  lcd.setCursor(3, 0);
  lcd.print("RTOS LAB04");
  vTaskDelay(pdMS_TO_TICKS(1000));
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Current = ");
  lcd.print(Current);
  lcd.print("mA");
  lcd.setCursor(0, 1);
  lcd.print("Angle = ");
  lcd.print(real_angle);

  vTaskDelay(pdMS_TO_TICKS(1000));
  
    // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html, );
  });
  server.on("/rotation", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(real_angle).c_str());
  });
  server.on("/current", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(Current).c_str());
  });

  // Start server
  server.begin();


  xTaskCreatePinnedToCore(TaskDISPLAY,
                          "Task DISPLAY Core1",
                          2500,
                          NULL,
                          1,
                          &task_DISPLAY,
                          1);

  xTaskCreatePinnedToCore(TaskCURRENT_MEASURING,
                          "Task CURRENT_MEASURING Core1",
                          2500,
                          NULL,
                          1,
                          &task_CURRENT_MEASURING,
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

  xTaskCreatePinnedToCore(TaskConnectToNet,
                          "IoT",
                          6000,
                          NULL,
                          1,
                          NULL,
                          0);
                          
}

void loop() {

}

void TaskCURRENT_MEASURING(void *parameter) {

  while (1) {
    analogReadResolution(10);
    float current_sensor_adc_value = analogRead(current_sensor);
    adc_voltage  = (current_sensor_adc_value * Reference_Voltage) / 1024.0;
    Voltage = adc_voltage / (R2 / (R1 + R2));
    Current = (Voltage / 1000) * 1000;
  }
}

void TaskDISPLAY(void *parameter) {

  while (1) {
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
}


void TaskADC(void *parameters) {

  while (1) {
    analogReadResolution(10);
    LDRValue1 = analogRead(LDRpin1);      // read the value from the LDR
    analogReadResolution(10);
    LDRValue2 = analogRead(LDRpin2);      // read the value from the LDR

  }
}


void TaskROTATE(void *parameters) {

  while (1) {

    if (LDRValue1 < LDRValue2) {  //rotate left
      vTaskDelay(pdMS_TO_TICKS(10));
      while (( ((LDRValue2 - 50) < LDRValue1) & (LDRValue1 < (LDRValue2 + 50)) ) == 0) {
        if (rotate_angle > 10) {
          rotate_angle = rotate_angle - 3;
          myservo.write(rotate_angle);
          real_angle = 45 - (rotate_angle / 2);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
      }
    }
    if (LDRValue1 > LDRValue2) {

      vTaskDelay(pdMS_TO_TICKS(10));
      while (( ((LDRValue1 - 50) < LDRValue2) & (LDRValue2 < (LDRValue1 + 50)) ) == 0) {
        if (rotate_angle < 180) {
          rotate_angle = rotate_angle + 3;
          myservo.write(rotate_angle);
          real_angle = 45 - (rotate_angle / 2);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
      }
    }
  }
}

void TaskConnectToNet(void *parameters) {

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());


  const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>

  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">

  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 3rem;
             vertical-align:middle;
    }
    .labels{
      font-size: 5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
  
</head>

<body>
  <h2>ESP32 Solar Tracker Server</h2>
  <p>
    <i class="fa-solid fa-angle" style="color:#059e8a;"></i> 
    <span class="labels">Rotation</span> 
    <span id="rotation">%ROTATION%</span>                     
    <sup class="units">&deg;</sup>
  </p>
  <p>
    <i class="fa-solid fa-meter" style="color:#00add6;"></i> 
    <span class="labels">Current</span>
    <span id="humidity">%CURRENT%</span>                  
    <sup class="units">A;</sup>
  </p>
</body>

<script>
setInterval(function ( ) {                /*runs every 10 sencods*/
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("rotation").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/rotaion", true);         //gets the latest ROTATION
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("current").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/current", true);
  xhttp.send();
}, 10000 ) ;
</script>

</html>)rawliteral";


  // Replaces placeholder with DHT values
  String processor(const String & var) {
    //Serial.println(var);
    if (var == "ROTATION") {
      return String(real_angle);
    }
    else if (var == "CURRENT") {
      return String(Current);
    }
    return String();
  }
}
