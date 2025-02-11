#include<Arduino.h>
void setup(){
    pinMode(25, INPUT);
    pinMode(13, INPUT);
    Serial.begin(115200);
}
void loop(){
    Serial.print(digitalRead(25));
    Serial.print("   ");
    Serial.println(digitalRead(13));
}