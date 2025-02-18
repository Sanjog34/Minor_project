#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include<Arduino.h>

const char* ssid = "ESP32_Hotspot";  
const char* password = "12345678";    

WebServer server(80);

void handleData() {
  if (server.hasArg("phone_no") && server.hasArg("message") && server.hasArg("delay")) {
      String phone_no = server.arg("phone_no");
      String message = server.arg("message");
      String delayTime = server.arg("delay");
      Serial.println("Received Phone no. :" + phone_no);
      Serial.println("Received Message: " + message);
      Serial.println("Received Delay: " + delayTime);
      
      int delayVal = delayTime.toInt();
      Serial.println("Converted Delay: " + String(delayVal) + " ms");
      
      // Simulate delay
      delay(delayVal);
      
      server.send(200, "text/plain", "Message: " + message + "\nDelay: " + String(delayVal) + " ms");
  } else {
      server.send(400, "text/plain", "Missing 'message' or 'delay' parameter");
  }
}

void handleRoot() {
    File file = SPIFFS.open("/index.html", "r");
    if (!file) {
        server.send(404, "text/plain", "File Not Found");
        return;
    }

    server.streamFile(file, "text/html");
    file.close();
}

void setup() {
    Serial.begin(115200);

    WiFi.softAP(ssid, password);
    Serial.println("ESP32 Access Point Started!");
    Serial.print("ESP32 AP IP Address: ");
    Serial.println(WiFi.softAPIP());

    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Initialization Failed!");
        return;
    }

    server.on("/", HTTP_GET, handleRoot);
    server.on("/send-data", HTTP_POST, handleData);

    server.begin();
}

void loop() {
    server.handleClient();
}
