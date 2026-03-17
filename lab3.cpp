#include <Arduino.h>
#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

const char* ssid = "IphoneT";
const char* password = "12345678";
const int potPin = 34;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void setup() {
    Serial.begin(115200);
    pinMode(potPin, INPUT);
    LittleFS.begin();

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) delay(500);

    ws.onEvent([](AsyncWebSocket *s, AsyncWebSocketClient *c, AwsEventType t, void *arg, uint8_t *d, size_t l){});
    server.addHandler(&ws);
    
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *r){
        r->send(LittleFS, "/index.html", "text/html");
    });

    server.begin();
}

void loop() {
    ws.cleanupClients();
    static unsigned long lastTime = 0;
    if (millis() - lastTime > 200) { // Оновлення 5 разів на секунду
        lastTime = millis();
        int val = analogRead(potPin);
        
        JsonDocument doc;
        doc["value"] = val;
        String json;
        serializeJson(doc, json);
        ws.textAll(json);
    }
}