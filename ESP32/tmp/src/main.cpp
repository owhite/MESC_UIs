#include <Arduino.h>
#include <regex.h>
#include <cstring>

// Function to remove ANSI escape sequences
void remove_ansi_escape_sequences(char *data) {
    char buffer[strlen(data) + 1];  // Create a buffer to store the cleaned string
    int bufferIndex = 0;  // Index for the buffer
    int i = 0;  // Index for the input string

    while (data[i] != '\0') {
        // Check for escape character
        if (data[i] == '\x1B') {
            // Skip over the escape sequence
            i++;  // Skip the escape character
            if (data[i] == '[') {
                i++;  // Skip the '[' character

                // Skip the rest of the escape sequence
                while ((data[i] >= '0' && data[i] <= '?') || (data[i] >= ' ' && data[i] <= '/')) {
                    i++;
                }

                // Skip the final character in the escape sequence
                if (data[i] >= '@' && data[i] <= '~') {
                    i++;
                }
            }
        } else {
            // Copy regular characters to the buffer
            buffer[bufferIndex++] = data[i++];
        }
    }

    // Null-terminate the buffer
    buffer[bufferIndex] = '\0';

    // Copy the cleaned string back to the original data pointer
    strcpy(data, buffer);
}

void setup() {
    Serial.begin(115200);
    while (!Serial) {}

    // Example data with ANSI escape sequences


}

void loop() {
    char data[] = "\x1B[34muart\x1B[0m@MESC>get vbus\n\x1B[3`Parameter\x1B[15`| Value\x1B[35`| Min\x1B[46`| Max\x1B[57`| Description\n\x1B[3`\x1B[36mvbus\x1B[15`\x1B[37m| \x1B[32m54.455933\x1B[35`\x1B[37m| 0.00\x1B[46`\x1B[37m| inf\x1B[57`\x1B[37m| Read input voltage";

    // Your main code
    // Serial.println("Original data:");
    // Serial.println(data);

    // Remove ANSI escape sequences
    remove_ansi_escape_sequences(data);

    Serial.println("Processed data:");
    Serial.println(data);


}
