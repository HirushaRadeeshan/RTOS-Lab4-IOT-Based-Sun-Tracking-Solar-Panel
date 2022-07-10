#include <Arduino.h>
#include <WiFi.h>
#include "ESPAsyncWebServer.h"


float data1 = -23.0;
float data2 = 2.32;
AsyncWebServer server(80);

//Network Credentials
#define WIFI_NETWORK "EN20417284"
#define WIFI_PASSWORD "12345678"
#define WIFI_TIMEOUT_MS 20000

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
    .units { font-size: 5rem;
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
    <sup class="units">mA</sup>
  </p>
</body>
<script>
setInterval(function ( ) {                /runs every 10 sencods/
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
String processor(const String& var) {
  //Serial.println(var);
  if (var == "ROTATION") {
    return String(data1);
  }
  else if (var == "CURRENT") {
    return String(data2);
  }
  return String();
}
//
void keepWiFiAlive(void *parameter) {
  for (;;) {
    if (WiFi.status() == WL_CONNECTED) {

      Serial.println("WIFI is Connected");
      vTaskDelay(10000 / portTICK_PERIOD_MS);
      continue;

    }

    Serial.println("WIFI Connecting");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

    unsigned long startAttemptTime = millis();

    // keep looping while we're not connetected
    while ((WiFi.status()) != (WL_CONNECTED && millis()) - startAttemptTime < WIFI_TIMEOUT_MS);


    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WIFI FAILED");
      vTaskDelay(20000 / portTICK_PERIOD_MS);
      continue;
    }

    Serial.println("WIFI connected" + WiFi.localIP());

  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  xTaskCreatePinnedToCore(keepWiFiAlive,
                          "WIFI",
                          5000,
                          NULL,
                          1,
                          NULL,
                          0);

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/rotation", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(data1).c_str());
  });
  server.on("/current", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(data2).c_str());
  });

  // Start server
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

}
