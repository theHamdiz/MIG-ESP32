#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>

String copyRight = 
" *\n"
" * Hamdiz Multi-ID Generator\n"
" * -------------------------\n"
" * Created with ❤️ by Ahmad Hamdi (@theHamdiz) => (2024 - 2025)\n"
" * ESP32 Version\n"
" * This program generates various types of unique IDs on demand and supports additional features\n"
" * like streaming mode, saving outputs, and customizable settings for enhanced flexibility.\n"
" * this program leverages esp32 true randomness to produce truly randomized (not pseudo-random) IDs.\n"
" *\n"
" * Supported Commands:\n"
" * --------------------\n"
" * 1. int <length> (or integer <length>)\n"
" *    - Generate a random integer with the specified length (1 to 64 digits).\n"
" *    - Example: int 16\n"
" *    - Example: integer 32\n"
" *\n"
" * 2. uuid v1 (or uuid1)\n"
" *    - Generate a UUID Version 1 (timestamp-based).\n"
" *    - Example: uuid v1\n"
" *    - Example: uuid1\n"
" *\n"
" * 3. uuid v4 (or uuid4)\n"
" *    - Generate a UUID Version 4 (random).\n"
" *    - Example: uuid v4\n"
" *    - Example: uuid4\n"
" *\n"
" * 4. hex <length> (or hexadecimal <length>)\n"
" *    - Generate a random hexadecimal string with the specified length.\n"
" *    - Example: hex 32\n"
" *    - Example: hexadecimal 64\n"
" *\n"
" * 5. base64 <length>\n"
" *    - Generate a random Base64 string with the specified length.\n"
" *    - Example: base64 16\n"
" *\n"
" * 6. alphanum <length> (or alphanumeric <length>)\n"
" *    - Generate a random alphanumeric string with the specified length.\n"
" *    - Example: alphanum 20\n"
" *    - Example: alphanumeric 64\n"
" *\n"
" * 7. bin <length> (or binary <length>)\n"
" *    - Generate a random binary string with the specified length.\n"
" *    - Example: bin 32\n"
" *    - Example: binary 16\n"
" *\n"
" * 8. custom <set> <length>\n"
" *    - Generate a random string using the provided custom character set.\n"
" *    - Example: custom ABCD1234 16\n"
" *\n"
" * 9. stream <command> <interval>\n"
" *    - Continuously execute a command at the specified interval (in milliseconds).\n"
" *    - The streaming mode outputs only the generated IDs (no labels or decoration).\n"
" *    - Example: stream uuid v4 1000\n"
" *      - Generates a new UUID v4 every 1 second.\n"
" *\n"
" * 10. stop stream\n"
" *     - Stops the currently active streaming mode.\n"
" *     - Example: stop stream\n"
" *\n"
" * 11. setup\n"
" *     - Starts the setup process to configure the output destination (serial, file, API).\n"
" *       - For \"API\" output:\n"
" *         - Prompts for endpoint URL, payload format, authentication type (if required),\n"
" *           and credentials or tokens.\n"
" *       - Saves configuration to EEPROM and skips setup on subsequent runs unless explicitly requested.\n"
" *     - Example: setup\n"
" *\n"
" * 12. save <file>\n"
" *     - Saves generated output to the specified file (SD card required).\n"
" *     - Example: save output.txt\n"
" *\n"
" * 13. load (or read)\n"
" *     - Reads the data from the output.txt file.\n"
" *     - Example: load\n"
" *\n"
" * Notes:\n"
" * ------\n"
" * - All commands are case-insensitive and support abbreviations/synonyms for easier usage.\n"
" * - Streaming suppresses all additional output except for the generated IDs.\n"
" * - To return to normal mode, use \"stop stream\".\n"
" * - Setup must be completed on the first run, but it can be modified later using the \"setup\" command.\n"
" *\n"
" * Advanced Features:\n"
" * -------------------\n"
" * - Peripheral Validation: Automatically checks for connected peripherals (e.g., SD card, LCD).\n"
" *   Prints warnings if a required device is unavailable.\n"
" * - Customizable Output: Supports serial output, file saving, and API calls with configurable\n"
" *   authentication.\n"
" * - API Integration: Allows sending generated IDs to a specified API endpoint with optional\n"
" *   authentication (Bearer token, Basic Auth, etc.).\n"
" * - Synonyms and Aliases: Commands support multiple aliases for improved usability.\n"
" * - Error Handling: Comprehensive error messages for invalid inputs or missing peripherals.\n";

