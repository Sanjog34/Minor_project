#include<Arduino.h>
void setup(){
    pinMode(5, INPUT);
    pinMode(23, INPUT);
    Serial.begin(115200);
}
void loop(){
    Serial.print(digitalRead(5));
    Serial.print("   ");
    Serial.println(digitalRead(23));
}