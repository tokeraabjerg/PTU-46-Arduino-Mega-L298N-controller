// StepperMotor.cpp

#include "StepperMotor.h"

// Constructor
StepperMotor::StepperMotor(int in1, int in2, int in3, int in4, int ena, int enb,
                           int led1, int led2, StepMode mode,
                           unsigned long stepDelay)
    : _in1(in1), _in2(in2), _in3(in3), _in4(in4),
      _enableA(ena), _enableB(enb),
      _led1(led1), _led2(led2),
      _stepMode(mode),
      _stepDelay(stepDelay),
      _currentPos(0),
      _currentStep(0),
      _invertDirection(false) {} // Initialize inversion flag to false

// Initialize the motor pins
void StepperMotor::begin() {
    pinMode(_in1, OUTPUT);
    pinMode(_in2, OUTPUT);
    pinMode(_in3, OUTPUT);
    pinMode(_in4, OUTPUT);
    pinMode(_enableA, OUTPUT);
    pinMode(_enableB, OUTPUT);

    if (_led1 != -1) {
        pinMode(_led1, OUTPUT);
        digitalWrite(_led1, LOW);
    }
    if (_led2 != -1) {
        pinMode(_led2, OUTPUT);
        digitalWrite(_led2, LOW);
    }

    // Enable the motor channels
    digitalWrite(_enableA, HIGH);
    digitalWrite(_enableB, HIGH);

    stop(); // Ensure motor is stopped
}

// Set stepping mode
void StepperMotor::setStepMode(StepMode mode) {
    _stepMode = mode;
}

// Set step delay
void StepperMotor::setStepDelay(unsigned long stepDelay) {
    _stepDelay = stepDelay;
}

// Set direction inversion
void StepperMotor::setInvertDirection(bool invert) {
    _invertDirection = invert;
}

// Set current position
void StepperMotor::setCurrentPosition(long pos) {
    _currentPos = pos;
}

// Execute a single step
void StepperMotor::step(bool forward) {
    // Apply direction inversion
    bool actualForward = _invertDirection ? !forward : forward;

    if (_stepMode == FULL_STEP) {
        // Full-step sequence has 4 steps
        if (actualForward) {
            _currentStep = (_currentStep + 1) % 4;
        } else {
            _currentStep = (_currentStep - 1 + 4) % 4;
        }
        digitalWrite(_in1, fullStepSeq[_currentStep][0]);
        digitalWrite(_in2, fullStepSeq[_currentStep][1]);
        digitalWrite(_in3, fullStepSeq[_currentStep][2]);
        digitalWrite(_in4, fullStepSeq[_currentStep][3]);
    }
    else { // HALF_STEP
        // Half-step sequence has 8 steps
        if (actualForward) {
            _currentStep = (_currentStep + 1) % 8;
        } else {
            _currentStep = (_currentStep - 1 + 8) % 8;
        }
        digitalWrite(_in1, halfStepSeq[_currentStep][0]);
        digitalWrite(_in2, halfStepSeq[_currentStep][1]);
        digitalWrite(_in3, halfStepSeq[_currentStep][2]);
        digitalWrite(_in4, halfStepSeq[_currentStep][3]);
    }
}

// Move a specific number of steps in a direction
void StepperMotor::moveSteps(long steps, bool forward) {
    if (_led1 != -1 && _led2 != -1) {
        digitalWrite(_led1, HIGH);
        digitalWrite(_led2, HIGH);
    }

    for (long i = 0; i < abs(steps); i++) {
        step(forward);
        _currentPos += (forward) ? 1 : -1;
        delayMicroseconds(_stepDelay);
    }

    stop();

    if (_led1 != -1 && _led2 != -1) {
        digitalWrite(_led1, LOW);
        digitalWrite(_led2, LOW);
    }
}

// Home the motor by moving in the specified direction until limit switch is activated,
// then move in the opposite direction to release the switch
void StepperMotor::home(bool forward, int limitSwitchPin, bool activeState) {
    if (_led1 != -1 && _led2 != -1) {
        digitalWrite(_led1, HIGH);
        digitalWrite(_led2, HIGH);
    }

    Serial.print("Homing Motor on Limit Switch Pin ");
    Serial.print(limitSwitchPin);
    Serial.println("...");

    // Move in the specified direction until the limit switch is activated
    Serial.print("Moving ");
    Serial.print(forward ? "Forward" : "Backward");
    Serial.println(" to Limit Switch.");
    while (digitalRead(limitSwitchPin) != activeState) {
        step(forward);
        _currentPos += (forward) ? 1 : -1;
        delayMicroseconds(_stepDelay);
    }

    Serial.println("Limit Switch Activated. Stopping.");
    stop();

    // Move a fixed number of steps in the opposite direction to release the limit switch
    const long releaseSteps = 50; // Adjust as needed
    Serial.print("Releasing Limit Switch by ");
    Serial.print(releaseSteps);
    Serial.println(" steps.");

    moveSteps(releaseSteps, !forward);

    // Set the current position based on the homing end
    // This should be handled externally after homing to ensure accuracy

    Serial.println("Homing Complete.");
}

// Move a specific number of steps relative to current position with bounds checking
void StepperMotor::moveRelative(long steps, long minPos, long maxPos) {
    if (steps == 0) return;

    bool forward = (steps > 0);
    long absSteps = abs(steps);

    // Calculate potential new position
    long potentialPos = _currentPos + (forward ? absSteps : -absSteps);

    // Check bounds
    if (potentialPos < minPos || potentialPos > maxPos) {
        Serial.println("Movement out of bounds. Operation aborted.");
        return;
    }

    Serial.print("Moving ");
    Serial.print(forward ? "Forward" : "Backward");
    Serial.print(" by ");
    Serial.print(absSteps);
    Serial.println(" steps.");

    moveSteps(absSteps, forward);
}

// Move to an absolute position with bounds checking
void StepperMotor::moveTo(long position, long minPos, long maxPos) {
    // Ensure the target position is within bounds
    if (position < minPos || position > maxPos) {
        Serial.println("Target position out of bounds. Operation aborted.");
        return;
    }

    long steps = position - _currentPos;

    if (steps == 0) {
        Serial.println("Already at the desired position.");
        return;
    }

    bool forward = (steps > 0);
    long absSteps = abs(steps);

    Serial.print("Moving ");
    Serial.print(forward ? "Forward" : "Backward");
    Serial.print(" to position ");
    Serial.println(position);

    moveSteps(absSteps, forward);
}

// Stop the motor by deactivating all coils
void StepperMotor::stop() {
    digitalWrite(_in1, LOW);
    digitalWrite(_in2, LOW);
    digitalWrite(_in3, LOW);
    digitalWrite(_in4, LOW);
}

// Get current position
long StepperMotor::getCurrentPosition() {
    return _currentPos;
}
