#include <Arduino.h>
#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include <strings.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <LiquidCrystal_I2C.h>

#define DEBUG // Comment this line to disable debug prints
#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#endif

#define SDA_PIN 21
#define SCL_PIN 22
#define PANIC 35
#define TERMINATE 34
#define RED 23
#define GREEN 19
#define BLUE 18

HardwareSerial M65(1);
#define TXM65_PIN 15 // ESP32 TX pin connected to M65 RX
#define RXM65_PIN 26 // ESP32 RX pin connected to M65 TX  26

HardwareSerial GPS(2); // UART2 for Neo6M GPS Module
TinyGPSPlus gps;       // Create TinyGPS++ object
#define GPS_TX_PIN 17  // ESP32 TX pin connected to GPS RX
#define GPS_RX_PIN 16  // ESP32 RX pin connected to GPS TX

LiquidCrystal_I2C lcd(0x27, 16, 2);

bool IsSettingUp = false;
bool button_pressed = false;
bool termination_button = false;
bool Wifi_status = false;
bool NEW = true;
long int time_keeper;
int count = 0;
const char *ssid = "ESP32_Hotspot";
const char *password = "12345678";
String fromUser = "I need help. My location : ";
String message = "";
String resp = "";
String phoneNumber = "";
String PhoneName = "";
String Command = "";
String ipAddress = "";
int delayVal = 30000;

WebServer server(80);

void disp(String mes)
{
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(mes);
}

void disp2(String mes1, String mes2)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(mes1);
  lcd.setCursor(0, 1);
  lcd.print(mes2);
}

void led(int red, int green, int blue)
{
  digitalWrite(RED, red);
  digitalWrite(GREEN, green);
  digitalWrite(BLUE, blue);
}

void blink(int del)
{
  led(1, 1, 0);
  delay(del);
  led(0, 0, 0);
  delay(del);
  led(1, 1, 0);
  delay(del);
  led(0, 0, 0);
}

void handleData()
{
  if (server.hasArg("phone_no") && server.hasArg("message") && server.hasArg("delay"))
  {
    phoneNumber = server.arg("phone_no");
    fromUser = server.arg("message");
    String delayTime = server.arg("delay");
    DEBUG_PRINT("Received Phone no. :" + phoneNumber);
    DEBUG_PRINT("Received Message: " + fromUser);
    DEBUG_PRINT("Received Delay: " + delayTime);
    delayVal = delayTime.toInt();
    DEBUG_PRINT("Converted Delay: " + String(delayVal) + " ms");
    server.send(200, "text/plain", "phone number: " + phoneNumber + " Message: " + fromUser + "\nDelay: " + String(delayVal) + " ms");
    IsSettingUp = false;
    DEBUG_PRINT("Setup done(from user)!!!!!");
    disp("Done setting up!");
    blink(100);
    led(0, 0, 0);
    WiFi.mode(WIFI_OFF);
    Wifi_status = false;
    delay(1000);
  }
  else
  {
    server.send(400, "text/plain", "Missing 'message' or 'delay' parameter");
  }
}

void handleRoot()
{
  File file = SPIFFS.open("/index.html", "r");
  if (!file)
  {
    server.send(404, "text/plain", "File Not Found");
    return;
  }
  server.streamFile(file, "text/html");
  file.close();
}

void sendATCommand(String command)
{
  DEBUG_PRINT(command);
  M65.println(command);
  delay(1000);
}

void PhoneBook(int mode, int index)
{
  sendATCommand("AT+CPBS=\"SM\"");
  delay(500);
  if (mode == 0)
  { // read first number from phonebook
    Command = "AT+CPBR=" + String(index);
    M65.println(Command);
    delay(500);
    while (M65.available())
    {
      String line = M65.readStringUntil('\n');
      line.trim();
      Serial.println("<< " + line); // Debug: show each line

      if (line.startsWith("+CPBR:"))
      {
        resp = line;

        // Extract number from line
        int firstQuote = line.indexOf('"');
        int secondQuote = line.indexOf('"', firstQuote + 1);
        int thirdQuote = line.indexOf('"', secondQuote + 1);
        int fourthQuote = line.indexOf('"', thirdQuote + 1);
        if (firstQuote > 0 && secondQuote > firstQuote)
        {
          phoneNumber = line.substring(firstQuote + 1, secondQuote);
          PhoneName = line.substring(thirdQuote + 1, fourthQuote);
          Serial.println("Extracted Number: " + phoneNumber);
          Serial.println("extracted Name: " + PhoneName);
        }
      }
    }
  }
  else if (mode == 1)
  { // store number in phonebook
    Command = "AT+CPBW=" + String(index) + ",\"" + phoneNumber + "\",129,\"" + "1emergancy" + "\"";
    sendATCommand(Command);
  }
}

void ReadGPS()
{
  while (GPS.available() > 0)
  {
    char c = GPS.read();
    gps.encode(c);
  }
}

void Create_message()
{
  ReadGPS();
  delay(500);
  message.reserve(200);
  message += fromUser;
  message += " https://www.google.com/maps?q=";
  message += String(gps.location.lat(), 6) + "," + String(gps.location.lng(), 6);
}