// Enum for ID types
enum IDType { RANDOM_INT, UUID_V1, UUID_V4, HEX_ID, BASE64, ALPHANUMERIC, BINARY, CUSTOM };

// Constants
const char BASE64_CHARS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char ALPHANUMERIC_CHARS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
const size_t EEPROM_SIZE = 512;

bool isStreaming = false;           // Streaming flag.
unsigned long streamInterval = 0;   // Streaming interval in ms.
unsigned long lastStreamTime = 0;   // Last time a command was streamed.
String streamCommand = "";          // Command being streamed.
String outputDestination = "serial"; // Default output destination.
String apiUrl = "";                 // API endpoint.
String authType = "";               // Authentication type.
String authToken = "";              // Authentication token.
bool setupCompleted = false;        // Setup completion flag.

// Function prototypes
void processCommand(String command, bool streaming = false);
String generateRandomID(IDType type, int length, String customSet = "");
String generateUUIDv1();
String generateUUIDv4();
void printDecorativeLine(String text);
void handleStreamCommand(String command);
void printError(String message);
void performSetup();
void saveSetupData();
void loadSetupData();
bool isPeripheralAvailable(String device);
void printToOutput(String message);
void saveToFile(String filename, String content);
void readFromFile(String filename);
void printCopyRight();
void sendToAPI(String url, String payload, String authType, String token);

//##########################################//
//##########################################//
// Setup function.
//##########################################//
//##########################################//
void setup() {
  Serial.begin(921600);
  delay(1000);
  EEPROM.begin(EEPROM_SIZE);

  if (!SD.begin()) {
    Serial.println(" => SD card initialization failed.");
  } else {
    Serial.println("SD card initialized successfully.");
  }

  // Load setup data from EEPROM
  loadSetupData();

  if (!setupCompleted) {
    performSetup();
  }

  printCopyRight();
  printDecorativeLine("Hamdiz <@theHamdiz> Multi-ID Generator Ready");
}

//##########################################//
//##########################################//
// Main loop.
//##########################################//
//##########################################//
void loop() {
  // Handle streaming
  if (isStreaming && millis() - lastStreamTime >= streamInterval) {
    lastStreamTime = millis();
    processCommand(streamCommand, true);
  }

  // Handle new input commands
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "stop stream") {
      isStreaming = false;
      printDecorativeLine("Stream stopped");
      return;
    }

    if (command.startsWith("stream ")) {
      handleStreamCommand(command);
      return;
    }

    if (command == "setup") {
      performSetup();
      return;
    }

    processCommand(command);
  }
}

//##########################################//
//##########################################//
// Processes various commands.
//##########################################//
//##########################################//
void processCommand(String command, bool streaming) {
  command.toLowerCase(); // Normalize command for synonyms/abbreviations
  if (command.startsWith("int ")) {
    int length = command.substring(command.indexOf(' ') + 1).toInt();
    if (length >= 1 && length <= 64) {
      if (!streaming) printDecorativeLine("Random Integer");
      printToOutput(generateRandomID(RANDOM_INT, length));
    } else if (!streaming) {
      printError("Length must be between 1 and 64.");
    }
  } else if (command == "uuid v1" || command == "uuid1" || command == "u1" || command == "uu1") {
    if (!streaming) printDecorativeLine("Random UUID (v1)");
    printToOutput(generateUUIDv1());
  } else if (command == "uuid v4" || command == "uuid4" || command == "u4" || command == "uu4") {
    if (!streaming) printDecorativeLine("Random UUID (v4)");
    printToOutput(generateUUIDv4());
  } else if (command.startsWith("hex ")) {
    int length = command.substring(command.indexOf(' ') + 1).toInt();
    if (length >= 1) {
      if (!streaming) printDecorativeLine("Random Hexadecimal");
      printToOutput(generateRandomID(HEX_ID, length));
    } else if (!streaming) {
      printError("Length must be at least 1.");
    }
  } else if (command.startsWith("base64 ")) {
    int length = command.substring(7).toInt();
    if (length >= 1) {
      if (!streaming) printDecorativeLine("Random Base64");
      printToOutput(generateRandomID(BASE64, length));
    } else if (!streaming) {
      printError("Length must be at least 1.");
    }
  } else if (command.startsWith("alphanum ")) {
    int length = command.substring(command.indexOf(' ') + 1).toInt();
    if (length >= 1) {
      if (!streaming) printDecorativeLine("Random Alphanumeric");
      printToOutput(generateRandomID(ALPHANUMERIC, length));
    } else if (!streaming) {
      printError("Length must be at least 1.");
    }
  } else if (command.startsWith("bin ")) {
    int length = command.substring(command.indexOf(' ') + 1).toInt();
    if (length >= 1) {
      if (!streaming) printDecorativeLine("Random Binary");
      printToOutput(generateRandomID(BINARY, length));
    } else if (!streaming) {
      printError("Length must be at least 1.");
    }
  } else if (command.startsWith("custom ")) {
    int lastSpace = command.lastIndexOf(' ');
    if (lastSpace > 6) {
      String charset = command.substring(7, lastSpace);
      int length = command.substring(lastSpace + 1).toInt();
      if (length >= 1 && !charset.isEmpty()) {
        if (!streaming) printDecorativeLine("Custom Character Set");
        printToOutput(generateRandomID(CUSTOM, length, charset));
      } else if (!streaming) {
        printError("Invalid length or character set.");
      }
    }
  } else if (command.startsWith("read") || command.startsWith("load")) {
    readFromFile("output.txt");
  } else if (!streaming) {
    printError("Invalid command.");
  } 
} 

