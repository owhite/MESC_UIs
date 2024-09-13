#include "sd_card.h"
#include "global.h"

// Define the queue handle and semaphore
QueueHandle_t loggingQueue;
SemaphoreHandle_t sdLoggingStateMutex;  // Rename to avoid confusion

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

  loggingQueue = xQueueCreate(10, sizeof(LoggingRequest));  // Create a queue for 10 requests
  if (loggingQueue == NULL) {
    g_compSerial->println("Failed to create logging queue");
    return;
  }

  sdLoggingStateMutex = xSemaphoreCreateMutex();  // Create a mutex for synchronizing access
  if (sdLoggingStateMutex == NULL) {
    g_compSerial->println("Failed to create mutex");
    return;
  }

  xTaskCreate(sdLoggingTask, "SD Logging Task", 4096, NULL, 1, NULL);  // Create SD logging task
}

bool isSDCardStillMounted() {
  // Attempt to open the root directory to verify the SD card is still accessible
  File testFile = SD.open("/");
  if (!testFile) {
    g_compSerial->println("SD card is no longer accessible.");
    return false;
  }

  testFile.close();
  return true;
}

void sdLoggingTask(void* pvParameters) {
  LoggingRequest request;
    
  while (true) {
    if (xQueueReceive(loggingQueue, &request, portMAX_DELAY) == pdPASS) {
      xSemaphoreTake(sdLoggingStateMutex, portMAX_DELAY);  // Take mutex before modifying state

      switch (request.commandType) {
      case LOG_START:
	setLogFileName("/log.txt");
	openLogFile();
	sdLoggingState.isLogging = true;
	break;

      case LOG_STOP:
	sdLoggingState.isLogging = false;
	closeLogFile();
	break;

      case LOG_ADD_LINE:
	if (sdLoggingState.isLogging && sdLoggingState.fileHandle) {
	  sdLoggingState.fileHandle.println(request.logLine);  // Write log line to file
	}
	break;
      }

      xSemaphoreGive(sdLoggingStateMutex);  // Release mutex after modifying state
    }
  }
}

void setLoggingState(bool state) {
  if (xSemaphoreTake(sdLoggingStateMutex, portMAX_DELAY)) {
    sdLoggingState.isLogging = state;
    xSemaphoreGive(sdLoggingStateMutex);
  }
}

void setLogFileName(const char* name) {
  if (xSemaphoreTake(sdLoggingStateMutex, portMAX_DELAY)) {
    sdLoggingState.fileName = name;
    if (sdLoggingState.fileHandle) {
      sdLoggingState.fileHandle.close();
    }
    xSemaphoreGive(sdLoggingStateMutex);
  }
}

void openLogFile() {
  if (xSemaphoreTake(sdLoggingStateMutex, portMAX_DELAY)) {
    if (!sdLoggingState.fileName.isEmpty()) {
      sdLoggingState.fileHandle = SD.open(sdLoggingState.fileName.c_str(), FILE_WRITE);
      if (sdLoggingState.fileHandle) {
	g_compSerial->println("File opened for logging: " + sdLoggingState.fileName);
      } else {
	g_compSerial->println("Failed to open file for logging.");
      }
    }
    xSemaphoreGive(sdLoggingStateMutex);
  }
}

void addLineToLogFile(const char* line) {
  if (xSemaphoreTake(sdLoggingStateMutex, portMAX_DELAY)) {
    if (sdLoggingState.isLogging && sdLoggingState.fileHandle) {
      sdLoggingState.fileHandle.println(line);
    }
    xSemaphoreGive(sdLoggingStateMutex);
  }
}

void closeLogFile() {
  if (xSemaphoreTake(sdLoggingStateMutex, portMAX_DELAY)) {
    if (sdLoggingState.fileHandle) {
      sdLoggingState.fileHandle.close();
      g_compSerial->println("Log file closed.");
    }
    xSemaphoreGive(sdLoggingStateMutex);
  }
}
