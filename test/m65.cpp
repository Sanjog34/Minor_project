#include <HardwareSerial.h>
#include <Arduino.h>
// Create a HardwareSerial object
HardwareSerial M65(1); // Use UART1

#define TX_PIN 2  // ESP32 TX pin connected to M65 RX
#define RX_PIN 4  // ESP32 RX pin connected to M65 TX

void setup() {
  Serial.begin(115200); // For debugging
  M65.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN); // Initialize UART1 with 9600 baud rate

  delay(3000); // Wait for the module to initialize

  Serial.println("Initializing M65 Module...");

  // Send AT command to check communication
  sendATCommand("AT");
  delay(1000);

  // Set SMS text mode
  sendATCommand("AT+CMGF=1");
  delay(1000);

  // Set the recipient's phone number (replace with actual number)
  sendATCommand("AT+CMGS=\"+9779708849452\"");
  delay(1000);

  // Message content
  M65.print("Hello from ESP32 and M65!");
  delay(500);

  // End message with Ctrl+Z (ASCII 26)
  M65.write(26);
  delay(3000);

  Serial.println("Message Sent!");
}

void loop() {
  // Nothing to do here
}

void sendATCommand(const char* command) {
  M65.println(command);
  delay(500);
  while (M65.available()) {
    Serial.write(M65.read()); // Display response from M65
  }
}
