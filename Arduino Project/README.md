# Arduino Project — Line-Following Robot with Obstacle Detection

A line-following robot built with an Arduino Nano for contact-less COVID test kit delivery. Part of a parking-lot scenario where the robot navigates from a home position (H1/H2) through 6 intersections (i0–i5), detecting and approaching objects at up to 11 delivery points.

**Group 19:** Dajr Alfred, Mohammed Nauman Shariff, Gordon Oboh

---

## Overview

The robot follows a black tape line on a white background using an 8-channel Pololu QTR-8RC reflectance sensor array. It detects intersections, scans for objects using three HC-SR04 ultrasonic sensors (left, front, right), and approaches detected objects within 7cm before stopping.

**Track scenario:** A parking lot with two home positions (H1, H2), 6 intersections (i0–i5), and delivery points A1–A5, B1–B5, and C1.

## Hardware

| Component | Details |
|-----------|---------|
| **Microcontroller** | Arduino Nano (ATmega328, 32KB flash, 1KB EEPROM, 2KB SRAM) |
| **Line sensor** | Pololu QTR-8RC (8 digital IR sensors) |
| **Ultrasonic sensors** | 3x HC-SR04 (left, front, right) |
| **Motors** | 2x continuous rotation servos |
| **Indicators** | Blue LED (left), Red LED (right) |
| **Power** | 12V battery → voltage regulator → 5V |

### Pin Mapping

| Pin | Connection |
|-----|-----------|
| D2 | Front ultrasonic trigger |
| D3 | Front ultrasonic echo |
| D4 | Left ultrasonic echo |
| D5 | Left ultrasonic trigger |
| D6 | Right ultrasonic trigger |
| D7 | Right ultrasonic echo |
| D8–D9 | Pololu IR sensors 1–2 |
| D10 | Left servo signal |
| D11 | Right servo signal |
| D12 | Left LED (blue) |
| D13 | Right LED (red) |
| A0–A5 | Pololu IR sensors 3–8 |

## Software

The program uses a state-machine architecture (`switch/case` within `loop()`) with the following stages:

| Stage | Description |
|-------|-------------|
| **Calibration** | Auto-calibrates the QTR sensor (7–10s) |
| **Case 0** | Navigates from H1/H2 to intersection i0 |
| **Case 1** | Travels i0 → i5, scans for objects at each intersection using left/right ultrasonic sensors |
| **Case 2** | Uses front ultrasonic to approach objects at C1 |
| **Case 3** | Final stop, LED flash sequence |
| **Case 4** | Branches off main track to approach detected objects |
| **Case 5** | Returns to main track or proceeds to second object |

### Key Functions

- `readUltraSonic(trig, echo)` — measures distance using pulse timing
- `flashSequence(iters)` — blinks LEDs for indication
- `intersectionSequence()` — special LED pattern on intersection arrival
- `objSequence()` — LED pattern on object approach
- `moveForward()`, `moveBackward()`, `turnLeft()`, `turnRight()` — motor control
- `Stop()` — halts both servos

## Files

| File | Description |
|------|-------------|
| [`Mechatronic_Project_V4.ino`](Mechatronic_Project_V4.ino) | Main Arduino sketch |
| [`Arduino Project.docx (1).pdf`](Arduino%20Project.docx%20%281%29.pdf) | Project report |
| [`Arduino Project.docx (1).docx`](Arduino%20Project.docx%20%281%29.docx) | Project report (source) |
| [`Demonstration.mp4`](Demonstration.mp4) | Demo video (26MB) |

## Video

<video src="Demonstration.mp4" width="600" controls></video>

*Robot navigating the track, detecting objects, and stopping at delivery points*

## Result

The robot successfully follows the line, detects intersections, scans for objects, and approaches them within 7cm. In testing, 5 objects were placed at different intersections — the robot bypasses i0 (no scan), checks intersections i1–i5, and stops at the final destination C1.
