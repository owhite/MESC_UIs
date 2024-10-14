#include "pins.h"
#include "sd_card.h"
#include "global.h"

SemaphoreHandle_t sdLoggingStateMutex;

void initSDCard() {
  Serial.println("Card mount attempt");

  SD_MMC.setPins(SD_SCLK_PIN, SD_MOSI_PIN, SD_MISO_PIN);

  bool rlst = false;
  while (1) {
    delay(500);

    rlst = SD_MMC.begin("/sdcard", true);
    if (!rlst) {
      tft.print("* ");
      Serial.println("SD init failed");
      Serial.println("➸ No detected SdCard");
    } else {
      Serial.println("SD init success");
      Serial.printf("➸ Detected SdCard insert: %.2f GB\r\n", SD_MMC.cardSize() / 1024.0 / 1024.0 / 1024.0);
      break;
    }
  }

  uint8_t cardType = SD_MMC.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD_MMC card attached");
    return;
  }

  Serial.print("SD_MMC Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);

  // Create a queue for 10 requests
  loggingQueue = xQueueCreate(10, sizeof(LoggingRequest));  
  if (loggingQueue == NULL) {
    Serial.println("Failed to create logging queue");
    return;
  }

  sdLoggingStateMutex = xSemaphoreCreateMutex();
  if (sdLoggingStateMutex == NULL) {
    Serial.println("Failed to create mutex");
    return;
  }

  xTaskCreate(sdLoggingTask, "SD Logging Task", 4096, NULL, 1, NULL);
}

void sdLoggingTask(void* pvParameters) {
  while (true) {
    if (xQueueReceive(loggingQueue, &logRequest, portMAX_DELAY) == pdPASS) {
      xSemaphoreTake(sdLoggingStateMutex, portMAX_DELAY);
 
      switch (logRequest.commandType) {
      case LOG_START:
	if (sdLoggingState.fileName[0] == '\0') {
	  setLogFileName((char*) "log");
	}
	sdLoggingState.fileHandle = SD.open(sdLoggingState.fileName, FILE_WRITE);
	if (sdLoggingState.fileHandle) {
	  Serial.printf("File opened for logging: %s\n", sdLoggingState.fileName);
	} else {
	  Serial.println("Failed to open file for logging.");
	}
	sdLoggingState.isLogging = true;
	break;

      case LOG_STOP:
	if (sdLoggingState.fileHandle) {
	  sdLoggingState.fileHandle.close();
	  Serial.println("Log file closed.");
	}
	sdLoggingState.isLogging = false;
	break;

      case LOG_ADD_LINE:
	if (sdLoggingState.isLogging && sdLoggingState.fileHandle) {
	  sdLoggingState.fileHandle.println(logRequest.logLine);
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

  return true;

  // Attempt to open the root directory
  File testFile = SD_MMC.open("/");
  if (!testFile) {
    Serial.println("SD card is no longer accessible.");
    return false;
  }

  testFile.close();
  return true;
}

