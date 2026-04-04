//Bluetooth libraries
#include <BluetoothSerial.h>
#include <string.h>

//OLED libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <graphics.h>

//LoRa libraries
#include <LoRa.h>
#include <SPI.h>

//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//LoRa pins
#define LORA_SCK 5
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_SS 18
#define LORA_RST 14
#define LORA_DI0 26

//button pins
#define BUTTON_PIN 0

//Frequency for LoRa
#define LORA_FREQUENCY 866E6

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
BluetoothSerial SerialBT;

// Variables for LoRa
String receivedText = "";
String messageToSend = "";
String incomingLoRaMessage = "";
const String deviceName = "ESP32_OLED-2";
bool messageReadyToSend = false;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  SerialBT.begin(deviceName); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  
  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  //initialize OLED and display buffers
  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false);
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.drawBitmap(0,0,epd_bitmap_bunnytransmssion,64,64,WHITE);
  display.setCursor(64, 10);
  display.println("Waiting");
  display.setCursor(64, 18);
  display.println("for BT...");
  display.display();

  // Initialize LoRa
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);
  if (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("LoRa init failed!");
    while (1);
  }

  Serial.println("LoRa init succeeded.");

  // Initialize Bluetooth
  SerialBT.begin(deviceName); // Bluetooth name
  Serial.println("Device started, pairing allowed");

  //Initial screen
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.println("BT: " + deviceName);
  display.println("LoRa: 866MHz; OK");
  display.println("------------------");
  display.println("Send msg via BT, then press button to send via LoRa");
  display.display();
}

// Function to display status on OLED
void displayStatus(String line1, String message, String line3) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(line1);
  display.println("----------------");
  
  // Crop message if it's too long for the display
  if (message.length() > 21) {
    display.println(message.substring(0, 21));
    display.println(message.substring(21));
  } else {
    display.println(message);
  }
  
  if (line3.length() > 0) {
    display.setCursor(0, 56);
    display.println(line3);
  }
  display.display();
}

void sendMessageViaLoRa(String message) {
  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();
  Serial.println("Sent via LoRa: " + message);
  
  // Show sent message on OLED
  displayStatus("Sent via LoRa:", message, "Waiting for BT message...");
  SerialBT.println("LoRa TX OK: " + message);
}

void loop() {
  if (SerialBT.available()) {
    char incomingChar = SerialBT.read();
    
    if (incomingChar != '\n'){ // If not newline, build string
      receivedText += String(incomingChar);
    } else {
      // If newline, message is ready to send
      messageToSend = receivedText;
      messageReadyToSend = true;
      Serial.println("Received via BT: " + messageToSend);
      
      //Show received message on OLED
      displayStatus("Received via BT:", messageToSend, "Press button to send");
      receivedText = "";
    }
  }
  if(messageReadyToSend && digitalRead(BUTTON_PIN) == LOW) {
    sendMessageViaLoRa(messageToSend);
    messageReadyToSend = false;
  }

  // Check for incoming LoRa messages
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    
    while (LoRa.available()) {
      incomingLoRaMessage += (char)LoRa.read();
    }
    Serial.println("Received via LoRa: " + incomingLoRaMessage);
    
    // Show received LoRa message on OLED
    displayStatus("Received via LoRa:", incomingLoRaMessage, "Waiting for BT message...");
    
    // Send acknowledgment back via Bluetooth
    SerialBT.println("LoRa RX OK: " + incomingLoRaMessage);
  }
}

