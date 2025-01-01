
# Dual-Axis Light Tracking System Based on STM32

## Project Overview

This project implements a dual-axis light tracking system using the STM3210E EVAL board with an STM32F103ZG microcontroller. The system leverages two SG90 servo motors for directional control and five photoresistors for light detection. The goal is to dynamically align the system with the brightest light source in its environment, demonstrating real-time light tracking capabilities.

---

## Features

- **Real-Time Light Source Detection**: Five photoresistors detect the brightness of the surrounding environment to determine the light source's direction.
- **Dual-Axis Direction Control**: Two SG90 servo motors adjust the system’s orientation both horizontally and vertically.
- **Embedded Control System**: Powered by the STM32F103ZG microcontroller, with efficient ADC and PWM integration.
- **Scalability**: Modular design allows for future expansion or integration into larger systems such as solar trackers or robotic platforms.

---

## Hardware Requirements

### Components

1. **STM3210E EVAL Board** with STM32F103ZG microcontroller.
2. **Servo Motors**: 2x SG90.
3. **Photoresistors**: 5 units.
4. Additional components:
   - Jumper wires
   - Resistors for photoresistor voltage dividers
   - Power supply for STM32 and servos

### Pin Configuration

#### ADC Inputs (Photoresistors)

- A3 (JP3-1): CH3
- A5 (JP21-2): CH5
- A6 (JP21-3): CH6
- A7 (JP21-1): CH7
- C2 (JP19-1): CH12

#### PWM Outputs (Servo Motors)

- A0 (JP13-1): CH1
- A2 (JP3-2): CH3

---

## Software Requirements

1. **Development Environment**: Keil C 5.9.
2. **STM32 HAL Libraries** for ADC and PWM configuration.
3. **Toolchain**: ARM Compiler and STM32CubeMX for peripheral initialization.

---

## Setup and Installation

### Hardware Setup

1. **Photoresistor Circuit**: Connect the photoresistors to the ADC input pins (A3, A5, A6, A7, C2) through voltage divider circuits.
2. **Servo Motor Wiring**:
   - Connect the signal pins of the servo motors to PWM outputs (A0 for CH1, A2 for CH3).
   - Provide appropriate power (typically 5V) to the servo motors.
3. **STM32 Connections**: Ensure the EVAL board is powered and connected to the programming interface.

### Software Configuration

1. **Peripheral Initialization**:
   - Configure ADC channels for the photoresistor inputs.
   - Configure PWM outputs for controlling the servo motors.
2. **Code Upload**:
   - Compile the firmware in Keil and upload it to the STM32F103ZG via the appropriate programming interface.

---

## Usage Instructions

1. **System Calibration**:
   - Ensure the servo motors are in their neutral positions.
   - Verify that the photoresistors are providing stable ADC readings under uniform light.
2. **Operation**:
   - Power on the system.
   - The servos will adjust the orientation based on the photoresistor readings to align with the brightest light source.

---

## Algorithm Overview

1. **Light Detection**:
   - Read ADC values from the photoresistors.
   - Compare the values to determine the direction of maximum brightness.
2. **Servo Control**:
   - Compute the required angle adjustments for each axis.
   - Generate PWM signals to move the servo motors to the calculated positions.

---

## Results and Demonstration

- **Accuracy**: The system aligns with the brightest light source with minimal delay.
- **Performance**: Smooth and precise servo movements ensure stable tracking.
- **Potential Applications**: Solar trackers, robotic vision systems, and smart lighting systems.

---

## Future Enhancements

1. Increase the number of photoresistors for finer directional resolution.
2. Optimize the light detection algorithm for faster response times.
3. Integrate additional sensors (e.g., temperature, humidity) for environmental monitoring.
4. Develop a graphical user interface for real-time system monitoring.

---

## Author

- **Name**: Song-Hao, Lee
- **Contact**: lisong20050429@gmail.com

---

## License

This project is licensed under the MIT License. See the LICENSE file for details.

---

## References

1. STM32F103 Reference Manual
2. Keil uVision User Guide
3. Datasheet for SG90 Servo Motors
4. General guides for photoresistor-based light detection