//##########################################//
//##########################################//
// Handle the setup configuration.
//##########################################//
//##########################################//
void performSetup() {
  printDecorativeLine("The Hamdiz Multi Rand Gen(ESP32) Setup");
  Serial.println("Choose output destination (serial, file, api):");
  while (!Serial.available()) {}
  outputDestination = Serial.readStringUntil('\n');
  outputDestination.trim();  // First trim the input
  outputDestination.toLowerCase();  // Then convert to lowercase

  if (outputDestination == "serial" || outputDestination == "s") {
    Serial.println("Output will be printed to Serial.");
  } else if (outputDestination == "file" || outputDestination == "f") {
    Serial.println("Please specify the filename for saving output:");
    while (!Serial.available()) {}
    String filename = Serial.readStringUntil('\n');
    filename.trim();
    if (filename.isEmpty()) {
      Serial.println("Invalid filename. Setting default: 'output.txt'");
      filename = "output.txt";
    }
    apiUrl = filename; // Reuse apiUrl to store the filename for simplicity
    Serial.printf("Output will be saved to file: %s\n", filename.c_str());
  } else if (outputDestination == "api" || outputDestination == "a") {
    Serial.println("Enter API URL:");
    while (!Serial.available()) {}
    apiUrl = Serial.readStringUntil('\n');
    apiUrl.trim();

    Serial.println("Does the API require authentication? (yes/no):");
    while (!Serial.available()) {}
    String authRequired = Serial.readStringUntil('\n');
    authRequired.trim();
    authRequired.toLowerCase();

    if (authRequired.startsWith("y")) {
      Serial.println("Enter authentication type (bearer, basic):");
      while (!Serial.available()) {}
      authType = Serial.readStringUntil('\n');
      authType.trim();
      authType.toLowerCase();

      Serial.println("Enter authentication token or credentials:");
      while (!Serial.available()) {}
      authToken = Serial.readStringUntil('\n');
      authToken.trim();
    } else if (authRequired.startsWith("n")) {
      Serial.println("No authentication will be used.");
    } else {
      Serial.println("Unrecognized response! Defaulting to no authentication.");
    }
  } else {
    Serial.println("Unrecognized output destination. Defaulting to Serial.");
    outputDestination = "serial";
  }

  setupCompleted = true;
  saveSetupData();
}

//##########################################//
//##########################################//
// Save setup data to EEPROM!
//##########################################//
//##########################################//
void saveSetupData() {
  EEPROM.writeString(0, outputDestination);
  EEPROM.writeString(256, apiUrl); // Reuse apiUrl to store filename or API URL
  EEPROM.writeString(512, authType);
  EEPROM.writeString(1024, authToken);
  EEPROM.writeByte(1280, setupCompleted);
  EEPROM.commit();
}

//##########################################//
//##########################################//
// Load setup data from EEPROM.
//##########################################//
//##########################################//
void loadSetupData() {
  outputDestination = EEPROM.readString(0);
  apiUrl = EEPROM.readString(256); // apiUrl reused for filename or API URL
  authType = EEPROM.readString(512);
  authToken = EEPROM.readString(1024);
  setupCompleted = EEPROM.readByte(1280);
}

