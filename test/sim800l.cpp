#include <HardwareSerial.h>

// Define the pins for the SIM800L module
#define RX_PIN 16 // Connect to SIM800L TX
#define TX_PIN 17 // Connect to SIM800L RX

// Create a second hardware serial instance for SIM800L
HardwareSerial sim800l(1);

void setup() {
  // Initialize serial communication
  Serial.begin(115200);          // For debugging via Serial Monitor
  sim800l.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN); // Initialize SIM800L

  delay(2000); // Wait for the SIM800L to stabilize

  // Test communication with the SIM800L module
  Serial.println("Initializing SIM800L...");
  sendATCommand("AT"); // Basic AT command to check communication
  
  // Set SMS mode to text mode
  sendATCommand("AT+CMGF=1");
  
  // Send an SMS
  sendSMS("+1234567890", "Hello from ESP32 using SIM800L!");
}

void loop() {
  // Nothing to do here
}

// Function to send AT commands to the SIM800L and read the response
void sendATCommand(String command) {
  sim800l.println(command); // Send command to SIM800L
  delay(100);               // Wait for a response
  while (sim800l.available()) {
    String response = sim800l.readString(); // Read the response
    Serial.println(response);               // Print the response to Serial Monitor
  }
}

// Function to send an SMS
void sendSMS(String phoneNumber, String message) {
  // Set the recipient's phone number
  sim800l.print("AT+CMGS=\"");
  sim800l.print(phoneNumber);
  sim800l.println("\"");
  delay(100);

  // Send the SMS message
  sim800l.print(message);
  delay(100);

  // End the SMS message with Ctrl+Z (ASCII code 26)
  sim800l.write(26);
  delay(5000); // Wait for the message to be sent

  // Read and display the response
  while (sim800l.available()) {
    String response = sim800l.readString();
    Serial.println(response);
  }
}
