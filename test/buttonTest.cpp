#include<Arduino.h>
#define PANIC 34
#define TERMINATE 35
void setup(){
    pinMode(34, INPUT);
    pinMode(35, INPUT);
    Serial.begin(115200);
}
void loop(){
    digitalRead(PANIC)==1 ? Serial.print("panicbutton pressed"):Serial.print("panic button not pressed");
    digitalRead(TERMINATE)==1 ? Serial.println("panicbutton pressed"):Serial.println("panic button not pressed");
}