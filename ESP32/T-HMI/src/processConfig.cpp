#include "processConfig.h"
#include <LittleFS.h>
#include "global.h"
#include "WiFi.h"

Config config;
char *serialInput = nullptr; 

void initESP32Config() {
  serialInput = (char *)malloc(BUFFER_SIZE * sizeof(char));
  Serial.println("\nSwitching to ESP32 Configuration" );
  commState = COMM_ESP32;
}

void readConfig() {
  if (!LittleFS.begin()) {
    Serial.println("Failed to mount LittleFS");
    return;
  }

  if (!LittleFS.exists("/config.txt")) {
    Serial.println("Config file not found");
    LittleFS.end();
    return;
  }

  File file = LittleFS.open("/config.txt", "r");
  if (!file) {
    Serial.println("Failed to open config file");
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
      } else if (strcmp(key, "remote_IP") == 0) {
	strncpy(config.remote_IP, value, sizeof(config.remote_IP) - 1);
	parseIPAddress(config.remote_IP, config.remote_IP_array);
      } else if (strcmp(key, "local_IP") == 0) {
	strncpy(config.local_IP, value, sizeof(config.local_IP) - 1);
	parseIPAddress(config.local_IP, config.local_IP_array);
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

void parseIPAddress(const char* ipStr, uint8_t* ipArray) {
  char ipCopy[16];
  strncpy(ipCopy, ipStr, sizeof(ipCopy) - 1);
  ipCopy[sizeof(ipCopy) - 1] = '\0';

  // Tokenize and convert each part of the IP address
  char* token = strtok(ipCopy, ".");
  int i = 0;
  while (token != NULL && i < 4) {
    ipArray[i] = atoi(token);  // Convert string to integer
    token = strtok(NULL, ".");
    i++;
  }
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

void writeConfig() {
  if (!LittleFS.begin()) {
    Serial.println("Failed to mount LittleFS");
    return;
  }

  File file = LittleFS.open("/config.txt", "w");
  if (!file) {
    Serial.println("Failed to open config file for writing");
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
  Serial.println("Configuration saved");
}

void getConfig() {
  Serial.println("Current Configuration:");

  Serial.print("SSID: ");
  Serial.println(config.ssid);
  Serial.print("Password: ");
  Serial.println(config.password);
  Serial.print("Device Name: ");
  Serial.println(config.device_name);
  Serial.print("Sensor 1 Threshold: ");
  Serial.println(config.sensor1_threshold);
  Serial.print("Debug Mode: ");
  Serial.println(config.debug_mode ? "Enabled" : "Disabled");
  Serial.print("Log Interval (seconds): ");
  Serial.println(config.log_interval);
}

void *memcat(void *dest, size_t dest_len, const void *src, size_t src_len) {
  void *concat_position = (char *)dest + dest_len;
  memcpy(concat_position, src, src_len);
  return dest;
}

void handleESP32Config(char* localBuffer, int& localBufferIndex, char* serialInput, int& bufferIndex) {
  while (Serial.available()) {
    char ch = Serial.read();
    Serial.write(ch);

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
    Serial.print("too many variables");
  } else if (count == 0) {
    if (strncmp(line, "save", 4) == 0) {
      Serial.println("save entered");
    } else if (strncmp(line, "help", 4) == 0) {
      Serial.println("help entered");
    } else if (strncmp(line, "exit", 4) == 0) {
      Serial.println("exit entered");
      commState = COMM_IDLE;
    } else if (strncmp(line, "get", 3) == 0) {
      Serial.println("get entered");
      getConfig();
    } else if (strncmp(line, "IP", 2) == 0) {
      if (config.access_point) {
	Serial.println("Access point IP address: ");
	Serial.println(WiFi.softAPIP());
      }
      else {
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
      }
    } else {
      Serial.println("level 0 -- confused");
      Serial.println(line);
    }
  } else if (count == 1) {
    Serial.print("spaces: ");
    Serial.println(count);
    Serial.println(line);

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
      Serial.print("cmd: get ");
      Serial.print(value2);
      Serial.println(" Doesn't work :)");
    } else {
      Serial.print("command not recognized: ");
      Serial.println(value1);
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
      Serial.print("cmd: set ");
      Serial.println(value3);
	    
      if (strncmp(value2, "password", strlen("password")) == 0) {
	strcpy(config.password, value3);
      }
	    
    } else {
      Serial.print("command not recognized: ");
      Serial.println(value1);
    }
  } else {
    Serial.println("confusion about input");
    Serial.println(line);
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
