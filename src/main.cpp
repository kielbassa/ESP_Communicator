//Bluetooth libraries
#include "BluetoothSerial.h"
#include <string.h>

//OLED libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//other files
#include <graphics.h>

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Bluetooth device name
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

  // Initialize Bluetooth
  SerialBT.begin("ESP32_OLED"); // Bluetooth name
  Serial.println("Device started, pairing allowed");

}

uint8_t iter = 0;
String receivedText = "";

void loop() {
  if (SerialBT.available()) {
    char incomingChar = SerialBT.read();
    
    if (incomingChar != '\n'){ // If not newline, build string
      receivedText += String(incomingChar);
    } else {
      // Display received text
      Serial.println(receivedText);
      display.clearDisplay();
      display.setCursor(0,10);
      display.setTextSize(2);
      display.println(receivedText);
      display.display();
      receivedText = ""; // Clear string
    }
  }
  delay(20);
}
