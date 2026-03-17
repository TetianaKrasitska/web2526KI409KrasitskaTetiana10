#include <Arduino.h>
#include <WiFi.h>
#include <HTTPSServer.hpp>
#include <SSLCert.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>

using namespace httpsserver;

const char* ssid = "IphoneT";
const char* password = "12345678";

SSLCert cert = SSLCert(); 
HTTPSServer secureServer = HTTPSServer(&cert);

void handleRoot(HTTPRequest * req, HTTPResponse * res) {
    // Basic Authentication: admin:password -> YWRtaW46cGFzc3dvcmQ=
    if (req->getHeader("Authorization") != "Basic YWRtaW46cGFzc3dvcmQ=") {
        res->setStatusCode(401);
        res->setHeader("WWW-Authenticate", "Basic realm=\"Secure ESP32\"");
        res->print("401 Unauthorized - Access Denied");
        return;
    }

    res->setHeader("Content-Type", "text/html; charset=utf-8");
    res->print("<!DOCTYPE html><html><body><h1>Secure Session Active</h1><p>Student: Tetiana Krasitska</p></body></html>");
}

void setup() {
    Serial.begin(115200);
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) delay(500);
    Serial.println("\nConnected! IP address: " + WiFi.localIP().toString());

    // Генерація самопідписаного сертифіката
    int res = createSelfSignedCert(cert, KEYSIZE_2048, "CN=esp32.local,O=KrasitskaLabs,C=UA");
    if (res != 0) {
        Serial.println("Certificate generation failed!");
        return;
    }

    ResourceNode * nodeRoot = new ResourceNode("/", "GET", &handleRoot);
    secureServer.registerNode(nodeRoot);

    secureServer.start();
    if (secureServer.isRunning()) {
        Serial.println("HTTPS Server started on port 443");
    }
}

void loop() {
    secureServer.loop();
}