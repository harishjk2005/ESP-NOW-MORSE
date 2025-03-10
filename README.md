# Morse Code Communication Using ESP-NOW

This project enables two-way Morse code communication between ESP32 boards using the ESP-NOW protocol. The system includes a push button for Morse code input, an OLED display for decoding and showing received messages, and ESP-NOW for wireless data transmission.

## Features
- Real-time Morse code transmission between ESP32 boards.
- Decoding of received Morse code into characters.
- Display of decoded characters on an SSD1306 OLED screen.
- Debounce mechanism for button input.
- Automatic display clearing after inactivity.

## Hardware Requirements
- 2x ESP32 boards
- SSD1306 128x64 OLED Display
- Push button
- Connecting wires

## Software Requirements
- Arduino IDE with ESP32 board support
- Required Libraries:
  - `esp_now.h`
  - `WiFi.h`
  - `Wire.h`
  - `Adafruit_GFX.h`
  - `Adafruit_SSD1306.h`

## Installation & Setup
1. Install the ESP32 board in the Arduino IDE.
2. Install the required libraries through the Arduino Library Manager.
3. Connect the hardware as follows:
   - Push button to GPIO 23 with an internal pull-up.
   - SSD1306 OLED to I2C pins (SDA, SCL).
4. Update the `receiverMAC` address with the MAC address of the receiving ESP32.
5. Upload the code to both ESP32 boards.

## How It Works
- Press and hold the button:
  - Short press (<500ms) = `.` (dot)
  - Long press (>500ms) = `-` (dash)
- Release the button for more than 1.25 seconds to send the Morse code.
- The receiving ESP32 decodes and displays the character on the OLED screen.
- The display clears automatically after 5 seconds of inactivity.

## Usage
1. Power on both ESP32 boards.
2. Press the button to send Morse code.
3. Observe decoded characters on the OLED screen.
4. The display resets after inactivity.

## Troubleshooting
- **No message received:** Ensure both ESP32 boards are powered and within range.
- **OLED not displaying characters:** Check I2C connections and address (`0x3C`).
- **ESP-NOW initialization failed:** Verify WiFi mode is set to `WIFI_STA`.

## License
This project is open-source and available under the MIT License.

## Author
Developed by HARISH KARTHIC JK

