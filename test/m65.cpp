#include <HardwareSerial.h>
#include <Arduino.h>
#include<string.h>
// Create a HardwareSerial object
HardwareSerial M65(1); // Use UART1
String resp = "";
String foundNumber = "";
#define TX_PIN 15  // ESP32 TX pin connected to M65 RX
#define RX_PIN 26  // ESP32 RX pin connected to M65 TX


void sendATCommand(String command) {
  M65.println(command);
  delay(500);
  while (M65.available()) {
    String line = M65.readStringUntil('\n');
    line.trim();
    Serial.println("<< " + line);  // Debug: show each line

    if (line.startsWith("+CPBF:")) {
      resp = line;

      // Extract number from line
      int firstQuote = line.indexOf('"');
      int secondQuote = line.indexOf('"', firstQuote + 1);
      if (firstQuote > 0 && secondQuote > firstQuote) {
        foundNumber = line.substring(firstQuote + 1, secondQuote);
        Serial.println("Extracted Number: " + foundNumber);
      }
    }
  }
}



void PhoneBook(int mode){
  sendATCommand("AT+CPBS=\"SM\""); 
  if(mode==0){//read first number from phonebook
    M65.println("AT+CPBR=1");
    delay(500);
    while (M65.available()) {
      String line = M65.readStringUntil('\n');
      line.trim();
      Serial.println("<< " + line);  // Debug: show each line
  
      if (line.startsWith("+CPBR:")) {
        resp = line;
  
        // Extract number from line
        int firstQuote = line.indexOf('"');
        int secondQuote = line.indexOf('"', firstQuote + 1);
        if (firstQuote > 0 && secondQuote > firstQuote) {
          foundNumber = line.substring(firstQuote + 1, secondQuote);
          Serial.println("Extracted Number: " + foundNumber);
        }
      }
    }
  }
  else if(mode==1){//store number in phonebook
    String fullCommand = "AT+CPBW=" + String(1) + ",\"" + String(1234567890) + "\",129,\"" + "1emergancy" + "\"";
    sendATCommand(fullCommand);
  }
}

void setup() {
  Serial.begin(115200); // For debugging
  M65.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN); // Initialize UART1 with 9600 baud rate

  delay(3000); // Wait for the module to initialize

  Serial.println("Initializing M65 Module...");

  // Send AT command to check communication
  sendATCommand("AT");
  delay(3000);

  Serial.println("Message Sent!");
  PhoneBook(1);
  PhoneBook(0);
}

void loop() {
  if(Serial.available()){
String rd = Serial.readStringUntil('\n');
sendATCommand(rd);
}
}




// M65.println("AT+CPIN?");             // Make sure SIM is ready
// delay(500);

// M65.println("AT+CPBS=\"SM\"");       // Select SIM phonebook
// delay(500);

// M65.println("AT+CPBR=?");            // Get valid range
// delay(500);

// M65.println("AT+CPBR=1,10");         // Read first 10 entries