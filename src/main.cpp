// main.cpp

#include <Arduino.h>
#include <Ethernet.h>
#include "StepperMotor.h"

// Define LED Pins
const int greenLED = 22; // LED for Motor A (green light)
const int redLED = 23;   // LED for Motor B (red light)

// Homed status flags
bool motorA_homed = false;
bool motorB_homed = false;

// Add a flag for homing loop
bool homingLoopActive = false;

// Define Limit Switch Pins
const int LIMIT_SWITCH_A = 3; // Motor A's limit switch, activated when HIGH (at MIN_POS_A)
const int LIMIT_SWITCH_B = 2; // Motor B's limit switch, activated when HIGH (at MAX_POS_B)

// Define Bounds (Adjust based on your setup)
const long MIN_POS_A = 0;
const long MAX_POS_A = 5800;

const long MIN_POS_B = 0;
const long MAX_POS_B = 2000;

// Define Starting Positions
const long START_POS_A = 2716;
const long START_POS_B = 634;

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
    1000       // Step Delay (microseconds)
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
    1000       // Step Delay (microseconds)
);

// Ethernet settings
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 100, 115);
EthernetServer server(80);

void setup() {
    // Initialize Serial Monitor
    Serial.begin(9600);
    Serial.println("Dual Stepper Motor Control Initialized.");

    // Initialize Ethernet
    Ethernet.begin(mac, ip);
    server.begin();
    Serial.print("Server is at ");
    Serial.println(Ethernet.localIP());

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

    // Move Motor A by START_POS_A steps
    motorA.moveSteps(START_POS_A, true); // Assuming true is the forward direction

    motorA_homed = true; // Set homed flag for Motor A
    Serial.println("Motor A homed.");

    // Home MotorB: Move forward (inverted to physically backward) until LIMIT_SWITCH_B is HIGH, then move backward (physically forward)
    motorB.home(false, LIMIT_SWITCH_B, HIGH);
    motorB.setCurrentPosition(MIN_POS_B); // Set position to maximum after homing

    // Move Motor B by START_POS_B steps
    motorB.moveSteps(START_POS_B, true); // Assuming true is the forward direction

    motorB_homed = true; // Set homed flag for Motor B
    Serial.println("Motor B homed.");

    Serial.println("Homing Complete.");
    Serial.println("Ready for commands:");
    Serial.println("Commands:");
    Serial.println("  HOME                     - Home both drivers A and B");
    Serial.println("  HOME_LOOP                - Start/Stop continuous homing loop");
    Serial.println("  MOVE_REL A <steps>       - Move Driver A relative steps");
    Serial.println("  MOVE_REL B <steps>       - Move Driver B relative steps");
    Serial.println("  MOVE_ABS A <position>    - Move Driver A to absolute position");
    Serial.println("  MOVE_ABS B <position>    - Move Driver B to absolute position");
    Serial.println("  GETPOS                   - Get current positions");
}