void Send_message(String MESSAGE)
{
  delay(500);
  sendATCommand("AT");
  delay(500);
  sendATCommand("AT+CMGF=1"); // Set SMS text mode
  delay(500);
  Command = "AT+CMGS=\"" + phoneNumber + "\"";
  sendATCommand(Command);
  delay(500);
  M65.print(MESSAGE); // Message content
  delay(500);
  M65.write(26); // End message with Ctrl+Z (ASCII 26)
  delay(500);
  DEBUG_PRINT("Message Sent!");
  blink(100);
  led(0, 0, 1);
}

void panic_mode()
{
  disp2("SENDING MESSAGE", phoneNumber);
  delay(100);
  led(0, 0, 1);
  time_keeper = millis();
  count++;
  button_pressed = true;
  termination_button = false;
  Create_message();
  DEBUG_PRINT(message);
  Send_message(message);
  disp2("MESSAGE SENT!!", String(count));
  message = "";
  delay(100);
}

void terminate_mode()
{
  if (button_pressed)
  {
    disp2("TERMINATING", "PANIC MODE");
    delay(100);
    led(0, 1, 0);
    button_pressed = false;
    termination_button = true;
    count = 0;
    message += "termination button pressed.";
    DEBUG_PRINT(message);
    Send_message(message);
    DEBUG_PRINT("panic mode deactivated");
    message = "";
    led(0, 0, 0);
    delay(100);
    disp2("PANIC MODE", "TERMINATED");
    delay(500);
    NEW = true;
  }
  else
  {
    blink(500);
  }
}

String EnableWifi()
{
  Wifi_status = true;
  WiFi.softAP(ssid, password);
  DEBUG_PRINT("ESP32 Access Point Started!");
  Serial.print("ESP32 AP IP Address: ");
  DEBUG_PRINT(WiFi.softAPIP());
  ipAddress = WiFi.softAPIP().toString();
  disp2("SETUP LINK:", ipAddress);
  if (!SPIFFS.begin(true))
  {
    DEBUG_PRINT("SPIFFS Initialization Failed!");
  }
  DEBUG_PRINT("setting up");
  server.on("/", HTTP_GET, handleRoot);
  server.on("/send-data", HTTP_POST, handleData);
  server.begin();
  return ipAddress;
}

void setup()
{
  pinMode(PANIC, INPUT);
  pinMode(TERMINATE, INPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  Serial.begin(115200); // For debugging
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.begin(16, 2);
  lcd.backlight();
  delay(500);
  disp("M65 SETUP");
  led(0, 0, 1);
  DEBUG_PRINT("Initializing M65 Module...");
  M65.begin(9600, SERIAL_8N1, RXM65_PIN, TXM65_PIN); // Initialize GSM with 9600 baud rate
  delay(1000);                                       // Wait for the module to initialize
  sendATCommand("AT");
  delay(1000);
  disp("GPS SETUP");
  DEBUG_PRINT("Initializing Neo6M GPS Module...");
  GPS.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN); // Initialize GPS with 9600 baud rate
  delay(1000);                                         // Wait for GPS module to initialize
  led(0, 1, 0);
  DEBUG_PRINT("fetching previous data");
  disp2("INITIALIZING", "DEVICE");
  PhoneBook(0, 1);
  Serial.println(PhoneName);
  if (strcmp("1emergancy", PhoneName.c_str()))
  {
    Serial.println("not setup yet");
    disp2("setup your ", "device");
    IsSettingUp = true;
    delay(500);
  }
  else
  {
    IsSettingUp = false;
    PhoneBook(0, 1);
    delay(50);
    disp("READY TO GO!!");
    DEBUG_PRINT("Ready to go");
    blink(100);
    led(0, 0, 0);
  }
  DEBUG_PRINT("passed");
}

void loop()
{
  while (IsSettingUp || (digitalRead(PANIC) && digitalRead(TERMINATE) && !button_pressed))
  {
    IsSettingUp = true;
    if (!Wifi_status)
    {
      disp2("setup link:", EnableWifi());
    }
    while (IsSettingUp)
    {
      server.handleClient(); // Serial.println("handeling client");
    }
    NEW = true;
    PhoneBook(1, 1);
    delay(50);
    PhoneBook(0, 1);
  }
  if (!button_pressed && NEW)
  {
    disp2("WATCH ON", "STANDBY MODE");
    NEW = false;
  }
  if (digitalRead(PANIC) && !button_pressed && !digitalRead(TERMINATE))
  {
    delay(200);
    disp2("PANIC BTN PRESSED!!", "SEND MESSAGE?(P)");
    DEBUG_PRINT("you pressed PANIC BUTTON. DO you really want to send SOS?");
    led(1, 0, 0);
    while (!digitalRead(PANIC) && !digitalRead(TERMINATE));
    if (digitalRead(PANIC))
      panic_mode();
    else if (digitalRead(TERMINATE))
    {
      led(0, 0, 0);
      NEW = true;
    }
  }

  if (button_pressed && !termination_button)
  {
    if (millis() - time_keeper > delayVal)
    {
      panic_mode();
    }
  }
  if (digitalRead(TERMINATE) && button_pressed && !digitalRead(PANIC))
  {
    terminate_mode();
  }
}