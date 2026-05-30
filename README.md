# Bun-1 Communicator

[![en](https://img.shields.io/badge/lang-en-blue)](https://github.com/kielbassa/Bun-1_Communicator/blob/master/README.pl.md)
[![pl](https://img.shields.io/badge/lang-PL-red)](https://github.com/kielbassa/Bun-1_Communicator/blob/master/README.pl.md)

ESP32-based LoRa communicator with OLED status display and Bluetooth control.

This project is designed for an ESP32 TTGO LoRa32 board and uses:
- LoRa radio for wireless messaging
- Bluetooth Serial as the user interface
- OLED display for menus and status
- AES-256 encryption for outgoing and incoming payloads
- Non-volatile storage (NVS) to save device name and AES key

## Features

- Bluetooth menu interface for:
  - sending a LoRa message
  - viewing received messages
  - configuring device settings
- AES-256 CBC encryption/decryption of LoRa payloads
- AES key stored in flash
- inbox buffer for up to 5 received messages
- button-based navigation for the OLED menu

## Hardware

Recommended board:
- `ttgo-lora32-v1`

Configured pins in code:
- OLED / I2C: `SDA=4`, `SCL=15`, `RST=16`
- LoRa: `SCK=5`, `MISO=19`, `MOSI=27`, `SS=18`, `RST=14`, `DIO0=26`
- Button: `GPIO0`
- LoRa frequency: `866 MHz`

## Software

Project built with PlatformIO using the provided `platformio.ini` file.

Dependencies:
- `sandeepmistry/LoRa@^0.8.0`
- `adafruit/Adafruit GFX Library@^1.12.5`
- `adafruit/Adafruit SSD1306@^2.5.16`

## Usage

1. Build and upload the firmware with PlatformIO:
   ```bash
   pio run
   pio run -t upload
   ```
2. Open a Bluetooth terminal app and connect to the ESP32 device. The device name is displayed on the OLED and is usually `ESP32_OLED_##`.
3. Use the Bluetooth terminal to send simple commands:
   - `1` → Send a message
   - `2` → Open inbox
   - `3` → Configuration

### Sending a message

- Select `1` in the Bluetooth terminal.
- Send the message text as a single line.
- Press the hardware button to transmit the message over LoRa.

### Reading received messages

- Select `2` in the Bluetooth menu.
- Incoming LoRa messages are decrypted and shown on the OLED.
- Double-press the button to cycle through stored inbox messages.
- Single press returns to the main menu.

### Configuration

- Select `3` in the Bluetooth menu.
- Single press returns to the main menu.
- Double press enables AES key edit mode.
- Triple press regenerates the device name and restarts Bluetooth.

### AES key editing

- In configuration mode, double press the button.
- Send a 64-character hexadecimal AES key via Bluetooth.
- The key is saved to NVS and used for all future encrypt/decrypt operations.

## Notes

- The firmware uses a fixed IV for AES-CBC, which is suitable for learning and hobby projects but not ideal for production-grade security.
- Ensure all communicating devices share the same AES key for successful decryption.

## Project Structure

- `src/main.cpp` — main firmware and application logic
- `src/encryption_aes.h` — AES encryption/decryption helpers
- `src/graphics.h` — OLED bitmap data and display asset definitions
- `platformio.ini` — build configuration and dependencies
