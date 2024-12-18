# Dual Stepper Motor Control with Arduino

## Overview
This project enables precise control of two stepper motors using an Arduino Mega. It features homing sequences, relative and absolute movements, motor inversion, and limit switch integration to ensure accurate positioning. The project is tailor made to replace a proprietary and outdated controller for the Pan/Tilt unit PTU-46.

## Features
- **Dual Motor Control:** Independently control two stepper motors.
- **Homing Sequence:** Automatically home each motor to establish a reference position.
- **Relative & Absolute Movements:** Move motors by a specified number of steps or to an exact position within defined bounds.
- **Limit Switch Integration:** Utilize limit switches to prevent over-travel.

## Hardware 
- **Arduino Mega 2560**
- **2 Stepper Motors**
- **2 Stepper Motor Drivers (L298N)**
- **2 Limit Switches**
- **2 LEDs** 
- **Connecting Wires**
- **Power Supply** suitable for motors

## Software Structure
- **`main.cpp`**: Handles initialization, homing, and serial command processing.
- **`StepperMotor.h` & `StepperMotor.cpp`**: Defines the `StepperMotor` class encapsulating motor operations.

## Usage
- **Homing:** Automatically performed on startup. To re-home, send the `HOME` command via Serial Monitor.
- **Commands:**
  - `MOVE_REL A <steps>`: Move Motor A relative steps.
  - `MOVE_REL B <steps>`: Move Motor B relative steps.
  - `MOVE_ABS A <position>`: Move Motor A to an absolute position.
  - `MOVE_ABS B <position>`: Move Motor B to an absolute position.
  - `GETPOS`: Retrieve current positions of both motors.

## Connectivity
- **Serial Connection:** Use a terminal such as PuTTY to connect to the Arduino's serial port. Set the baud rate to `9600` to send commands.
- **Ethernet Connection:** Connect to the Arduino over Ethernet using the self-assigned IP `192.168.100.115` on port `80`. Send commands in the same manner as with the serial connection. You can use a terminal such as netcat (`nc`) to establish the connection or use custom code such as Python, see below.


### Python Control Script

The repository includes a Python script `arduino_coms.py` that allows control of the Arduino over Ethernet. This script demonstrates how to send commands to the Arduino using Python's `socket` module.

To use the script, run:
```bash
python arduino_coms.py
```

#### Args:
- `positions` (list or dict): A list of dictionaries or a single dictionary specifying the positions to move the motors.
  - Each dictionary can have the following keys:
    - `home` (bool): If True, the motors will be homed.
    - `pos_a` (int or float): The absolute position to move motor A.
    - `pos_b` (int or float): The absolute position to move motor B.

#### Returns:
- `str`: "success" if all commands are executed successfully, otherwise an error message.

#### Raises:
- `Exception`: If an error occurs during the scan.

#### Example

```python
positions = [
  {'home': True},
  {'pos_a': 10, 'pos_b': 20},
  {'pos_a': 30, 'pos_b': 40}
]

result = interpret_command(positions)
```

In this example, the `positions` list contains three dictionaries, each representing a set of commands for the Arduino to execute:

1. The first dictionary `{'home': True}` instructs the Arduino to home both motors. Homing typically means moving the motors to their starting or reference positions.
2. The second dictionary `{'pos_a': 10, 'pos_b': 20}` commands the Arduino to move Motor A to position 10 and Motor B to position 20.
3. The third dictionary `{'pos_a': 30, 'pos_b': 40}` commands the Arduino to move Motor A to position 30 and Motor B to position 40.

Sending the entire list will make the Arduino go through all three positions sequentially:
- First, it will home both motors.
- Then, it will move Motor A to position 10 and Motor B to position 20.
- Finally, it will move Motor A to position 30 and Motor B to position 40.

Alternatively, sending one entry at a time will make the Arduino execute just that one entry. For example:
- Sending `{'home': True}` will only home the motors.
- Sending `{'pos_a': 10, 'pos_b': 20}` will only move Motor A to position 10 and Motor B to position 20.
- Sending `{'pos_a': 30, 'pos_b': 40}` will only move Motor A to position 30 and Motor B to position 40.

This flexibility allows you to control the Arduino's actions either step-by-step or in a batch, depending on your needs.

Ensure you have Python installed along with any necessary dependencies. The script connects to the Arduino at the IP `192.168.100.115` on port `80` and allows sending commands similar to those used in the terminal.

This script allows easy integration into other systems, with the `interpret_command` function being a feature to send commands without the use of a terminal window.

## License
This project is open-source and available under the [MIT License](LICENSE).


