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
  static int counter = 0;

  Serial.print("Sending packet: ");
  Serial.println(counter);

  LoRa.beginPacket();
  LoRa.print("Hello LoRa ");
  LoRa.print(counter);
  LoRa.endPacket();

  counter++;
  delay(1000); // Send every second
}
