#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "processData.h"
#include "blink.h"
#include "webserver.h"  // Include webserver.h

void handleRoot() {
    if (!LittleFS.exists("/index.html")) {
        server.send(404, "text/plain", "File not found");
        return;
    }
    
    File file = LittleFS.open("/index.html", "r");
    if (!file) {
        server.send(500, "text/plain", "Failed to open file");
        return;
    }

    String htmlTemplate = file.readString();
    file.close();

    // Replace %TABLE_ROWS% with actual data
    String tableRows = ""; // Replace with actual logic to fetch table rows
    htmlTemplate.replace("%TABLE_ROWS%", tableRows);

    server.send(200, "text/html", htmlTemplate);
}

void setup() {
    Serial.begin(115200);

    // Initialize LittleFS
    if (!LittleFS.begin()) {
        Serial.println("Error mounting LittleFS");
        while (1);
    }

    // Initialize WiFi
    WiFi.begin("YourWiFiSSID", "YourWiFiPassword");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Initialize tasks
    initProcessData(); // Initialize serial data processing task
    initWebserver();   // Initialize web server task
    initBlinkTask();   // Initialize any other tasks or peripherals as needed
}

void loop() {
    while (Serial.available()) {
        char incomingByte = Serial.read();
        
        if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
            if (bufferIndex < BUFFER_SIZE - 1) {
                serialBuffer[bufferIndex++] = incomingByte;
                serialBuffer[bufferIndex] = '\0'; 
                lastReceiveTime = millis(); 
            }
            xSemaphoreGive(mutex);
        }
    }
}
