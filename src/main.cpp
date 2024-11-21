// main.cpp

#include <Arduino.h>
#include "StepperMotor.h"

// Define LED Pins
const int greenLED = 22; // LED for Motor A (green light)
const int redLED = 23;   // LED for Motor B (red light)

// Homed status flags
bool motorA_homed = false;
bool motorB_homed = false;

// Define Limit Switch Pins
const int LIMIT_SWITCH_A = 3; // Motor A's limit switch, activated when HIGH (at MIN_POS_A)
const int LIMIT_SWITCH_B = 2; // Motor B's limit switch, activated when HIGH (at MAX_POS_B)

// Define Bounds (Adjust based on your setup)
const long MIN_POS_A = 0;
const long MAX_POS_A = 5000;

const long MIN_POS_B = 0;
const long MAX_POS_B = 2000;

// Define Stepper Motors
// MotorA (Driver A)
StepperMotor motorA(
    8,   // IN1
    9,   // IN2
    10,  // IN3
    11,  // IN4
    48,  // ENA
    50,  // ENB
    7,   // LED1 (Optional)
    6,   // LED2 (Optional)
    HALF_STEP, // Stepping Mode
    5000       // Step Delay (microseconds)
);

// MotorB (Driver B)
StepperMotor motorB(
    14,  // IN1
    15,  // IN2
    16,  // IN3
    17,  // IN4
    40,  // ENA
    42,  // ENB
    5,   // LED1 (Optional)
    4,   // LED2 (Optional)
    HALF_STEP, // Stepping Mode
    5000       // Step Delay (microseconds)
);

void setup() {
    // Initialize Serial Monitor
    Serial.begin(9600);
    Serial.println("Dual Stepper Motor Control Initialized.");

    // Initialize LED Pins
    pinMode(greenLED, OUTPUT);
    pinMode(redLED, OUTPUT);
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, LOW);

    // Initialize Motors
    motorA.begin();
    motorB.begin();

    // Invert direction for Motor B
    motorB.setInvertDirection(true); // Hardcode inversion for Motor B
    motorA.setInvertDirection(true); // Hardcode inversion for Motor B


    // Start Homing Sequence
    Serial.println("Starting Homing Sequence...");

    // Home MotorA: Move forward until LIMIT_SWITCH_A is HIGH, then move backward
    motorA.home(false, LIMIT_SWITCH_A, HIGH);
    motorA.setCurrentPosition(MIN_POS_A); // Set position to minimum after homing
    motorA_homed = true; // Set homed flag for Motor A
    Serial.println("Motor A homed.");

    // Home MotorB: Move forward (inverted to physically backward) until LIMIT_SWITCH_B is HIGH, then move backward (physically forward)
    motorB.home(false, LIMIT_SWITCH_B, HIGH);
    motorB.setCurrentPosition(MIN_POS_B); // Set position to maximum after homing
    motorB_homed = true; // Set homed flag for Motor B
    Serial.println("Motor B homed.");

    Serial.println("Homing Complete.");
    Serial.println("Ready for commands:");
    Serial.println("Commands:");
    Serial.println("  HOME                     - Home both drivers A and B");
    Serial.println("  MOVE_REL A <steps>       - Move Driver A relative steps");
    Serial.println("  MOVE_REL B <steps>       - Move Driver B relative steps");
    Serial.println("  MOVE_ABS A <position>    - Move Driver A to absolute position");
    Serial.println("  MOVE_ABS B <position>    - Move Driver B to absolute position");
    Serial.println("  GETPOS                   - Get current positions");
}

void loop() {
    // Update LEDs based on homed flags
    digitalWrite(greenLED, motorA_homed ? HIGH : LOW);
    digitalWrite(redLED, motorB_homed ? HIGH : LOW);

    // Handle Serial Commands
    if (Serial.available() > 0) {
        String commandLine = Serial.readStringUntil('\n');
        commandLine.trim(); // Remove any trailing newline or spaces

        Serial.print("Received Command: ");
        Serial.println(commandLine);

        // Split the command into parts
        int firstSpace = commandLine.indexOf(' ');
        String command = "";
        String param1 = "";
        String param2 = "";

        if (firstSpace != -1) {
            command = commandLine.substring(0, firstSpace);
            String params = commandLine.substring(firstSpace + 1);
            params.trim();
            int secondSpace = params.indexOf(' ');
            if (secondSpace != -1) {
                param1 = params.substring(0, secondSpace);
                param2 = params.substring(secondSpace + 1);
            }
            else {
                param1 = params;
            }
        }
        else {
            command = commandLine;
        }

        // Convert command to uppercase for case-insensitive comparison
        command.toUpperCase();

        // Handle commands
        if (command == "HOME") {
            Serial.println("Homing both drivers A and B...");
            motorA.home(false, LIMIT_SWITCH_A, HIGH);
            motorA.setCurrentPosition(MIN_POS_A); // Set position to minimum after homing
            motorA_homed = true; // Update homed flag
            Serial.println("Motor A homed.");
            motorB.home(false, LIMIT_SWITCH_B, HIGH);
            motorB.setCurrentPosition(MIN_POS_B); // Set position to maximum after homing
            motorB_homed = true; // Update homed flag
            Serial.println("Motor B homed.");
            Serial.println("Homing Complete.");
        }
        else if (command == "MOVE_REL") {
            param1.toUpperCase();
            long steps = param2.toInt();
            if (param1 == "A") {
                motorA.moveRelative(steps, MIN_POS_A, MAX_POS_A);
            }
            else if (param1 == "B") {
                motorB.moveRelative(steps, MIN_POS_B, MAX_POS_B);
            }
            else {
                Serial.println("Invalid driver. Use A or B.");
            }
        }
        else if (command == "MOVE_ABS") {
            param1.toUpperCase();
            long position = param2.toInt();
            if (param1 == "A") {
                motorA.moveTo(position, MIN_POS_A, MAX_POS_A);
            }
            else if (param1 == "B") {
                motorB.moveTo(position, MIN_POS_B, MAX_POS_B);
            }
            else {
                Serial.println("Invalid driver. Use A or B.");
            }
        }
        else if (command == "GETPOS") {
            long posA = motorA.getCurrentPosition();
            long posB = motorB.getCurrentPosition();
            Serial.print("Current Position - A: ");
            Serial.print(posA);
            Serial.print(" steps, B: ");
            Serial.print(posB);
            Serial.println(" steps.");
        }
        else {
            Serial.println("Unknown Command.");
            Serial.println("Available Commands:");
            Serial.println("  HOME                     - Home both drivers A and B");
            Serial.println("  MOVE_REL A <steps>       - Move Driver A relative steps");
            Serial.println("  MOVE_REL B <steps>       - Move Driver B relative steps");
            Serial.println("  MOVE_ABS A <position>    - Move Driver A to absolute position");
            Serial.println("  MOVE_ABS B <position>    - Move Driver B to absolute position");
            Serial.println("  GETPOS                   - Get current positions");
        }

        Serial.println("Ready for next command.");
    }
}
