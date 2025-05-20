#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SDA_PIN 21
#define SCL_PIN 22

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
    Wire.begin(SDA_PIN, SCL_PIN);  // Initialize I2C with custom pins
    lcd.begin(16, 2);  // Initialize LCD with 16x2 size
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Emergency Rsponse");
    Wire.end();
}

void loop() {
    // Nothing needed in loop for static message
}
