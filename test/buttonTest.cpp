#include<Arduino.h>
void setup(){
    pinMode(34, INPUT);
    pinMode(35, INPUT);
    Serial.begin(115200);
}
void loop(){
    Serial.print(digitalRead(34));
    Serial.print("   ");
    Serial.println(digitalRead(35));
}