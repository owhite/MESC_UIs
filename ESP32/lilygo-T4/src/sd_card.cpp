#include "sd_card.h"
#include "global.h"

// Define the queue handle and semaphore
QueueHandle_t loggingQueue;
SemaphoreHandle_t sdLoggingStateMutex;

void initSDCard(HardwareSerial& compSerial, HardwareSerial& mescSerial) {
  g_compSerial = &compSerial;
  g_mescSerial = &mescSerial;

  if (!SD.begin()) {
    g_compSerial->println("Card Mount Failed");
    return;
  }

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    g_compSerial->println("No SD card attached");
    return;
  }

  g_compSerial->print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    g_compSerial->println("MMC");
  } else if (cardType == CARD_SD) {
    g_compSerial->println("SDSC");
  } else if (cardType == CARD_SDHC) {
    g_compSerial->println("SDHC");
  } else {
    g_compSerial->println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  g_compSerial->printf("SD Card Size: %lluMB\n", cardSize);

  // Create a queue for 10 requests
  loggingQueue = xQueueCreate(10, sizeof(LoggingRequest));  
  if (loggingQueue == NULL) {
    g_compSerial->println("Failed to create logging queue");
    return;
  }

  sdLoggingStateMutex = xSemaphoreCreateMutex();
  if (sdLoggingStateMutex == NULL) {
    g_compSerial->println("Failed to create mutex");
    return;
  }

  xTaskCreate(sdLoggingTask, "SD Logging Task", 4096, NULL, 1, NULL);
}

void sdLoggingTask(void* pvParameters) {
  while (true) {
    if (xQueueReceive(loggingQueue, &globalRequest, portMAX_DELAY) == pdPASS) {
      xSemaphoreTake(sdLoggingStateMutex, portMAX_DELAY);
 
      switch (globalRequest.commandType) {
      case LOG_START:
	if (sdLoggingState.fileName[0] == '\0') {
	  setLogFileName((char*) "log");
	}
	sdLoggingState.fileHandle = SD.open(sdLoggingState.fileName, FILE_WRITE);
	if (sdLoggingState.fileHandle) {
	  g_compSerial->printf("File opened for logging: %s\n", sdLoggingState.fileName);
	} else {
	  g_compSerial->println("Failed to open file for logging.");
	}
	sdLoggingState.isLogging = true;
	break;

      case LOG_STOP:
	if (sdLoggingState.fileHandle) {
	  sdLoggingState.fileHandle.close();
	  g_compSerial->println("Log file closed.");
	}
	sdLoggingState.isLogging = false;
	break;

      case LOG_ADD_LINE:
	if (sdLoggingState.isLogging && sdLoggingState.fileHandle) {
	  sdLoggingState.fileHandle.println(globalRequest.logLine);
	}
	break;
      }
      xSemaphoreGive(sdLoggingStateMutex);
    }
    vTaskDelay(10 / portTICK_PERIOD_MS); 
  }
}

const char* getLogFileName() {
  return sdLoggingState.fileName;
}

void setLoggingState(bool state) {
  sdLoggingState.isLogging = state;
}

void setLogFileName(char* name) {
  sdLoggingState.fileName[0] = '/';
  strncpy(sdLoggingState.fileName + 1, name, sizeof(sdLoggingState.fileName) - 1);
  strcat(sdLoggingState.fileName, ".txt");
}

void clrLogFileName() {
  sdLoggingState.fileName[0] = '\0';
}

// this doesnt work on this lilygo
bool isSDCardStillMounted() {
  // Attempt to open the root directory
  File testFile = SD.open("/");
  if (!testFile) {
    g_compSerial->println("SD card is no longer accessible.");
    return false;
  }

  testFile.close();
  return true;
}

