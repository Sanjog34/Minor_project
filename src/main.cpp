#include<Arduino.h>
#include<HardwareSerial.h>
#include <TinyGPS++.h>
#include<strings.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SDA_PIN 21
#define SCL_PIN 22

const char* ssid = "ESP32_Hotspot";  
const char* password = "12345678";    

#define PANIC 4
#define TERMINATE 34
// #define INTERVAL 20000
#define RED 23
#define GREEN 19
#define BLUE 18


HardwareSerial M65(1);
#define TXM65_PIN 15  // ESP32 TX pin connected to M65 RX
#define RXM65_PIN -1  // ESP32 RX pin connected to M65 TX

HardwareSerial GPS(2); // UART2 for Neo6M GPS Module
TinyGPSPlus gps;        // Create TinyGPS++ object
#define GPS_TX_PIN 17  // ESP32 TX pin connected to GPS RX
#define GPS_RX_PIN 16 // ESP32 RX pin connected to GPS TX

LiquidCrystal_I2C lcd(0x27, 16, 2);
bool IsSettingUp=false;
bool button_pressed = false;
bool termination_button =false;
long int time_keeper;
bool NEW=true;
int count=0;
char buff[10]="";
char fromUser[50]="I need help. My location : ";
char* message;
char* lk;
String phoneNumber = "+9779806512990";
int delayVal=30000;

WebServer server(80);

void disp(String mes){
  // Wire.begin(SDA_PIN, SCL_PIN);  // Initialize I2C with custom pins
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(mes);
  // Wire.end();
}
void disp2(String mes1, String mes2){
  // Wire.begin(SDA_PIN, SCL_PIN);  // Initialize I2C with custom pins
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(mes1);
  lcd.setCursor(0, 1);
  lcd.print(mes2);
  // Wire.end();
}

void led(int red, int green ,int blue){
    digitalWrite(RED,red);
    digitalWrite(GREEN,green);
    digitalWrite(BLUE,blue);
  }

  void blink(int del){
    led(1,1,0);
    delay(del);
    led(0,0,0);
    delay(del);
    led(1,1,0);
    delay(del);
    led(0,0,0);
  }

void handleData() {
    if (server.hasArg("phone_no") && server.hasArg("message") && server.hasArg("delay")) {
        phoneNumber = server.arg("phone_no");
        String MSG = server.arg("message");
        strcpy(fromUser, MSG.c_str());
        String delayTime = server.arg("delay");
        Serial.println("Received Phone no. :" + phoneNumber);
        Serial.println("Received Message: " + MSG);
        Serial.println("Received Delay: " + delayTime);
        
        delayVal = delayTime.toInt();
        Serial.println("Converted Delay: " + String(delayVal) + " ms");
        
        // Simulate delay
        // delay(delayVal);
        
        server.send(200, "text/plain","phone number: "+ phoneNumber +  " Message: " + MSG + "\nDelay: " + String(delayVal) + " ms");
        IsSettingUp=false;
        Serial.println("Setup done(from user)!!!!!");
        disp("Done setting up!");
        blink(100);
        led(0,0,0);
        WiFi.mode(WIFI_OFF);
        delay(1000);
        delay(500);
    }
    
    else {
        server.send(400, "text/plain", "Missing 'message' or 'delay' parameter");
    }
    }
  

  
  void handleRoot() {
      File file = SPIFFS.open("/index.html", "r");
      if (!file) {
          server.send(404, "text/plain", "File Not Found");
          return;
      }
  
      server.streamFile(file, "text/html");
      file.close();
  }



void sendATCommand(const char* command) {
  Serial.println(command);
    M65.println(command);
    delay(1000);
    // while (M65.available()) {
    //   Serial.write(M65.read());  // Display response from M65
     
    // }
  
  }



  void ReadGPS(){
    while(GPS.available()>0){
      char c = GPS.read();
      gps.encode(c);
    }
    }
  

    char* int_to_char (int num){
    itoa(num,buff,10);
    return buff;
    }



  void Create_message(){
  ReadGPS();
  delay(500);
  dtostrf(gps.location.lat(),0,5,buff);
  strcat(lk,buff);
  strcat(lk,",");
  dtostrf(gps.location.lng(),0,5,buff);
  strcat(lk,buff);
  strcat(message,lk);
  }




