#include<Arduino.h>
#include<HardwareSerial.h>
#include <TinyGPS++.h>
#include<strings.h>

#define PANIC 25
#define TERMINATE 13
#define INTERVAL 20000
#define RED 21
#define GREEN 19
#define BLUE 18
HardwareSerial M65(1);
#define TXM65_PIN 15  // ESP32 TX pin connected to M65 RX
#define RXM65_PIN -1  // ESP32 RX pin connected to M65 TX

HardwareSerial GPS(2); // UART2 for Neo6M GPS Module
TinyGPSPlus gps;        // Create TinyGPS++ object
#define GPS_TX_PIN 17  // ESP32 TX pin connected to GPS RX
#define GPS_RX_PIN 14 // ESP32 RX pin connected to GPS TX



bool button_pressed = false;
bool termination_button =false;
long int time_keeper;
int count=0;
char buff[10]="";
char message[50];
char TIME[15]="";
char lk[50]="https://www.google.com/maps?q=";



void led(int red, int green ,int blue){
  digitalWrite(RED,red);
  digitalWrite(GREEN,green);
  digitalWrite(BLUE,blue);
}


void sendATCommand(const char* command) {
  Serial.println(command);
    M65.println(command);
    delay(1000);
    // while (M65.available()) {
    //   Serial.print("sent command : "); // Display response from M65
    //  
    // }
  
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
  

    char* int_to_char (int num){
    itoa(num,buff,10);
    return buff;
    }



  void Create_message(){
    while(GPS.available()>0){
    char c = GPS.read();
    gps.encode(c);
  }
  delay(500);
  dtostrf(gps.location.lat(),0,5,buff);
  Serial.print(buff);
  strcat(lk,buff);
  strcat(lk,",");
  dtostrf(gps.location.lng(),0,5,buff);
  Serial.println(buff);
  strcat(lk,buff);
  strcat(TIME,int_to_char(gps.date.year()));
  strcat(TIME,":");
  strcat(TIME,int_to_char(gps.date.month()));
  strcat(TIME,":");
  strcat(TIME,int_to_char(gps.date.day()));
  strcpy(message,"the date is ");
  strcat(message,TIME);
  strcpy(TIME,"");
  strcat(TIME,int_to_char(gps.time.hour()));
  strcat(TIME,":");
  strcat(TIME,int_to_char(gps.time.minute()));
  strcat(TIME,":");
  strcat(TIME,int_to_char(gps.time.second()));
  strcat(message," and the time is ");
  strcat(message,TIME);
  strcpy(TIME,"");
  }




void Send_message(char MESSAGE[]){
  delay(1000);
  sendATCommand("AT");
  delay(1000);

  // Set SMS text mode
  sendATCommand("AT+CMGF=1");
  delay(1000);

  // Set the recipient's phone number (replace with actual number)
  sendATCommand("AT+CMGS=\"+9779806512990\"");
  delay(1000);

  // Message content
  M65.print(MESSAGE);
  delay(500);

  // End message with Ctrl+Z (ASCII 26)
  M65.write(26);
  delay(3000);

  Serial.println("Message Sent!");
  led(1,1,1);
  delay(100);
  led(0,0,0);
  delay(100);
  led(1,1,1);
  delay(100);
  led(0,0,1);
}



void panic_mode(){
  delay(100);
  led(0,0,1);
  time_keeper=millis();
  count++;
  button_pressed=true;
  termination_button=false;
  Create_message();
  Serial.println(message);
  Serial.println(lk);
  Send_message(lk);
  strcpy(lk,"https://www.google.com/maps?q=");
  // ReadGPS();
  delay(100);
  }
  


  void terminate_mode(){
    if(button_pressed){
    delay(100);
    led(0,1,0);
    button_pressed=false;
    termination_button=true;
    count=0;
    strcpy(message,"termination button pressed.");
     Send_message(message);
    Serial.println("panic mode deactivated");
    led(0,0,0);
    delay(100);
    }
    else{
    led(1,1,1);
    delay(500);
    led(0,0,0);
    delay(500);
    led(1,1,1);
    delay(500);
    led(0,0,0);
    delay(500);
  }
  }



void setup(){
    pinMode(PANIC, INPUT);
    pinMode(TERMINATE,INPUT);
    pinMode(RED, OUTPUT);
    pinMode(GREEN,OUTPUT);
    pinMode(BLUE, OUTPUT);
    Serial.begin(115200); // For debugging
    led(0,0,1);
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
    led(0,0,0);
}




void loop(){
 if(digitalRead(PANIC) && !button_pressed){
  delay(200);
  Serial.println("you pressed PANIC BUTTON. DO you really want to send SOS?");
led(1,0,0);
  while(!digitalRead(PANIC)){
  }
 panic_mode();
 }
 


if(button_pressed && !termination_button){
  if(millis()-time_keeper>INTERVAL){
    panic_mode();
}
}
if(digitalRead(TERMINATE)){
  terminate_mode();
}
}


