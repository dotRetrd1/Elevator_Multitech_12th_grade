#include <Arduino.h>
#include <Wire.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>

int Devices = 4;

std::map<int, int> floorHeight = {
    {1, 10},
    {2, 20},
    {3, 30}
};

#define DisTRIG 4
#define DisECHO 5
const int buttonPins[] = {1,2,3};
const int coilPins[] = {6,7,8};
std::vector<int> outPins(coilPins, coilPins + sizeof(coilPins) / sizeof(coilPins[0]));

int buttonState[sizeof(buttonPins) / sizeof(buttonPins[0])];


void setup() {
    Serial.begin(115200);

    outPins.push_back(DisTRIG);
    outPins.push_back(DisECHO);
    
    for (int val : outPins) {
        pinMode(val,OUTPUT);
    }
    for (int val : buttonPins) {
        pinMode(val,INPUT_PULLUP);
    }
}

void loop() {

}

//chcek height, reset system etc
void onStart() {

}

//Read and print buttons mainly
void onLoop() {
    String printOut = "";

    for (int button : buttonPins) {
        buttonState[button] = !digitalRead(button);
        
        printOut += "Button: " + String(button) + " State: " + String(buttonState[button]) + " | ";
    }
    Serial.println(printOut);
}
