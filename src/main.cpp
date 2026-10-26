#include <Arduino.h>
#include <Wire.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>

//pins
#define DisTRIG 23
#define DisECHO 14

//buttons mapped to Floor 1 through 3
const int buttonPins[3] = {19, 17, 16};

//stepper pins
const int coilPins[4] = {18, 5, 13, 12};

//floor heights in cm
const float floorHeight[4] = {0.0, 10.0, 20.0, 30.0};
const float TOLERANCE = 2;
const int stepDelay = 2000;

int currentTargetFloor = 1; 
bool lastButtonState[3] = {HIGH, HIGH, HIGH};
unsigned long lastButtonPress = 0;

const int stepSequence[4][4] = {
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
};
int currentStep = 0;

unsigned long lastSensorRead = 0;
float lastError = 0.0;

float getDist();
void stepMotor(int direction);
void stopMotor();
void checkButtons();
void updateClosedLoopControl();

void setup() {
    Serial.begin(115200);

    //ultrasonic sensor
    pinMode(DisTRIG, OUTPUT);
    pinMode(DisECHO, INPUT); // ECHO pin MUST be set as INPUT

    //buttons
    for (int i = 0; i < 3; i++) {
        pinMode(buttonPins[i], INPUT_PULLUP);
    }

    //stepper
    for (int i = 0; i < 4; i++) {
        pinMode(coilPins[i], OUTPUT);
        digitalWrite(coilPins[i], LOW);
    }

    Serial.println("onstart");
}

void loop() {
    checkButtons();
    updateClosedLoopControl();
}

float getDist() {
    digitalWrite(DisTRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(DisTRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(DisTRIG, LOW);

    //10ms timeout
    long duration = pulseIn(DisECHO, HIGH, 10000);
    if (duration == 0) return -1.0;
    //distance in cm
    return (duration * 0.0343) / 2.0;
}

//up = +1, down = -1
void stepMotor(int direction) {
    if (direction == 1) {
        currentStep = (currentStep + 1) % 4;
    } else if (direction == -1) {
        currentStep = (currentStep - 1 + 4) % 4;
    }

    for (int pin = 0; pin < 4; pin++) {
        digitalWrite(coilPins[pin], stepSequence[currentStep][pin]);
    }
}
void stopMotor() {
    for (int pin = 0; pin < 4; pin++) {
        digitalWrite(coilPins[pin], LOW);
    }
}

void checkButtons() {
    for (int i = 0; i < 3; i++) {
        bool reading = !digitalRead(buttonPins[i]);

        if (lastButtonState[i] == HIGH && reading == LOW) {
            if (millis() - lastButtonPress > 200) { 
                currentTargetFloor = i + 1;
                Serial.print("\n !! Target set to Floor ");
                Serial.print(currentTargetFloor);
                Serial.println(" !! \n");
                lastButtonPress = millis();
            }
        }
        lastButtonState[i] = reading;
    }
}


// the בקרה סגורה part 
void updateClosedLoopControl() {
    // read every 50ms
    if (millis() - lastSensorRead >= 50) {
        float currentHeight = getDist();
        
        if (currentHeight >= 0) { //error was valid
            float targetHeight = floorHeight[currentTargetFloor];
            lastError = targetHeight - currentHeight;
        }
        lastSensorRead = millis();
    }

    //printout
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 500) {
        Serial.print("Target: Floor ");
        Serial.print(currentTargetFloor);
        Serial.print(" | Error: ");
        Serial.print(lastError);
        Serial.print(" | Distance: ");
        Serial.println(getDist());
        lastPrint = millis();
    }

    //react to error
    if (abs(lastError) <= TOLERANCE) {
        stopMotor(); // good enough
    } else if (lastError > 0) {
        stepMotor(1); // move up
        delayMicroseconds(stepDelay);
    } else {
        stepMotor(-1); // move down
        delayMicroseconds(stepDelay);
    }
}
