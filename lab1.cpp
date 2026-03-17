#include <Arduino.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

const char* ssid = "IphoneT";
const char* password = "12345678";

WebServer server(80);

void setup() {
    Serial.begin(115200);

    // Ініціалізація LittleFS
    if(!LittleFS.begin(true)){
        Serial.println("Помилка LittleFS!");
        return;
    }

    // Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nIP: " + WiFi.localIP().toString());

    server.on("/", HTTP_GET, [](){
        File file = LittleFS.open("/index.html", "r");
        server.streamFile(file, "text/html");
        file.close();
    });

    server.begin();
}

void loop() {
    server.handleClient();
}