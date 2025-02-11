
#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

// #define SCK 14
// #define MISO 12
// #define MOSI 13
// #define NSS 18

#define SCK 18
#define MISO 19
#define MOSI 23
#define NSS 5


// LoRa frequency (adjust for your region)
#define LORA_BAND 432E6 // Change to 433E6 if using 433 MHz

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa Sender");

  // Initialize LoRa
  LoRa.setPins(NSS, -1, -1); // NSS, Reset, DIO0 (not used here)
  if (!LoRa.begin(LORA_BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa initialized successfully.");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Received packet: ");

    // Read the incoming message
    while (LoRa.available()) {
      String received = LoRa.readString();
      Serial.print(received);
    }
    Serial.println();

    // Print RSSI (signal strength)
    Serial.print("RSSI: ");
    Serial.println(LoRa.packetRssi());
  }
}