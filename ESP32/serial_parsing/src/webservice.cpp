#include "webservice.h"
#include "blink.h"

WebServer server(80);
String host;

void initWebService() {
    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }

    Serial.println("LittleFS mounted successfully");

    // WiFi.begin("Love Factory", "ILoveLyra");
    WiFi.begin("WhiteVan-551E", "thing123");

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

    xTaskCreate(webServerTask, "Web Server Task", 8192, NULL, 1, NULL);
    xTaskCreatePinnedToCore(healthCheckTask, "Health Check Task", 4096, NULL, 1, NULL, 1);
}

void webServerTask(void *pvParameter) {
    while (1) {
        server.handleClient();
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

void healthCheckTask(void *pvParameter) {
    while (1) {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi connection lost. Attempting to reconnect...");
	    blinkSpeed = 1000;
            WiFi.begin("WhiteVan-551E", "thing123");
            while (WiFi.status() != WL_CONNECTED) {
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                Serial.print(".");
            }
	    blinkSpeed = 80;
            Serial.println("");
            Serial.println("WiFi reconnected");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());

            host = WiFi.localIP().toString().c_str();
        } else {
            WiFiClient client;
            if (!client.connect(host.c_str(), 80)) {
	      Serial.print("Connection to host failed: ");
	      Serial.print(host);
	      Serial.println(" Trying again...");
            } else {
		blinkSpeed = 80;
                client.stop();
            }
        }
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}
