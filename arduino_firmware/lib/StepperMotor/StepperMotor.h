// StepperMotor.h

#ifndef STEPPERMOTOR_H
#define STEPPERMOTOR_H

#include <Arduino.h>

// Define stepping modes
enum StepMode {
    FULL_STEP,
    HALF_STEP
};

class StepperMotor {
public:
    /**
     * @brief Constructor for StepperMotor class.
     * 
     * @param in1      Arduino pin connected to IN1 of the driver.
     * @param in2      Arduino pin connected to IN2 of the driver.
     * @param in3      Arduino pin connected to IN3 of the driver.
     * @param in4      Arduino pin connected to IN4 of the driver.
     * @param ena      Arduino pin connected to ENA of the driver.
     * @param enb      Arduino pin connected to ENB of the driver.
     * @param led1     Arduino pin connected to LED1 (optional, default -1).
     * @param led2     Arduino pin connected to LED2 (optional, default -1).
     * @param mode     Stepping mode (FULL_STEP or HALF_STEP).
     * @param stepDelay Delay between steps in microseconds.
     */
    StepperMotor(int in1, int in2, int in3, int in4, int ena, int enb,
                int led1 = -1, int led2 = -1, StepMode mode = HALF_STEP,
                unsigned long stepDelay = 5000);

    /**
     * @brief Initialize the motor pins and enable the driver.
     */
    void begin();

    /**
     * @brief Move a specific number of steps in a given direction.
     * 
     * @param steps    Number of steps to move.
     * @param forward  Direction of movement (true for forward, false for backward).
     */
    void moveSteps(long steps, bool forward);

    /**
     * @brief Home the motor by moving in the specified direction until the limit switch is activated,
     *        then move in the opposite direction to release the switch.
     * 
     * @param forward        Initial direction to move during homing.
     * @param limitSwitchPin Arduino pin connected to the limit switch.
     * @param activeState    Logic level that indicates the limit switch is activated.
     */
    void home(bool forward, int limitSwitchPin, bool activeState);

    /**
     * @brief Move the motor a relative number of steps with bounds checking.
     * 
     * @param steps    Number of steps to move relative to current position.
     * @param minPos   Minimum allowable position in steps.
     * @param maxPos   Maximum allowable position in steps.
     * @return const char*  Message indicating the result of the operation.
     */
    const char* moveRelative(long steps, long minPos, long maxPos);

    /**
     * @brief Move the motor to an absolute position with bounds checking.
     * 
     * @param position Desired absolute position in steps.
     * @param minPos   Minimum allowable position in steps.
     * @param maxPos   Maximum allowable position in steps.
     * @return const char*  Message indicating the result of the operation.
     */
    const char* moveTo(long position, long minPos, long maxPos);

    /**
     * @brief Stop the motor by deactivating all coils.
     */
    void stop();

    /**
     * @brief Set the stepping mode of the motor.
     * 
     * @param mode Stepping mode (FULL_STEP or HALF_STEP).
     */
    void setStepMode(StepMode mode);

    /**
     * @brief Set the delay between steps.
     * 
     * @param stepDelay Delay in microseconds.
     */
    void setStepDelay(unsigned long stepDelay);

    /**
     * @brief Get the current position of the motor.
     * 
     * @return Current position in steps.
     */
    long getCurrentPosition();

    /**
     * @brief Set the direction inversion for the motor.
     * 
     * @param invert If true, invert the stepping direction.
     */
    void setInvertDirection(bool invert);

    /**
     * @brief Set the current position of the motor.
     * 
     * @param pos The position to set.
     */
    void setCurrentPosition(long pos);

private:
    // Control pins
    int _in1, _in2, _in3, _in4;
    int _enableA, _enableB;
    int _led1, _led2;

    // Stepping mode
    StepMode _stepMode;

    // Step delay in microseconds
    unsigned long _stepDelay;

    // Step sequences for full-step and half-step modes
    const int fullStepSeq[4][4] = {
        {1, 0, 1, 0},
        {0, 1, 1, 0},
        {0, 1, 0, 1},
        {1, 0, 0, 1}
    };

    const int halfStepSeq[8][4] = {
        {1, 0, 1, 0}, // Step 1
        {1, 0, 1, 1}, // Step 2
        {1, 0, 0, 1}, // Step 3
        {0, 0, 0, 1}, // Step 4
        {0, 1, 0, 1}, // Step 5
        {0, 1, 1, 1}, // Step 6
        {0, 1, 1, 0}, // Step 7
        {0, 0, 1, 0}  // Step 8
    };

    // Current step position
    long _currentPos;

    // Current step in sequence
    int _currentStep;

    // Direction inversion flag
    bool _invertDirection;

    /**
     * @brief Execute a single step in the specified direction.
     * 
     * @param forward Direction to step (true for forward, false for backward).
     */
    void step(bool forward);
};

#endif
