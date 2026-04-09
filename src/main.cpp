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
bool messageReadyToSend = false;

// Bluetooth device name
const String deviceName = "ESP32_OLED-1";

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
  display.setCursor(0, 0);
  display.println("Menu:");
  display.println("----------------");
  display.println("1. Send Message - 1");
  display.println("2. Received Messages - 2");
  display.println("3. See configuration - 3");
  display.println("----------------");
  display.println("Enter option number:");
  display.display();
}

void initialScreen(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Menu:");
  display.println("----------------");
  display.println("1. Send Message - 1");
  display.println("2. Received Messages - 2");
  display.println("3. See configuration - 3");
  display.println("----------------");
  display.println("Enter option number:");
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
void showConfig() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Configuration:");
  display.println("----------------");
  display.println("LoRa Frequency: " + String(LORA_FREQUENCY / 1E6) + " MHz");
  display.println("Bluetooth Name: " + deviceName);
  display.println("----------------");
  display.println("Press button to return to menu");
  display.display();
  
  while (digitalRead(BUTTON_PIN) == HIGH) {
    // Wait for button press
    delay(100);
  }
  initialScreen();

}
void sendViaLoRa(String message) {
  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();
  Serial.println("Sent via LoRa: " + message);
  
  // Show sent message on OLED
  displayStatus("Sent via LoRa:", message, "Waiting for BT message...");
  SerialBT.println("LoRa TX OK: " + message);
  while (digitalRead(BUTTON_PIN) == HIGH) {
    // Wait for button press
    delay(100);
  }
}

void outBox(){
  bool continueRunning = true;
  receivedText = "";
  displayStatus("Message to send:", "", "Press button to return to menu");

  while(continueRunning){
    if (SerialBT.available()) {
      char incomingChar = SerialBT.read();
      
      if (incomingChar != '\n'){ // If not newline, build string
        receivedText += String(incomingChar);
      } else {
        // If newline, message is ready to send
        messageToSend = receivedText;
        Serial.println("Received via BT: "+messageToSend);
        displayStatus("Message to send:", messageToSend, "Press button to send");
        receivedText = "";
      }
    }
    if(digitalRead(BUTTON_PIN) == LOW){
      if(messageToSend != ""){
        sendViaLoRa(messageToSend);
        messageToSend = "";
        delay(500); //debounce delay
      }else{
        continueRunning = false;
      }     
    }
  }
  initialScreen();
}


//menu function
void menu(String message) {
  Serial.println("Entering menu");
  int messageInt = int(message.toInt());
  messageToSend = "";
  switch (messageInt)
  {
  case 1:
    outBox();
    break;
  case 2:
    //recieved messages
    break;
  case 3:
    showConfig();
    break;

  default:
    break;
  }
}


void loop() {
  if (SerialBT.available()) {
    char incomingChar = SerialBT.read();
    
    if (incomingChar != '\n'){ // If not newline, build string
      receivedText += String(incomingChar);
    } else {
      // If newline, message is ready to send
      messageToSend = receivedText;
      Serial.println("Received via BT: "+messageToSend);
      menu(messageToSend);
      
      receivedText = "";
      messageReadyToSend = false;
      
    }
  }
}

