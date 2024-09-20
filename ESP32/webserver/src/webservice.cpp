#include <WiFi.h>
#include <esp_now.h>
#include "index_html.h"
#include "webservice.h"
#include "global.h"

int comm_counter = 0;
const uint16_t localPort = 1234; 
const uint16_t remote_Port = 1234;

IPAddress remote_IP;
WiFiUDP udpReceiver;
WiFiUDP udpSender;

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
    IPAddress local_IP(config.local_IP_array[0], config.local_IP_array[1], config.local_IP_array[2], config.local_IP_array[3]);
    IPAddress gateway(192,168,1,1);
    IPAddress subnet(255,255,255,0);

  remote_IP = IPAddress(config.remote_IP_array[0], config.remote_IP_array[1], config.remote_IP_array[2], config.remote_IP_array[3]);

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
  xTaskCreate(udpReceiveTask, "UDP Send Task", 4096, NULL, 1, NULL);
}

void udpReceiveTask(void *pvParameter) {
  udpReceiver.begin(localPort);
  Serial.printf("UDP Receiver listening on port %d\n", localPort);

  while (1) {
    int packetSize = udpReceiver.parsePacket();
    if (packetSize) {
      char incomingPacket[255];
      int len = udpReceiver.read(incomingPacket, 255);
      if (len > 0) {
        incomingPacket[len] = 0;  // Null-terminate packet
        Serial.printf("Received UDP packet: %s\n", incomingPacket);
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void udpSend(char *message) {
  // Serial.printf("Send UDP: %s to IP: %s, port: %d\n", message, remote_IP.toString().c_str(), remote_Port);

  udpSender.beginPacket(remote_IP, remote_Port);
  udpSender.printf("%s", message);
  udpSender.endPacket();
}

// WebSocket message handler
void handleWebSocketMessage(AsyncWebSocketClient* client, uint8_t *data, size_t len) {
  data[len] = '\0';  // Null-terminate the string
  char* message = (char*)data;

  // Variables to handle the log name extraction
  char my_str[100];  // Array to hold the extracted log name
  char* spacePtr = NULL;
  int spaceCount = 0;

  // Count spaces and find the first space
  for (char* p = (char*)message; *p != '\0'; p++) {
    if (*p == ' ') {
      spaceCount++;
      if (spaceCount == 1) {
        spacePtr = p;
      }
    }
  }

  if (strncmp(message, "MESC:", 5) == 0 && spacePtr) {
    strcpy(my_str, spacePtr + 1); 
    my_str[sizeof(my_str) - 1] = '\0'; 

    g_compSerial->printf("WebSocket to MESC: %s\n", my_str);
    g_mescSerial->write(my_str);
    g_mescSerial->write("\r\n");
  }
  else if (strncmp(message, "LOG_NAME:", 9) == 0 && spacePtr) {
    strcpy(my_str, spacePtr + 1); 
    my_str[sizeof(my_str) - 1] = '\0'; 
    g_compSerial->printf("Setting log name: %s\n", my_str);
    udpSend(message);
  }
  else if (strncmp(message, "LOG_START:", 10) == 0) {
    udpSend(message);
    vTaskDelay(300 / portTICK_PERIOD_MS); // no idea if this is realistic
    g_compSerial->println("Start logging...");
    logState = LOG_REQUEST_GET;
  }
  else if (strncmp(message, "LOG_STOP:", 9) == 0) {
    g_compSerial->println("...stop logging");
    logState = LOG_REQUEST_IDLE;
    udpSend(message);
  }
  else if (strncmp(message, "GRAPH_REQUEST:", 15) == 0) {
    g_compSerial->println("graphing");
    commState = COMM_GRAPH;
  }
  else {
    g_compSerial->println("strange websocket message");
  }
}

// Task to manage web server (runs in a FreeRTOS task)
void webServerTask(void *pvParameter) {
  while (1) {
    if (commState == COMM_GRAPH) {
      // Continuously update the graph -- should be optional.
      if (comm_counter > 5) { 
        g_mescSerial->write("log -fl\r\n");
        comm_counter = 0;
      }
      comm_counter++;
    }
    // this switch helps deal with multiple methods that can
    //   request logging. 
    switch(logState)
      {
      case LOG_REQUEST_GET:
	g_mescSerial->write("get\r\n");
	logState = LOG_GET;
	break;
      case LOG_REQUEST_JSON:
	g_mescSerial->write("status json\r\n");
	logState = LOG_JSON;
	break;
      case LOG_REQUEST_IDLE:
	g_mescSerial->write("status stop\r\n");
	logState = LOG_IDLE;
	break;
      default:
	break;
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

