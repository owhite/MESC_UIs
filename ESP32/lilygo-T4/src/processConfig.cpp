#include "processConfig.h"
#include <LittleFS.h>
#include "global.h"
#include "WiFi.h"

Config config;
char *serialInput = nullptr; 

void initESP32Config() {
  serialInput = (char *)malloc(BUFFER_SIZE * sizeof(char));
  g_compSerial->println("\nSwitching to ESP32 Configuration" );
  commState = COMM_ESP32;
}

void readConfig() {
  if (!LittleFS.begin()) {
    g_compSerial->println("Failed to mount LittleFS");
    return;
  }

  if (!LittleFS.exists("/config.txt")) {
    g_compSerial->println("Config file not found");
    LittleFS.end();
    return;
  }

  File file = LittleFS.open("/config.txt", "r");
  if (!file) {
    g_compSerial->println("Failed to open config file");
    LittleFS.end();
    return;
  }

  // Initialize buffers
  char line[128];
  while (file.available()) {
    size_t len = file.readBytesUntil('\n', line, sizeof(line) - 1);
    line[len] = '\0';  // Null-terminate the line

    // Skip empty lines and comments
    if (len == 0 || line[0] == '#' || line[0] == '\n') continue;

    // Split the line into key and value
    char* delimiter = strchr(line, '=');
    if (delimiter != nullptr) {
      *delimiter = '\0';  // Split key and value
      const char* key = line;
      const char* value = delimiter + 1;

      // Trim whitespace (optional)
      while (*key == ' ') key++;
      while (*value == ' ') value++;

      // Assign values to the corresponding fields
      if (strcmp(key, "ssid") == 0) {
	strncpy(config.ssid, value, sizeof(config.ssid) - 1);
      } else if (strcmp(key, "password") == 0) {
	strncpy(config.password, value, sizeof(config.password) - 1);
      } else if (strcmp(key, "MAC") == 0) {
	strncpy(config.MAC_str, value, sizeof(config.MAC_str) - 1);
	// Convert config.MAC to byte array and add peer
	parseMAC(config.MAC_str, config.MAC);
      } else if (strcmp(key, "device_name") == 0) {
	strncpy(config.device_name, value, sizeof(config.device_name) - 1);
      } else if (strcmp(key, "sensor1_threshold") == 0) {
	config.sensor1_threshold = atof(value);
      } else if (strcmp(key, "debug_mode") == 0) {
	config.debug_mode = atoi(value) != 0; // ??? 
      } else if (strcmp(key, "access_point") == 0) {
	if (strcmp(value, "1") == 0) {
	  config.access_point = true;
	} else {
	  config.access_point = false;
	}
      } else if (strcmp(key, "log_interval") == 0) {
	config.log_interval = atoi(value);
      }
    }
  }

  file.close();
  LittleFS.end();
}

int countCharOccurrences(const char* str, char ch) {
  int count = 0;
  while (*str) {
    if (*str == ch) {
      count++;
    }
    str++;
  }
  return count;
}

void parseMAC(const char* macStr, uint8_t* mac) {
  int values[6];
  if (sscanf(macStr, "%x:%x:%x:%x:%x:%x", &values[0], &values[1], &values[2], &values[3], &values[4], &values[5]) == 6) {
    for (int i = 0; i < 6; ++i) {
      mac[i] = (uint8_t)values[i];
    }
  }
}

void writeConfig() {
  if (!LittleFS.begin()) {
    g_compSerial->println("Failed to mount LittleFS");
    return;
  }

  File file = LittleFS.open("/config.txt", "w");
  if (!file) {
    g_compSerial->println("Failed to open config file for writing");
    LittleFS.end();
    return;
  }

  file.printf("ssid=%s\n", config.ssid);
  file.printf("password=%s\n", config.password);
  file.printf("device_name=%s\n", config.device_name);
  file.printf("sensor1_threshold=%.2f\n", config.sensor1_threshold);
  file.printf("debug_mode=%d\n", config.debug_mode ? 1 : 0);
  file.printf("log_interval=%d\n", config.log_interval);

  file.close();
  LittleFS.end();
  g_compSerial->println("Configuration saved");
}

void getConfig() {
  g_compSerial->println("Current Configuration:");

  g_compSerial->print("SSID: ");
  g_compSerial->println(config.ssid);
  g_compSerial->print("Password: ");
  g_compSerial->println(config.password);
  g_compSerial->print("Device Name: ");
  g_compSerial->println(config.device_name);
  g_compSerial->print("Sensor 1 Threshold: ");
  g_compSerial->println(config.sensor1_threshold);
  g_compSerial->print("Debug Mode: ");
  g_compSerial->println(config.debug_mode ? "Enabled" : "Disabled");
  g_compSerial->print("Log Interval (seconds): ");
  g_compSerial->println(config.log_interval);
}