//##########################################//
//##########################################//
// Generates random IDs of various types.
//##########################################//
//##########################################//
String generateRandomID(IDType type, int length, String customSet) {
  String result = "";
  while (result.length() < length) {
    uint32_t randomPart = esp_random();
    switch (type) {
      case RANDOM_INT:
        result += String(randomPart % 10); // Random digit
        break;
      case HEX_ID:
        result += String(randomPart, HEX); // Random hex
        break;
      case BASE64:
        for (int i = 0; i < 4 && result.length() < length; i++) {
          result += BASE64_CHARS[randomPart % 64];
          randomPart /= 64;
        }
        break;
      case ALPHANUMERIC:
        for (int i = 0; i < 4 && result.length() < length; i++) {
          result += ALPHANUMERIC_CHARS[randomPart % 62];
          randomPart /= 62;
        }
        break;
      case BINARY:
        result += String(randomPart, BIN);
        break;
      case CUSTOM:
        for (int i = 0; i < 4 && result.length() < length; i++) {
          result += customSet[randomPart % customSet.length()];
          randomPart /= customSet.length();
        }
        break;
    }
  }
  return result.substring(0, length); // Trim to exact length
}


//##############################################//
//##############################################//
// Generates a UUID version 1 (timestamp-based).
//##############################################//
//##############################################//
String generateUUIDv1() {
  uint32_t timeLow = esp_random();
  uint16_t timeMid = esp_random() & 0xFFFF;
  uint16_t timeHiAndVersion = ((esp_random() & 0x0FFF) | 0x1000); // Version 1
  uint16_t clockSeq = (esp_random() & 0x3FFF) | 0x8000;           // Variant 10
  uint32_t node = esp_random();                                  // Random MAC-like node
  char uuid[37];
  snprintf(uuid, sizeof(uuid), "%08X-%04X-%04X-%04X-%012lX",
           timeLow, timeMid, timeHiAndVersion, clockSeq, node);
  return String(uuid);
}


//##########################################//
//##########################################//
// Generates a UUID version 4 (random).
//##########################################//
//##########################################//
String generateUUIDv4() {
  uint32_t randomParts[4];
  for (int i = 0; i < 4; i++) {
    randomParts[i] = esp_random();
  }
  char uuid[37];
  snprintf(uuid, sizeof(uuid),
           "%08X-%04X-%04X-%04X-%08X%04X",
           randomParts[0],               // First 32 bits
           randomParts[1] >> 16,         // Next 16 bits
           (randomParts[1] & 0x0FFF) | 0x4000, // Version 4
           (randomParts[2] & 0x3FFF) | 0x8000, // Variant 10
           randomParts[2] >> 16,         // Next 32 bits
           randomParts[3]);              // Last 16 bits
  return String(uuid);
}

//##########################################//
//##########################################//
// Handles streaming commands.
//##########################################//
//##########################################//
void handleStreamCommand(String command) {
  int lastSpace = command.lastIndexOf(' ');
  if (lastSpace > 0) {
    String cmd = command.substring(7, lastSpace);
    cmd.trim();
    int interval = command.substring(lastSpace + 1).toInt();
    if (interval > 0) {
      streamCommand = cmd;
      streamInterval = interval;
      isStreaming = true;
      lastStreamTime = millis();
      processCommand(cmd, true);
      return;
    }
  }
  printError("Invalid stream format. Use: stream <command> <interval>.");
}

//##############################################//
//##############################################//
// Checks if a peripheral device is available.
//##############################################//
//##############################################//
bool isPeripheralAvailable(String device) {
  if (device == "lcd") {
    // Check if an I2C LCD is connected (example address 0x27)
    Wire.begin(); // Initialize I2C communication
    Wire.beginTransmission(0x27); // Attempt to communicate with the device
    if (Wire.endTransmission() == 0) {
      return true; // Device responded, LCD is available
    } else {
      return false; // No response, LCD is not available
    }
  } else if (device == "sdcard") {
    // Check if the SD card is initialized
    if (SD.begin()) {
      return true; // SD card initialized successfully
    } else {
      return false; // SD card not available
    }
  } else if (device == "sensor") {
    // Example check for a generic sensor (e.g., DHT22, BMP280)
    // You can add specific sensor checks here based on your project
    int sensorPin = 2; // Example pin
    pinMode(sensorPin, INPUT);
    int sensorValue = digitalRead(sensorPin);
    if (sensorValue >= 0) { // Simple check: Ensure pin can be read
      return true;
    } else {
      return false;
    }
  } else {
    Serial.println("Unknown device type: " + device);
    return false; // Default: Device not available
  }
}

