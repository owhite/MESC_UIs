#include <WiFi.h>
#include "index_html.h"
#include "webservice.h"
#include "sd_card.h"
#include "global.h"
#include "blink.h"

int comm_counter = 0;

void initWebService(HardwareSerial& compSerial, HardwareSerial& mescSerial, AsyncWebServer& server, AsyncWebSocket& webSocket) {

  g_compSerial = &compSerial;
  g_mescSerial = &mescSerial;
  g_webSocket = &webSocket;

  if (strlen(config.password) == 0) {
    g_compSerial->println("WiFi password not set!");
    return;
  }

  WiFi.begin((const char*)config.ssid, (const char*)config.password);

  g_compSerial->print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    g_compSerial->print(".");
  }

  g_compSerial->println("");
  g_compSerial->println("WiFi connected");
  g_compSerial->print("IP address: ");
  g_compSerial->println(WiFi.localIP());

  // Web server setup remains unchanged
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
    data[len] = '\0';  // Null-terminate the string
    const char* message = (char*)data;

    // LoggingRequest request;

    if (strcmp(message, "IP") == 0) {
        // Handle IP request...
    }
    else if (strcmp(message, "graph_request") == 0) {
        // Handle graph request...
    }
    else if (strcmp(message, "log_start") == 0) {
        g_compSerial->println("Starting logging...");
        // request.commandType = LOG_START;
        // xQueueSend(loggingQueue, &request, portMAX_DELAY);
    }
    else if (strcmp(message, "log_stop") == 0) {
        g_compSerial->println("Stopping logging...");
        // request.commandType = LOG_STOP;
        // xQueueSend(loggingQueue, &request, portMAX_DELAY);
    }
    else if (strcmp(message, "log_add_line") == 0) {
        g_compSerial->println("Adding line to log...");
        // request.commandType = LOG_ADD_LINE;
        // request.logLine = "Sample log line\n";  // Replace with dynamic content if needed
        // xQueueSend(loggingQueue, &request, portMAX_DELAY);
    }
    else {
        g_compSerial->printf("WebSocket message: %s\n", message);
        g_mescSerial->write(message);
        g_mescSerial->write("\r\n");
    }
}

void webServerTask(void *pvParameter) {
  while (1) {
    if (commState == COMM_LOG) {
      // continuously update the graph -- should be optional. 
      if (comm_counter > 5) { 
	g_mescSerial->write("log -fl\r\n");
	comm_counter = 0;
      }
      comm_counter++;
    }
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

  commState = COMM_GET;
}