void handleCommand(String commandLine, EthernetClient& client) {
    commandLine.trim(); // Remove any trailing newline or spaces
    commandLine.toUpperCase(); // Convert the command to uppercase to match the if statements

    Serial.print("Received Command: ");
    Serial.println(commandLine);

    // Split the command into parts
    int firstSpace = commandLine.indexOf(' ');
    String command = commandLine.substring(0, firstSpace);
    String param1 = "";
    String param2 = "";

    if (firstSpace != -1) {
        int secondSpace = commandLine.indexOf(' ', firstSpace + 1);
        if (secondSpace != -1) {
            param1 = commandLine.substring(firstSpace + 1, secondSpace);
            param2 = commandLine.substring(secondSpace + 1);
        } else {
            param1 = commandLine.substring(firstSpace + 1);
        }
    }

    if (command == "HOME") {
        motorA.home(false, LIMIT_SWITCH_A, HIGH);
        motorA.setCurrentPosition(MIN_POS_A);
        motorA_homed = true;
        // Move Motor A by START_POS_A steps
        motorA.moveSteps(START_POS_A, true); // Assuming true is the forward direction

        motorB.home(false, LIMIT_SWITCH_B, HIGH);
        motorB.setCurrentPosition(MIN_POS_B);
        motorB_homed = true;
        // Move Motor B by START_POS_B steps
        motorB.moveSteps(START_POS_B, true); // Assuming true is the forward direction
        client.println("\"success\", \"Homing Complete.\"");
    } 
    else if (command == "HOME_LOOP") {
        if (homingLoopActive) {
            homingLoopActive = false;
            client.println("\"success\", \"Homing loop stopped.\"");
        } else {
            homingLoopActive = true;
            client.println("\"success\", \"Homing loop started.\"");
        }
    }
    else if (command == "MOVE_REL") {
        if (param1 == "A") {
            const char* result = motorA.moveRelative(param2.toInt(), MIN_POS_A, MAX_POS_A);
            if (strcmp(result, "Movement out of bounds") == 0) {
                client.println("\"error\", \"Movement out of bounds.\"");
            } else {
                client.println("\"success\", \"Motor A moved successfully.\"");
            }
        } else if (param1 == "B") {
            const char* result = motorB.moveRelative(param2.toInt(), MIN_POS_B, MAX_POS_B);
            if (strcmp(result, "Movement out of bounds") == 0) {
                client.println("\"error\", \"Movement out of bounds.\"");
            } else {
                client.println("\"success\", \"Motor B moved successfully.\"");
            }
        } else {
            client.println("\"error\", \"Invalid motor identifier.\"");
        }
    } else if (command == "MOVE_ABS") {
        if (param1 == "A") {
            const char* result = motorA.moveTo(param2.toInt(), MIN_POS_A, MAX_POS_A);
            if (strcmp(result, "Movement out of bounds") == 0) {
                client.println("\"error\", \"Movement out of bounds.\"");
            } else {
                client.println("\"success\", \"Motor A moved to position.\"");
            }
        } else if (param1 == "B") {
            const char* result = motorB.moveTo(param2.toInt(), MIN_POS_B, MAX_POS_B);
            if (strcmp(result, "Movement out of bounds") == 0) {
                client.println("\"error\", \"Movement out of bounds.\"");
            } else {
                client.println("\"success\", \"Motor B moved to position.\"");
            }
        } else {
            client.println("\"error\", \"Invalid motor identifier.\"");
        }
    } else if (command == "GETPOS") {
        String posMessage = "Motor A Position: " + String(motorA.getCurrentPosition()) + ", Motor B Position: " + String(motorB.getCurrentPosition());
        client.println("\"success\", \"" + posMessage + "\"");
    } else {
        client.println("\"error\", \"Unknown command.\"");
    }
    client.flush(); // Ensure the response is sent
    client.stop();  // Close the connection after sending the response
}

void loop() {
    // Update LEDs based on homed flags
    digitalWrite(greenLED, motorA_homed ? HIGH : LOW);
    digitalWrite(redLED, motorB_homed ? HIGH : LOW);

    // Handle Serial Commands
    if (Serial.available() > 0) {
        String commandLine = Serial.readStringUntil('\n');
        // Handle serial commands separately if needed
        Serial.print("Received Command: ");
        Serial.println(commandLine);
        // Add code to handle serial commands if needed
    }

    // Handle Ethernet Commands
    EthernetClient client = server.available();
    if (client) {
        String commandLine = "";
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                if (c == '\n') {
                    handleCommand(commandLine, client);
                    commandLine = "";
                } else {
                    commandLine += c;
                }
            }
        }
        client.stop();
    }

    // Handle Homing Loop
    if (homingLoopActive) {
        motorA.home(false, LIMIT_SWITCH_A, HIGH);
        motorA.setCurrentPosition(MIN_POS_A);
        motorA.moveSteps(START_POS_A, true);
        
        motorB.home(false, LIMIT_SWITCH_B, HIGH);
        motorB.setCurrentPosition(MIN_POS_B);
        motorB.moveSteps(START_POS_B, true);
        
        // Short delay to prevent rapid looping
        delay(1000);
    }
}
