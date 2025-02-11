#include <HardwareSerial.h>
#include <TinyGPS++.h>

// Create HardwareSerial object for GPS
HardwareSerial GPS(2); // UART2 for Neo6M GPS Module
TinyGPSPlus gps;        // Create TinyGPS++ object

#define GPS_TX_PIN 17  // ESP32 TX pin connected to GPS RX
#define GPS_RX_PIN 14 // ESP32 RX pin connected to GPS TX

void setup() {
  Serial.begin(115200); // For debugging
  GPS.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN); // Initialize UART2 for GPS

  delay(2000); // Wait for GPS module to initialize

  Serial.println("Initializing Neo6M GPS Module...");
}

void loop() {
  // Continuously read data from GPS module
  while (GPS.available()) {
    char c = GPS.read();
    gps.encode(c);

    // Debugging: Print raw NMEA sentences
    Serial.write(c);

    // Check if new GPS data is available
  
      Serial.println("\n---- GPS Data ----");
      Serial.print("Latitude: ");
      Serial.println(gps.location.lat(), 6);
      Serial.print("Longitude: ");
      Serial.println(gps.location.lng(), 6);
      Serial.print("Altitude: ");
      Serial.println(gps.altitude.meters());
      Serial.print("Satellites: ");
      Serial.println(gps.satellites.value());
      Serial.print("HDOP: ");
      Serial.println(gps.hdop.hdop());
      Serial.print("time");
      Serial.print(gps.time.hour());
      Serial.print(":");
      Serial.print(gps.time.minute());
      Serial.print(":");
      Serial.println(gps.time.second());
      Serial.print("date :");
      Serial.print(gps.date.year());
      Serial.print("/");
      Serial.print(gps.date.month());
      Serial.print("/");
      Serial.println(gps.date.day());
      
    
  }
}