void Send_message(char MESSAGE[]){
  delay(1000);
  sendATCommand("AT");
  delay(1000);

  // Set SMS text mode
  sendATCommand("AT+CMGF=1");
  delay(1000);

  // Set the recipient's phone number (replace with actual number)
  String command = "AT+CMGS=\"" + phoneNumber + "\"";
  sendATCommand(command.c_str());
  delay(1000);

  // Message content
  M65.print(MESSAGE);
  delay(500);

  // End message with Ctrl+Z (ASCII 26)
  M65.write(26);
  delay(3000);

  Serial.println("Message Sent!");
  blink(100);
  led(0,0,1);
}



void panic_mode(){
  disp2("SENDING MESSAGE", phoneNumber);
  delay(100);
  led(0,0,1);
  time_keeper=millis();
  count++;
  button_pressed=true;
  termination_button=false;
  message = (char*)malloc(500);
  lk = (char*)malloc(50);
  strcpy(lk,"https://www.google.com/maps?q=");
  strcpy(message,fromUser);
  Create_message();
  Serial.println(message);
  Send_message(message);
  disp2("MESSAGE SENT!!",String(count));
  free(message);
  free(lk);
  delay(100);
  }
  


  void terminate_mode(){
    if(button_pressed){
    disp2("TERMINATING","PANIC MODE");
    delay(100);
    led(0,1,0);
    button_pressed=false;
    termination_button=true;
    count=0;
    message = (char*)malloc(500);
    strcpy(message,"termination button pressed.");
    Serial.println(message);
     Send_message(message);
    Serial.println("panic mode deactivated");
    free(message);
    led(0,0,0);
    delay(100);
    disp2("PANIC MODE","TERMINATED");
    delay(2000);
    delay(200);
    NEW=true;
    }
    else{
   blink(500);
  }
  }



void setup(){
    pinMode(PANIC, INPUT);
    pinMode(TERMINATE,INPUT);
    pinMode(RED, OUTPUT);
    pinMode(GREEN,OUTPUT);
    pinMode(BLUE, OUTPUT);
    Serial.begin(115200); // For debugging
    Wire.begin(SDA_PIN, SCL_PIN); 
    lcd.begin(16, 2); 
    lcd.backlight();
   
    delay(500);
    disp("M65 SETUP");
    led(0,0,1);
    Serial.println("Initializing M65 Module..."); 
    M65.begin(9600, SERIAL_8N1, RXM65_PIN, TXM65_PIN); // Initialize UART1 with 9600 baud rate
    delay(3000); // Wait for the module to initialize
    sendATCommand("AT");
    delay(1000);
    disp("GPS SETUP");
    Serial.println("Initializing Neo6M GPS Module...");
    GPS.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN); // Initialize UART2 for GPS
    delay(2000); // Wait for GPS module to initialize
    led(0,1,0);

    Serial.println("press 'P' to setup device and press 'T' to go with default values\n");
    disp2("PRESS: P--SETUP","       T--SKIP");

    while(!digitalRead(PANIC) && !digitalRead(TERMINATE));
        if(digitalRead(PANIC)){
            IsSettingUp=true;
            delay(500);
            WiFi.softAP(ssid, password);
            Serial.println("ESP32 Access Point Started!");
            Serial.print("ESP32 AP IP Address: ");
            Serial.println(WiFi.softAPIP());
            String ipAddress = WiFi.softAPIP().toString();
             disp2("SETUP LINK:",ipAddress);
        
            if (!SPIFFS.begin(true)) {
                Serial.println("SPIFFS Initialization Failed!");
                return;
            }
            Serial.println("setting up");
            server.on("/", HTTP_GET, handleRoot);
            server.on("/send-data", HTTP_POST, handleData);
            server.begin();

}
else if(digitalRead(TERMINATE)){
    delay(500);
    IsSettingUp=false;
    disp("READY TO GO!!");
    Serial.println("Ready to go");
    blink(100);
    led(0,0,0);
}
Serial.println("passed");
 }


void loop(){
 while(IsSettingUp){
    server.handleClient();
 }
 if(!button_pressed && NEW){
 disp2("WATCH ON","STANDBY MODE");
 NEW=false;
 }
 if(digitalRead(PANIC) && !button_pressed){
  delay(200);
  disp2("PANIC BTN PRESSED!!","SEND MESSAGE?(P)");
  Serial.println("you pressed PANIC BUTTON. DO you really want to send SOS?");
led(1,0,0);
while(!digitalRead(PANIC) && !digitalRead(TERMINATE));
if(digitalRead(PANIC))
  panic_mode();
else if(digitalRead(TERMINATE)){
  led(0,0,0);
  NEW=true;
}
 }
 


if(button_pressed && !termination_button){
  if(millis()-time_keeper>delayVal){
    panic_mode();
}
}
if(digitalRead(TERMINATE) && button_pressed){
  terminate_mode();
}
}


