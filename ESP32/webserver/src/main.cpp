#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include "blink.h"  // Include your custom header file

WebServer server(80);

// Function to handle the root path
void handleRoot() {
    if (!LittleFS.exists("/index.html")) {
        server.send(404, "text/plain", "File not found");
        return;
    }
    
    File file = LittleFS.open("/index.html", "r");
    server.streamFile(file, "text/html");
    file.close();
}

// Task to run the web server
void webServerTask(void *pvParameter) {
    WiFi.begin("Love Factory", "ILoveLyra");

    Serial.print("Connecting to Love Factory");

    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/", handleRoot);
    server.begin();
    Serial.println("HTTP server started");
    blinkSpeed = 200; 

    while (1) {
        server.handleClient();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(1000);
    Serial.println("Starting setup...");
    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }

    Serial.println("LittleFS mounted successfully");

    // Initialize your tasks
    initBlinkTask();  // Call initBlinkTask() from blink_task.c
    xTaskCreate(webServerTask, "Web Server Task", 8192, NULL, 1, NULL);
}

void loop() {
    // The loop function is empty because the tasks are running in parallel
}