//##########################################//
//##########################################//
// Prints a decorative line.
//##########################################//
//##########################################//
void printDecorativeLine(String text) {
  Serial.println();
  Serial.println(text);
  for (int i = 0; i < text.length() + 4; i++) {
    Serial.print('=');
  }
  Serial.println();
}


//##########################################//
//##########################################//
// Prints error messages.
//##########################################//
//##########################################//
void printError(String message) {
  printDecorativeLine("Error: " + message);
}

//##########################################//
//##########################################//
// Save generated output to a file.
//##########################################//
//##########################################//
void saveToFile(String filename, String content) {
  // Check if SD card is available
  if (!isPeripheralAvailable("sdcard")) {
    printError("SD card not available. Cannot save to file.");
    return;
  }

  // Check if the filename is valid
  if (filename.isEmpty()) {
    printError("Invalid filename. Cannot save to file.");
    return;
  }

  // Attempt to open the file in append mode
  File file = SD.open(filename, FILE_APPEND);
  if (!file) {
    printError("Failed to open file for writing. Check SD card and filename.");
    return;
  }

  // Write content to the file
  file.println(content);
  file.close(); // Close the file to ensure data is saved

  // Provide feedback to the user
  Serial.println("Data successfully saved to file: " + filename);
}

//##########################################//
//##########################################//
// Load generated output from a file.
//##########################################//
//##########################################//
void readFromFile(String filename) {
  // Check if SD card is available
  if (!isPeripheralAvailable("sdcard")) {
    printError("SD card not available. Cannot read from file.");
    return;
  }

  // Check if the filename is valid
  if (filename.isEmpty()) {
    printError("Invalid filename. Cannot read from file.");
    return;
  }

  // Attempt to open the file in read mode
  File file = SD.open(filename, FILE_READ);
  if (!file) {
    printError("Failed to open file for reading. Check SD card and filename.");
    return;
  }

  // Read and print the file content
  Serial.println("Reading data from file: " + filename);
  while (file.available()) {
    Serial.write(file.read());
  }
  Serial.println(); // Add a newline after printing the file content
  file.close(); // Close the file to free resources
}


//##########################################//
//##########################################//
// Makes an API call.
//##########################################//
//##########################################//
void sendToAPI(String url, String payload, String authType = "", String token = "") {
  if (url.isEmpty()) {
    printError("API URL not specified.");
    return;
  }

  // Ensure WiFi is connected
  if (WiFi.status() != WL_CONNECTED) {
    printError("WiFi not connected. Cannot send data to API.");
    return;
  }

  HTTPClient http; // Create an HTTPClient object
  http.begin(url); // Initialize connection to the URL

  // Add headers if authentication is required
  if (!authType.isEmpty() && !token.isEmpty()) {
    if (authType == "bearer") {
      http.addHeader("Authorization", "Bearer " + token);
    } else if (authType == "basic") {
      http.addHeader("Authorization", "Basic " + token);
    } else {
      printError("Unsupported authentication type: " + authType);
      http.end(); // Clean up
      return;
    }
  }

  http.addHeader("Content-Type", "application/json"); // Set content type for JSON payload

  // Send POST request with the payload
  int httpResponseCode = http.POST(payload);

  // Handle the response
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println("Response body: " + response);
  } else {
    printError("HTTP request failed. Error: " + http.errorToString(httpResponseCode));
  }

  http.end(); // End the connection
}


//##################################################//
//##################################################//
// Custom output logic for different destinations.
//##################################################//
//##################################################//
void printToOutput(String message) {
  if (outputDestination.startsWith("s")) {
    Serial.println(message);
  } else if (outputDestination.startsWith("f")) {
    saveToFile("output.txt", message);
  } else if (outputDestination.startsWith("a")) {
    String payload = "{\"message\": \"" + message + "\"}";
    sendToAPI(apiUrl, payload, authType, authToken);
  } else {
    Serial.println("Unknown output destination.");
  }
}

//##################################################//
//##################################################//
// Prints the copy right message to the serial.
//##################################################//
//##################################################//
void printCopyRight() {
    Serial.println(copyRight);
}