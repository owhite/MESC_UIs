#include "webservice.h"
#include "processData.h"
#include "blink.h"

WebServer server(80);
String host;

void initWebService() {
    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }

    Serial.println("LittleFS mounted successfully");

    WiFi.begin("Love Factory", "ILoveLyra");

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
    server.on("/update", handleUpdate);
    server.begin();
    Serial.println("HTTP server started");
    blinkSpeed = 80;

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    xTaskCreate(webServerTask, "Web Server Task", 8192, NULL, 1, NULL);
}

void webServerTask(void *pvParameter) {
    while (1) {
        server.handleClient();
        webSocket.loop();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void handleUpdate() {
  // Handle POST request to update data
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

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    if (type == WStype_TEXT) {
        Serial.printf("[%u] get Text: %s\n", num, payload);
        // Process incoming WebSocket data here
    } else if (type == WStype_CONNECTED) {
        Serial.printf("[%u] Connected!\n", num);
    } else if (type == WStype_DISCONNECTED) {
        Serial.printf("[%u] Disconnected!\n", num);
    }
}
