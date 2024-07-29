#include "webservice.h"
#include "blink.h"

static AsyncWebSocket* g_webSocket;
static HardwareSerial* g_compSerial;
static HardwareSerial* g_mescSerial;

void initWebService(HardwareSerial& compSerial, HardwareSerial& mescSerial, AsyncWebServer& server, AsyncWebSocket& webSocket) {
    g_compSerial = &compSerial;
    g_mescSerial = &mescSerial;
    g_webSocket = &webSocket;

    if (!LittleFS.begin()) {
        g_compSerial->println("An Error has occurred while mounting LittleFS");
        return;
    }

    g_compSerial->println("LittleFS mounted successfully");

    WiFi.begin("HouseNextDoor", "ILoveLyra");

    g_compSerial->print("Connecting to wifi");

    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        g_compSerial->print(".");
    }

    g_compSerial->println("");
    g_compSerial->println("WiFi connected");
    g_compSerial->print("IP address: ");
    g_compSerial->println(WiFi.localIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        handleRoot(request);
    });

    server.on("/button", HTTP_GET, [](AsyncWebServerRequest *request) {
        handleButtonPress(request);
    });

    server.addHandler(&webSocket);  // Attach WebSocket to server
    server.begin();
    g_compSerial->println("HTTP server started");
    setBlinkSpeed(80);

webSocket.onEvent([](AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
    switch (type) {
        case WS_EVT_DISCONNECT:
            g_compSerial->printf("[%u] Disconnected!\n", client->id());
            break;
        case WS_EVT_CONNECT:
            g_compSerial->printf("[%u] Connected!\n", client->id());
            break;
        case WS_EVT_DATA:
            handleWebSocketMessage(client, data, len);
            break;
        case WS_EVT_PONG:
            g_compSerial->printf("[%u] Pong received\n", client->id());
            break;
        case WS_EVT_ERROR:
            g_compSerial->printf("[%u] Error\n", client->id());
            break;
        default:
            break;
    }
});

    xTaskCreate(webServerTask, "Web Server Task", 8192, NULL, 1, NULL);
}

void handleWebSocketMessage(AsyncWebSocketClient* client, uint8_t *data, size_t len) {
    data[len] = 0;  // Null-terminate the string
    g_compSerial->printf("WebSocket message: %s\n", (char *)data);
    // Handle received WebSocket message here
}

void webServerTask(void *pvParameter) {
    while (1) {
        // No need to call loop() for AsyncWebSocket
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void handleRoot(AsyncWebServerRequest *request) {
    File file = LittleFS.open("/index.html", "r");
    if (!file) {
        Serial.println("Failed to open file: /index.html");
        request->send(404, "text/plain", "File Not Found");
        return;
    }

    // Send the file
    request->send(file, "/index.html", "text/html");
    file.close();

    Serial.println("File served: /index.html");
}

void handleButtonPress(AsyncWebServerRequest *request) {
    g_compSerial->println("Button pressed");
    processButtonPress();
    request->send(200, "text/plain", "Button Pressed");
}

void processButtonPress() {
    g_compSerial->println("Running get.");
    g_mescSerial->write("get\r\n");
}
