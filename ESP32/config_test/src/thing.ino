#include <Arduino.h>
#include <LittleFS.h>

struct Config {
    char ssid[32];
    char password[32];
    int someSetting;
};

Config config;

void readConfig() {
    if (LittleFS.begin()) {
        if (LittleFS.exists("/config.txt")) {
            File file = LittleFS.open("/config.txt", "r");
            if (file) {
                file.readBytes(config.ssid, sizeof(config.ssid) - 1);
                file.readBytes(config.password, sizeof(config.password) - 1);
                file.readBytes((char*)&config.someSetting, sizeof(config.someSetting));
                file.close();
            }
        }
        LittleFS.end();
    }
}

void writeConfig() {
    if (LittleFS.begin()) {
        File file = LittleFS.open("/config.txt", "w");
        if (file) {
            file.write((const uint8_t*)config.ssid, strlen(config.ssid) + 1);
            file.write((const uint8_t*)config.password, strlen(config.password) + 1);
            file.write((const uint8_t*)&config.someSetting, sizeof(config.someSetting));
            file.close();
        }
        LittleFS.end();
    }
}

void getConfig() {
    Serial.print("SSID: ");
    Serial.println(config.ssid);
    Serial.print("Password: ");
    Serial.println(config.password);
    Serial.print("Some Setting: ");
    Serial.println(config.someSetting);
}

void setup() {
    Serial.begin(115200);
    readConfig();
    getConfig();
    Serial.println("Enter commands:");
}

void loop() {
    static char inputBuffer[128];
    static int inputIndex = 0;

    if (Serial.available() > 0) {
        char c = Serial.read();
        Serial.write(c);  // Echo back the character

        if (c == '\n' || c == '\r') {
            inputBuffer[inputIndex] = '\0';  // Null-terminate the string

            // Process command
            if (strncmp(inputBuffer, "set ssid ", 9) == 0) {
                strncpy(config.ssid, inputBuffer + 9, sizeof(config.ssid) - 1);
                config.ssid[sizeof(config.ssid) - 1] = '\0';
                writeConfig();
                Serial.println("SSID updated");
            } else if (strncmp(inputBuffer, "set password ", 13) == 0) {
                strncpy(config.password, inputBuffer + 13, sizeof(config.password) - 1);
                config.password[sizeof(config.password) - 1] = '\0';
                writeConfig();
                Serial.println("Password updated");
            } else if (strncmp(inputBuffer, "set someSetting ", 16) == 0) {
                config.someSetting = atoi(inputBuffer + 16);
                writeConfig();
                Serial.println("Some Setting updated");
            } else if (strncmp(inputBuffer, "get", 3) == 0) {
                getConfig();
            } else {
                Serial.println("Unknown command");
            }

            inputIndex = 0;  // Reset the buffer index
        } else {
            if (inputIndex < sizeof(inputBuffer) - 1) {
                inputBuffer[inputIndex++] = c;
            } else {
                Serial.println("Input buffer overflow");
                inputIndex = 0;  // Reset the buffer index
            }
        }
    }
}
