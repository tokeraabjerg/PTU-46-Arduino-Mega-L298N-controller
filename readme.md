# Dual Stepper Motor Control with Arduino

## Overview
This project enables precise control of two stepper motors (Motor A and Motor B) using an Arduino Mega. It features homing sequences, relative and absolute movements, direction inversion for Motor B, and limit switch integration to ensure accurate positioning.

## Features
- **Dual Motor Control:** Independently control two stepper motors.
- **Homing Sequence:** Automatically home each motor to establish a reference position.
- **Relative & Absolute Movements:** Move motors by a specified number of steps or to an exact position within defined bounds.
- **Direction Inversion:** Easily invert Motor B’s direction to accommodate wiring configurations.
- **Limit Switch Integration:** Utilize limit switches to prevent over-travel and ensure safety.
- **LED Indicators:** Visual feedback using LEDs to indicate homed status.

## Hardware Requirements
- **Arduino Mega 2560**
- **2 Stepper Motors**
- **2 Stepper Motor Drivers**
- **2 Limit Switches**
- **4 LEDs** (optional for status indicators)
- **Connecting Wires**
- **Power Supply** suitable for motors

## Software Structure
- **`main.cpp`**: Handles initialization, homing, and serial command processing.
- **`StepperMotor.h` & `StepperMotor.cpp`**: Defines the `StepperMotor` class encapsulating motor operations.

## Setup Instructions
1. **Wiring:**
   - Connect each stepper motor to its driver.
   - Connect limit switches to designated Arduino pins.
   - (Optional) Connect LEDs for status indication.
   - Ensure power connections are secure and appropriate for motor specifications.

2. **Code Deployment:**
   - Place `main.cpp` in the `src` folder.
   - Place `StepperMotor.h` and `StepperMotor.cpp` in the `lib/StepperMotor` directory.
   - Upload the code to the Arduino Mega using PlatformIO or Arduino IDE.

## Usage
- **Homing:** Automatically performed on startup. To re-home, send the `HOME` command via Serial Monitor.
- **Commands:**
  - `MOVE_REL A <steps>`: Move Motor A relative steps.
  - `MOVE_REL B <steps>`: Move Motor B relative steps.
  - `MOVE_ABS A <position>`: Move Motor A to an absolute position.
  - `MOVE_ABS B <position>`: Move Motor B to an absolute position.
  - `GETPOS`: Retrieve current positions of both motors.

## Troubleshooting
- **Incorrect Direction:** Ensure `setInvertDirection(true)` is set for Motor B.
- **Homing Issues:** Verify limit switch placement and wiring. Ensure correct homing directions in code.
- **No Movement:** Check power supply and motor connections. Confirm drivers are enabled.

## License
This project is open-source and available under the [MIT License](LICENSE).

## Acknowledgments
Special thanks to the Arduino community for invaluable resources and support.

