#include <LiquidCrystal595.h>
#include <Stepper.h>
#include "SR04.h"

// ======================
// LCD CONFIGURATION
// ======================

const int DATA_PIN  = 6;
const int CLOCK_PIN = 7;
const int LATCH_PIN = 8;

const int BACKLIGHT_PIN = 9;

LiquidCrystal595 lcd(DATA_PIN, LATCH_PIN, CLOCK_PIN);

// ======================
// SONAR CONFIGURATION
// ======================

#define TRIG_PIN 10
#define ECHO_PIN 11

SR04 sonar = SR04(ECHO_PIN, TRIG_PIN);

// ======================
// STEPPER CONFIGURATION
// ======================

const int IN1 = 2;
const int IN2 = 3;
const int IN3 = 4;
const int IN4 = 5;

// 28BYJ-48 stepper motor
const int STEPS_PER_REVOLUTION = 2048;

// Scan step angle
const float SCAN_STEP_ANGLE = 5.0;

// Convert angle to motor steps
const int STEPS_PER_SCAN =
    (SCAN_STEP_ANGLE * STEPS_PER_REVOLUTION) / 360.0;

Stepper stepperMotor(
    STEPS_PER_REVOLUTION,
    IN1,
    IN3,
    IN2,
    IN4
);

// ======================
// BUTTONS
// ======================

const int START_BUTTON = A2;
const int STOP_BUTTON  = A0;

// ======================
// STATES
// ======================

enum State
{
    STOPPED,
    SCANNING,
    RESETING,
    IDLE
};

State currentState = STOPPED;

// ======================
// GLOBAL VARIABLES
// ======================

// Current angle
float currentAngle = 0.0;

// Sonar distance
long currentRange = 0;

// Current motor position in steps
long currentStep = 0;

// Scan direction
//  1 = clockwise
// -1 = counter-clockwise
int scanDirection = 1;

// Button memory
bool lastStartPressed = false;
bool lastStopPressed  = false;

// ======================
// SETUP
// ======================

void setup()
{
    // LCD backlight
    pinMode(BACKLIGHT_PIN, OUTPUT);
    analogWrite(BACKLIGHT_PIN, 100);

    // LCD init
    lcd.begin(16, 2);
    lcd.clear();

    // Buttons
    pinMode(START_BUTTON, INPUT_PULLUP);
    pinMode(STOP_BUTTON, INPUT_PULLUP);

    // Stepper speed
    stepperMotor.setSpeed(10);

    // Serial debug
    Serial.begin(9600);
    Serial.println("angle,distance");

    updateLCD();
}

// ======================
// MAIN LOOP
// ======================

void loop()
{
    // ======================
    // BUTTON READING
    // ======================


    bool startPressed = !digitalRead(START_BUTTON);
    bool stopPressed  = !digitalRead(STOP_BUTTON);

    bool startEdge =
        startPressed && !lastStartPressed;

    bool stopEdge =
        stopPressed && !lastStopPressed;

    // ======================
    // STATE MACHINE
    // ======================

    switch(currentState)
    {
        // ======================
        // STOPPED
        // ======================

        case STOPPED:

            if(startEdge)
            {
                changeState(SCANNING);
            }
            else if(stopEdge)
            {
                changeState(RESETING);
            }

            break;

        // ======================
        // SCANNING
        // ======================

        case SCANNING:

            // Stop request
            if(stopEdge)
            {
                changeState(STOPPED);
                break;
            }

            // ======================
            // SONAR MEASUREMENT
            // ======================

            currentRange = sonar.Distance();

            // ======================
            // LCD UPDATE
            // ======================

            updateLCD();

            // ======================
            // SCAN LIMITS
            // ======================

            // +180°
            if(currentAngle >= 180.0)
            {
                scanDirection = -1;
            }

            // -180°
            if(currentAngle <= -180.0)
            {
                scanDirection = 1;
            }

            // ======================
            // MOTOR ROTATION
            // ======================

            int motorSteps =
                scanDirection * STEPS_PER_SCAN;

            stepperMotor.step(motorSteps);

            currentStep += motorSteps;

            // ======================
            // ANGLE COMPUTATION
            // ======================

            currentAngle =
                (currentStep * 360.0)
                / STEPS_PER_REVOLUTION;

            // Debug
            
            Serial.print(currentAngle);
            Serial.print(",");
            Serial.println(currentRange);

            break;

        // ======================
        // RESETING
        // ======================

case RESETING:

    // ======================
    // RETURN TO 0°
    // ======================

    if(currentStep > 0)
    {
        // Move toward negative direction

        stepperMotor.step(-1);

        currentStep--;
    }
    else if(currentStep < 0)
    {
        // Move toward positive direction

        stepperMotor.step(1);

        currentStep++;
    }

    // ======================
    // UPDATE ANGLE
    // ======================

    currentAngle =
        (currentStep * 360.0)
        / STEPS_PER_REVOLUTION;

    updateLCD();

    // ======================
    // HOME POSITION REACHED
    // ======================

    if(currentStep == 0)
    {
        currentAngle = 0;

        scanDirection = 1;

        changeState(IDLE);
    }

    // ======================
    // IMPORTANT:
    // Give Stepper library enough
    // time to generate phases
    // ======================

    delay(50);

    break;

        // ======================
        // IDLE
        // ======================

        case IDLE:

            if(startEdge)
            {
                changeState(SCANNING);
            }

            break;
    }

    // ======================
    // SAVE BUTTON STATES
    // ======================

    lastStartPressed = startPressed;
    lastStopPressed  = stopPressed;
}

// ======================
// CHANGE STATE
// ======================

void changeState(State newState)
{
    currentState = newState;

    updateLCD();
}

// ======================
// LCD UPDATE
// ======================

void updateLCD()
{
    lcd.setCursor(0,0);
    lcd.print("                ");

    lcd.setCursor(0,1);
    lcd.print("                ");

    // First line

    lcd.setCursor(0, 0);
    lcd.print("STATE:");

    switch(currentState)
    {
        case STOPPED:
            lcd.print("STOPPED");
            break;

        case SCANNING:
            lcd.print("SCANNING");
            break;

        case RESETING:
            lcd.print("RESETING");
            break;

        case IDLE:
            lcd.print("IDLE");
            break;
    }

    // Second line

    if(currentState == SCANNING)
    {
        lcd.setCursor(0, 1);

        lcd.print("A:");
        lcd.print((int)currentAngle);

        lcd.print(" R:");
        lcd.print(currentRange);
    }
}