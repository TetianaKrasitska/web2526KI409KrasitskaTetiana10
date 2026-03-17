#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

const char* ssid = "IphoneT";
const char* password = "12345678";
const int ledPin = 2; // Вбудований LED на ESP32 Dev Module

WebServer server(80);

void setup() {
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);
    LittleFS.begin();

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) delay(500);
    Serial.println(WiFi.localIP());

    // Головна сторінка
    server.on("/", HTTP_GET, [](){
        File file = LittleFS.open("/index.html", "r");
        server.streamFile(file, "text/html");
        file.close();
    });

    // API Endpoint
    server.on("/api/led", HTTP_POST, [](){
        if (server.hasArg("plain")) {
            String body = server.arg("plain");
            JsonDocument doc;
            deserializeJson(doc, body);
            
            int status = doc["status"];
            digitalWrite(ledPin, status ? HIGH : LOW);
            
            server.send(200, "application/json", "{\"result\":\"ok\"}");
        }
    });

    server.begin();
}

void loop() {
    server.handleClient();
}