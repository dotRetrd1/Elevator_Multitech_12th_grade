#include <Arduino.h>
#include <Wire.h>
#include <iostream>
#include <string>

global int Devices = 4;

std::map<std::int, int> floorHeight;
floorHeight[1] = 10;
floorHeight[2] = 20;
floorHeight[3] = 30;

#define button1Pin 1
#define button2Pin 2
#define button3Pin 3
#define DisTRIG 4
#define DisECHO 5

std::map<std::int, int> coilPins;
for (int i=0;i<4;i++) {
    coilPins[i+1] = 6+i;
}


void setup() {
    Serial.begin(115200);
    
    pinMode(Pin[1], OUTPUT)
    for (int i=0;i<4;i++) {
        pinMode(std::string ("Coil" + std::to_string(i)), coilPins[i+1]);
    }
}

void loop() {

}