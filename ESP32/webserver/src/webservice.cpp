#include "webservice.h"
#include "blink.h"

WebServer server(80);
String host;

static WebSocketsServer* g_webSocket;
static HardwareSerial* g_compSerial;
static HardwareSerial* g_mescSerial;

void initWebService(HardwareSerial& compSerial, HardwareSerial& mescSerial, WebSocketsServer& webSocket) {
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

  host = WiFi.localIP().toString();

  server.on("/", handleRoot);
  server.on("/button", handleButtonPress);
  server.begin();
  g_compSerial->println("HTTP server started");
  setBlinkSpeed(80);

  g_webSocket->begin();
  g_webSocket->onEvent(onWebSocketEvent);

  xTaskCreate(webServerTask, "Web Server Task", 8192, NULL, 1, NULL);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  data[len] = 0;  // Null-terminate the string
  g_compSerial->printf("WebSocket message: %s\n", (char *)data);
  // Handle received WebSocket message here
}

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
  case WStype_DISCONNECTED:
    g_compSerial->printf("[%u] Disconnected!\n", num);
    break;
  case WStype_CONNECTED: {
    IPAddress ip = g_webSocket->remoteIP(num);
    g_compSerial->printf("[%u] Connection from %s\n", num, ip.toString().c_str());
    break;
  }
  case WStype_TEXT:
    handleWebSocketMessage((void *)payload, payload, length);
    break;
  case WStype_BIN:
    g_compSerial->printf("[%u] Binary message received\n", num);
    break;
  default:
    break;
  }
}

void webServerTask(void *pvParameter) {
  while (1) {
    server.handleClient();
    g_webSocket->loop();
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
  g_compSerial->println("Button pressed");
  processButtonPress();
  server.send(200, "text/plain", "Button Pressed");
}

// custom logic independent of the server
void processButtonPress() {
  g_compSerial->println("Running get.");
  g_mescSerial->write("get\r\n");
}
