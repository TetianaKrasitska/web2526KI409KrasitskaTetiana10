#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <Preferences.h>

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
WebServer server(80);
Preferences preferences;

void setup() {
    Serial.begin(115200);
    LittleFS.begin();

    // Створюємо точку доступу
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP("ESP32_Setup_Krasitska");

    // DNS сервер перенаправляє всі запити на наш IP
    dnsServer.start(DNS_PORT, "*", apIP);

    server.on("/", HTTP_GET, []() {
        File file = LittleFS.open("/index.html", "r");
        server.streamFile(file, "text/html");
        file.close();
    });

    // Обробка форми
    server.on("/save", HTTP_POST, []() {
        String ssid = server.arg("ssid");
        String pass = server.arg("pass");
        
        preferences.begin("wifi", false);
        preferences.putString("ssid", ssid);
        preferences.putString("pass", pass);
        preferences.end();

        server.send(200, "text/html", "<h1>Data saved! ESP32 is rebooting...</h1>");
        delay(2000);
        ESP.restart();
    });

    // Captive Portal для Android/iOS
    server.onNotFound([]() {
        server.sendHeader("Location", String("http://") + apIP.toString(), true);
        server.send(302, "text/plain", "");
    });

    server.begin();
    Serial.println("Captive Portal запущено");
}

void loop() {
    dnsServer.processNextRequest();
    server.handleClient();
}