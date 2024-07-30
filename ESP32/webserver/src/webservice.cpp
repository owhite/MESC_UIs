#include "index_html.h"
#include "webservice.h"
#include "global.h"
#include "blink.h"

static AsyncWebSocket* g_webSocket;
static HardwareSerial* g_compSerial;
static HardwareSerial* g_mescSerial;

void initWebService(HardwareSerial& compSerial, HardwareSerial& mescSerial, AsyncWebServer& server, AsyncWebSocket& webSocket) {
  g_compSerial = &compSerial;
  g_mescSerial = &mescSerial;
  g_webSocket = &webSocket;

  g_compSerial->println("SPIFFS mounted successfully");

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
    data[len] = '\0'; // Null-terminate the string
    const char* message = (char*)data;

    g_compSerial->printf("WebSocket message: %s\n", message);
    g_mescSerial->write(message);
    g_mescSerial->write("\r\n");

    // not needed so far...
    if (strncmp(message, "COMMAND:", 8) == 0) {
        const char* command = message + 8; 
        processCommand(command);
    } else {
    }
}

void processCommand(const char* command) {
    g_compSerial->printf("Processing command: %s\n", command);

    // Process the command
    if (strcmp(command, "get") == 0) {
        g_mescSerial->write("get\r\n");
    } else {
        g_compSerial->printf("Unknown command: %s\n", command);
    }
}

void webServerTask(void *pvParameter) {
  while (1) {
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void handleRoot(AsyncWebServerRequest *request) {
  // Serve the HTML content from PROGMEM
  request->send_P(200, "text/html", index_html);
}

void handleButtonPress(AsyncWebServerRequest *request) {
  g_compSerial->println("Update requested");
  processButtonPress();
  request->send(200, "text/plain", "Update requested");
}

void processButtonPress() {
  g_compSerial->println("Running get.");
  g_mescSerial->write("get\r\n");

  jsonDoc.clear();
  commState = COMM_GET;
}