void *memcat(void *dest, size_t dest_len, const void *src, size_t src_len) {
  void *concat_position = (char *)dest + dest_len;
  memcpy(concat_position, src, src_len);
  return dest;
}

void handleESP32Config(char* localBuffer, int& localBufferIndex, char* serialInput, int& bufferIndex) {
  while (g_compSerial->available()) {
    char ch = g_compSerial->read();
    g_compSerial->write(ch);

    if (bufferIndex < BUFFER_SIZE - 1) {
      serialInput[bufferIndex++] = ch;
      serialInput[bufferIndex] = '\0';
    }
  }

  if (bufferIndex > 0) {
    // really dont think this is necessary
    memcat(localBuffer, localBufferIndex, serialInput, bufferIndex);
    localBufferIndex += bufferIndex;
    bufferIndex = 0;
    serialInput[bufferIndex] = '\0';
  }

  for (int i = 0; i < localBufferIndex; i++) {
    if (localBuffer[i] == '\n') {
      localBuffer[i] = '\0';
      processConfig(localBuffer);
      int remainingLength = localBufferIndex - (i + 1);
      memmove(localBuffer, localBuffer + i + 1, remainingLength);
      localBufferIndex = remainingLength;
      i = -1;
    }
  }
}

void processConfig(char* line) {
  char value1[40];
  char value2[40];
  char value3[40];

  consolidateSpaces(line);
  int count = countCharOccurrences(line, ' ');

  if (count > 3) {
    g_compSerial->print("too many variables");
  } else if (count == 0) {
    if (strncmp(line, "save", 4) == 0) {
      g_compSerial->println("save entered");
    } else if (strncmp(line, "help", 4) == 0) {
      g_compSerial->println("help entered");
    } else if (strncmp(line, "exit", 4) == 0) {
      g_compSerial->println("exit entered");
      commState = COMM_IDLE;
    } else if (strncmp(line, "get", 3) == 0) {
      g_compSerial->println("get entered");
      getConfig();
    } else if (strncmp(line, "IP", 2) == 0) {
      if (config.access_point) {
	g_compSerial->println("Access point IP address: ");
	g_compSerial->println(WiFi.softAPIP());
      }
      else {
	g_compSerial->println("IP address: ");
	g_compSerial->println(WiFi.localIP());
      }
    } else {
      g_compSerial->println("level 0 -- confused");
      g_compSerial->println(line);
    }
  } else if (count == 1) {
    g_compSerial->print("spaces: ");
    g_compSerial->println(count);
    g_compSerial->println(line);

    char* token = strtok(line, " ");
    if (token != NULL) {
      strncpy(value1, token, sizeof(value1) - 1);
      value1[sizeof(value1) - 1] = '\0';

      token = strtok(NULL, " ");
      if (token != NULL) {
	strncpy(value2, token, sizeof(value2) - 1);
	value2[sizeof(value2) - 1] = '\0';
      }
    }
    if (strncmp(value1, "get", 3) == 0) {
      g_compSerial->print("cmd: get ");
      g_compSerial->print(value2);
      g_compSerial->println(" Doesn't work :)");
    } else {
      g_compSerial->print("command not recognized: ");
      g_compSerial->println(value1);
    }
  } else if (count == 2) {
    char* token = strtok(line, " ");
    if (token != NULL) {
      strncpy(value1, token, sizeof(value1) - 1);
      value1[sizeof(value1) - 1] = '\0';

      token = strtok(NULL, " ");
      if (token != NULL) {
	strncpy(value2, token, sizeof(value2) - 1);
	value2[sizeof(value2) - 1] = '\0';

	token = strtok(NULL, " ");
	if (token != NULL) {
	  strncpy(value3, token, sizeof(value3) - 1);
	  value3[sizeof(value3) - 1] = '\0';
	}
      }
    }
    if (strncmp(value1, "set", 3) == 0) {
      g_compSerial->print("cmd: set ");
      g_compSerial->println(value3);
	    
      if (strncmp(value2, "password", strlen("password")) == 0) {
	strcpy(config.password, value3);
      }
	    
    } else {
      g_compSerial->print("command not recognized: ");
      g_compSerial->println(value1);
    }
  } else {
    g_compSerial->println("confusion about input");
    g_compSerial->println(line);
  }
}

void consolidateSpaces(char* input) {
  char output[strlen(input) + 1];
  int j = 0;
  bool inSpace = false;

  for (int i = 0; input[i] != '\0'; ++i) {
    if (input[i] == '\n') {
      continue; 
    }
    if (input[i] == ' ') {
      if (!inSpace) {
	output[j++] = ' ';
	inSpace = true;
      }
    } else {
      output[j++] = input[i];
      inSpace = false;
    }
  }

  while (j > 0 && output[j - 1] == ' ') {
    j--;
  }

  output[j] = '\0';

  strcpy(input, output);
}
