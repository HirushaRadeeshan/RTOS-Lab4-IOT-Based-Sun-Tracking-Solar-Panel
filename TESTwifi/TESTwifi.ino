#include <WiFi.h>

//Network Credentials
const char* ssid = "EN20417284";
const char* password = "12345678";

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  xTaskCreatePinnedToCore(Conect,
                          "Task Rotate Core1",
                          3000,
                          NULL,
                          1,
                          NULL,
                          0);

}

void loop() {
  // put your main code here, to run repeatedly:

}

void Conect(void *parameters) {

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(WiFi.localIP());
  }

}
