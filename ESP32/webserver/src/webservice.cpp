#include <WiFi.h>
#include <esp_now.h>
#include "index_html.h"
#include "webservice.h"
#include "global.h"

int comm_counter = 0;

void initWebService(HardwareSerial& compSerial, HardwareSerial& mescSerial, AsyncWebServer& server, AsyncWebSocket& webSocket) {

  g_compSerial = &compSerial;
  g_mescSerial = &mescSerial;
  g_webSocket = &webSocket;

  // Set up Wi-Fi
  if (strlen(config.password) == 0) {
    g_compSerial->println("WiFi password not set!");
    return;
  }

  if (config.access_point) {
    // Configure the ESP32 as an Access Point
    IPAddress local_IP(192,168,4,1);
    IPAddress gateway(192,168,4,1);
    IPAddress subnet(255,255,255,0);

    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP((const char*)config.ssid, (const char*)config.password, 6);//

    g_compSerial->println("Access Point started");
    g_compSerial->print("AP IP address: ");
    g_compSerial->println(WiFi.softAPIP());
  }
  else {
    // Connect to an existing WiFi network
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
  }

  // Web server setup
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    handleRoot(request);
  });

  server.on("/button", HTTP_GET, [](AsyncWebServerRequest *request) {
    handleButtonPress(request);
  });

  server.addHandler(&webSocket);  // Attach WebSocket to server
  server.begin();
  g_compSerial->println("HTTP server started");

  // WebSocket Event handling
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

  // Start a FreeRTOS task to manage the web server
  xTaskCreate(webServerTask, "Web Server Task", 8192, NULL, 1, NULL);

}

// WebSocket message handler
void handleWebSocketMessage(AsyncWebSocketClient* client, uint8_t *data, size_t len) {
    data[len] = '\0';  // Null-terminate the string
    const char* message = (char*)data;

    if (strcmp(message, "IP") == 0) {
        // Handle IP request...
    }
    else if (strcmp(message, "graph_request") == 0) {
        // Handle graph request...
    }
    else if (strcmp(message, "log_start") == 0) {
        g_compSerial->println("Starting logging...");
    }
    else if (strcmp(message, "log_stop") == 0) {
        g_compSerial->println("Stopping logging...");
    }
    else if (strcmp(message, "log_add_line") == 0) {
        g_compSerial->println("Adding line to log...");
    }
    else {
        g_compSerial->printf("WebSocket message: %s\n", message);
        g_mescSerial->write(message);
        g_mescSerial->write("\r\n");
    }
}

// Task to manage web server (runs in a FreeRTOS task)
void webServerTask(void *pvParameter) {
  while (1) {
    if (commState == COMM_LOG) {
      // Continuously update the graph -- should be optional.
      if (comm_counter > 5) { 
        g_mescSerial->write("log -fl\r\n");
        comm_counter = 0;
      }
      comm_counter++;
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// Handle root request and serve HTML content
void handleRoot(AsyncWebServerRequest *request) {
  request->send_P(200, "text/html", index_html);
}

// Handle button press
void handleButtonPress(AsyncWebServerRequest *request) {
  g_compSerial->println("Update requested");
  processButtonPress();
  request->send(200, "text/plain", "Update requested");
}

// Handle the actual processing of the button press
void processButtonPress() {
  g_compSerial->println("Running get.");
  g_mescSerial->write("get\r\n");

  commState = COMM_GET;
}

