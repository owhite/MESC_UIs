#include "webserver.h"

WebServer server(80);  // Initialize the WebServer instance

void initWebserver() {
    server.on("/", handleRoot);
    server.begin();
    Serial.println("HTTP server started");
}

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
    String tableRows = fetchTableRows();  // Fetch table rows dynamically
    htmlTemplate.replace("%TABLE_ROWS%", tableRows);

    server.send(200, "text/html", htmlTemplate);
}

String fetchTableRows() {
    // Implement your logic to fetch and return table rows here
    return "";  // Placeholder, replace with actual data
}
