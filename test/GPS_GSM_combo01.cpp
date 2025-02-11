#include<Arduino.h>
#include<HardwareSerial.h>
#include <TinyGPS++.h>
#include<strings.h>

#define PANIC 25
#define TERMINATE 13

HardwareSerial M65(1);
#define TXM65_PIN 15  // ESP32 TX pin connected to M65 RX
#define RXM65_PIN 4  // ESP32 RX pin connected to M65 TX

HardwareSerial GPS(2); // UART2 for Neo6M GPS Module
TinyGPSPlus gps;        // Create TinyGPS++ object

#define GPS_TX_PIN 17  // ESP32 TX pin connected to GPS RX
#define GPS_RX_PIN 14 // ESP32 RX pin connected to GPS TX



bool button_pressed = false;
bool termination_button =false;
long int time_keeper;
int count=0;
char message[50];
char TIME[15]="";
char link[50]="https://www.google.com/maps?q=";




void sendATCommand(const char* command) {
    M65.println(command);
    delay(500);
    while (M65.available()) {
      Serial.print("sent command : "); // Display response from M65
      Serial.println(command);
    }
  }



  void ReadGPS(){
    while(GPS.available()>0){
      char c = GPS.read();
      gps.encode(c);
    }
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
  



  void Create_message(){
    while(GPS.available()>0){
    char c = GPS.read();
    gps.encode(c);
  }
  char buff[10];
  
  dtostrf(gps.location.lat(),8,5,buff);
  strcat(link,buff);
  strcat(link,",");
  dtostrf(gps.location.lng(),8,5,buff);
  strcat(link,buff);
//     char buff[5];
//     itoa(gps.date.year(), buff, 10);
//     strcat(TIME,buff);
//     strcat(TIME,":");
//     itoa(gps.date.month(), buff, 10);
//     strcat(TIME,buff);
//     strcat(TIME,":");
//     itoa(gps.date.day(), buff, 10);
//     strcat(TIME,buff);
// strcpy(message,"the date is ");
// strcat(message,TIME);
//  strcpy(TIME,"");
//  itoa(gps.time.hour(), buff, 10);
//  strcat(TIME,buff);
//  strcat(TIME,":");
//  itoa(gps.time.minute(), buff, 10);
//  strcat(TIME,buff);
//  strcat(TIME,":");
//  itoa(gps.time.second(), buff, 10);
//  strcat(TIME,buff);
// strcat(message," and the time is ");
// strcat(message,TIME);
// strcpy(TIME,"");


  }




void Send_message(char *MESSAGE){
  delay(1000);
  sendATCommand("AT");
  delay(1000);

  // Set SMS text mode
  sendATCommand("AT+CMGF=1");
  delay(1000);

  // Set the recipient's phone number (replace with actual number)
  sendATCommand("AT+CMGS=\"+9779846953741\"");
  delay(1000);

  // Message content
  M65.print(MESSAGE);
  delay(500);

  // End message with Ctrl+Z (ASCII 26)
  M65.write(26);
  delay(3000);

  Serial.println("Message Sent!");
}



void panic_mode(){
  delay(100);
  time_keeper=millis();
  count++;
  button_pressed=true;
  termination_button=false;
  Create_message();
Serial.println(link);
  //  Send_message(link);
  delay(100);
  }
  


  void terminate_mode(){
    delay(100);
    button_pressed=false;
    termination_button=true;
    count=0;
    Send_message("termination button pressed.");
    Serial.println("panic mode deactivated");
    delay(100);

  }



void setup(){
    pinMode(PANIC, INPUT);
    pinMode(TERMINATE,INPUT);

    Serial.begin(115200); // For debugging
    Serial.println("Initializing M65 Module..."); 
    M65.begin(9600, SERIAL_8N1, RXM65_PIN, TXM65_PIN); // Initialize UART1 with 9600 baud rate
    delay(3000); // Wait for the module to initialize
    Serial.println("Initializing M65 Module..."); 
    delay(1000);
    sendATCommand("AT");
    delay(1000);

    GPS.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN); // Initialize UART2 for GPS
    delay(2000); // Wait for GPS module to initialize
    Serial.println("Initializing Neo6M GPS Module...");
}




void loop(){
 if(digitalRead(PANIC)){
  delay(200);
  Serial.println("you pressed PANIC BUTTON. DO you really want to send SOS?");
  while(!digitalRead(PANIC)){
  }
  Serial.println("jump1");
 panic_mode();
 }
 


if(button_pressed && !termination_button){
  if(millis()-time_keeper>20000){
    panic_mode();
}
}
if(digitalRead(TERMINATE)){
  terminate_mode();
}
}
