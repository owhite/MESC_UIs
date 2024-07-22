#include "webservice.h"
#include "blink.h"
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <FS.h>
#include <LittleFS.h>

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);  // Define webSocket here
String host;

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    data[len] = 0;  // Null-terminate the string
    Serial.printf("WebSocket message: %s\n", (char *)data);
    // Handle received WebSocket message here
}

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[%u] Connection from %s\n", num, ip.toString().c_str());
            break;
        }
        case WStype_TEXT:
            handleWebSocketMessage((void *)payload, payload, length);
            break;
        case WStype_BIN:
            Serial.printf("[%u] Binary message received\n", num);
            break;
        default:
            break;
    }
}

void initWebService() {
    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }

    Serial.println("LittleFS mounted successfully");

    WiFi.begin("Love Factory", "ILoveLyra");
    // WiFi.begin("WhiteVan-551E", "thing123");

    Serial.print("Connecting to wifi");

    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    host = WiFi.localIP().toString();

    server.on("/", handleRoot);
    server.on("/button", handleButtonPress);  // Add button handler
    server.begin();
    Serial.println("HTTP server started");
    blinkSpeed = 80;

    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);

    xTaskCreate(webServerTask, "Web Server Task", 8192, NULL, 1, NULL);
}

void webServerTask(void *pvParameter) {
    while (1) {
        server.handleClient();
        webSocket.loop();
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void handleRoot() {
    if (!LittleFS.exists("/index.html")) {
        server.send(404, "text/plain", "File not found");
        return;
    }
    
    File file = LittleFS.open("/index.html", "r");
    server.streamFile(file, "text/html");
    file.close();
}

// talks to the server
void handleButtonPress() {
    Serial.println("Button pressed");
    processButtonPress();
    server.send(200, "text/plain", "Button Pressed");
}

// custom logic independent of the server
void processButtonPress() {
    Serial.println("Custom function logic executed.");
}
