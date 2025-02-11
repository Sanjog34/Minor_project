#include<Arduino.h>
#include <SPI.h>

#define SCK 18
#define MISO 19
#define MOSI 23
#define NSS 5

void setup() {
  Serial.begin(115200);
  pinMode(NSS, OUTPUT);
  digitalWrite(NSS, HIGH); // Set NSS high

  SPI.begin(SCK, MISO, MOSI, NSS);
  Serial.println("SPI Test Initialized");
}

void loop() {
  digitalWrite(NSS, LOW); // Select LoRa module
  uint8_t response = SPI.transfer(0x42); // Send a test byte
  digitalWrite(NSS, HIGH); // Deselect LoRa module

  Serial.print("Response: 0x");
  Serial.println(response, HEX);
  delay(1000);
}
