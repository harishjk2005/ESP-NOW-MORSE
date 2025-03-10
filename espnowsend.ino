#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int buttonPin = 23;    // GPIO pin connected to the button
bool buttonState = LOW;      // Current state of the button
bool lastButtonState = LOW;  // Previous state of the button

unsigned long lastReceiveTime = 0;     // Last activity time
unsigned long lastDebounceTime = 0;    // Timestamp of the last debounce check
const unsigned long debounceDelay = 100; // Debounce time in milliseconds
unsigned long pressStartTime = 0;      // Time when the button was pressed
unsigned long releaseStartTime = 0;    // Time when the button was released

String morseCode = "";

// Peer MAC address (receiver device MAC)
uint8_t receiverMAC[] = {0x94, 0xb9, 0x7e, 0xe4, 0x54, 0x64};  // Replace with the correct MAC

const char* morseTable[][2] = {
  {".-", "A"}, {"-...", "B"}, {"-.-.", "C"}, {"-..", "D"}, {".", "E"},
  {"..-.", "F"}, {"--.", "G"}, {"....", "H"}, {"..", "I"}, {".---", "J"},
  {"-.-", "K"}, {".-..", "L"}, {"--", "M"}, {"-.", "N"}, {"---", "O"},
  {".--.", "P"}, {"--.-", "Q"}, {".-.", "R"}, {"...", "S"}, {"-", "T"},
  {"..-", "U"}, {"...-", "V"}, {"..--", "W"}, {"-..-", "X"}, {"-.--", "Y"},
  {"--..", "Z"}, {"-----", "0"}, {".----", "1"}, {"..---", "2"}, {"...--", "3"},
  {"....-", "4"}, {".....", "5"}, {"-....", "6"}, {"--...", "7"}, {"---..", "8"},
  {"----.", "9"}
};

int x = 0;
int y = 10;

// Flag to track receiving status
bool isReceiving = false;

char decodeMorse(String code) {
  for (int i = 0; i < sizeof(morseTable) / sizeof(morseTable[0]); i++) {
    if (code == morseTable[i][0]) {
      return morseTable[i][1][0];
    }
  }
  return '?'; // Return '?' if the code is not recognized
}

void displayDecodedChar(char decodedChar) {
  if (x > SCREEN_WIDTH - 12) {
    x = 0;
    y += 16; 
  }

  if (y > SCREEN_HEIGHT - 16) {
    x = 0;
    y = 10;
    display.clearDisplay();
  }

  display.setCursor(x, y);
  display.setTextSize(2);
  display.print(decodedChar);
  display.display();

  x += 12;
}

void onDataReceive(const esp_now_recv_info *info, const uint8_t *data, int len) {
  // Set the flag to indicate receiving process
  isReceiving = true;

  char incomingMessage[len + 1];
  memcpy(incomingMessage, data, len);
  incomingMessage[len] = '\0';
  String morseCode = String(incomingMessage);
  char decodedChar = decodeMorse(morseCode);

  Serial.print("Received Morse Code: ");
  Serial.println(morseCode);
  Serial.print("Decoded Character: ");
  Serial.println(decodedChar);

  displayDecodedChar(decodedChar);

  // Reset the timer for display clear
  lastReceiveTime = millis();

  // Receiving process completed
  isReceiving = false;
}

void setup() {
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Morse Code");
  display.display();
  delay(2000);
  display.clearDisplay();
  pinMode(buttonPin, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Initialization Failed!");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  esp_now_register_recv_cb(onDataReceive);
}

void sendMorseCode(String code) {
  if (isReceiving) {
    Serial.println("Cannot send Morse code while receiving data.");
    return;
  }

  const char* message = code.c_str();
  esp_err_t result = esp_now_send(receiverMAC, (uint8_t*)message, strlen(message));

  if (result == ESP_OK) {
    Serial.println("Message sent successfully: " + code);
  } else {
    Serial.println("Failed to send message");
  }
}

void loop() {
  int reading = digitalRead(buttonPin);

  // Update the last activity time if there is input
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
    lastReceiveTime = millis(); // Reset the timer for display clear
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        pressStartTime = millis();
      } else {
        unsigned long pressDuration = millis() - pressStartTime;

        if (pressDuration < 500) {
          morseCode += ".";
        } else {
          morseCode += "-";
        }

        releaseStartTime = millis();
      }
    }
  }

  if (releaseStartTime > 0 && (millis() - releaseStartTime > 1250)) {
    sendMorseCode(morseCode);
    morseCode = "";
    releaseStartTime = 0;
    lastReceiveTime = millis(); // Reset the timer for display clear
  }
  

  // Clear display if no input or activity for 5 seconds
  if (millis() - lastReceiveTime > 5000) {
    display.clearDisplay();
    display.display();
    

    // Reset cursor position after clearing
    x = 0;
    y = 10;

    lastReceiveTime = millis(); // Reset the timer to avoid repeated clears
  }

  lastButtonState = reading;
}
