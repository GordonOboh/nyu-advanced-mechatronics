# Term Project — Line-Following Robot with Friend/Foe Detection

The final and most advanced iteration: a line-following robot that identifies **friendlies and enemies** using ArUco marker detection, with an **arm servo** to eliminate threats. Combines three processors — Arduino Nano (QTR sensor), Parallax Propeller (motor control + ultrasonic sensing), and Raspberry Pi 3B (computer vision).

Scenarios: An autonomous robot scours Manhattan-styled streets for hostile intruders while avoiding obstacles and obeying traffic laws.

**Group 19:** Dajr Alfred, Mohammed Nauman Shariff, Gordon Oboh

---

## Overview

The robot navigates a grid track with 16 intersections, 10 delivery points, and a randomly placed obstacle. It uses the same multi-core Propeller architecture as the previous project, but adds a Raspberry Pi with a camera module for real-time ArUco tag detection. Each tag is classified as "friend" (ID ≤ 9) or "foe" (ID > 9) — friendlies are spared, enemies trigger the arm servo.

**Key upgrade from Propeller-only version:** Computer vision enables the robot to distinguish between friend and foe, with an actuated arm to respond to threats.

## System Architecture

```
┌─────────────────┐     UART (115200)     ┌──────────────────┐     GPIO     ┌────────────────┐
│  Arduino Nano   │ ────────────────────→ │    Propeller     │ ←─────────→ │  Raspberry Pi  │
│  (QTR Sensor)   │    sensor data        │  (Master/Actuator)│   friend/foe│  (Vision/ArUco)│
└─────────────────┘                       └──────────────────┘    start    └────────────────┘
                                                   │                          │
                                    ┌──────────────┼──────────────┐           │ Camera Module
                                    ▼              ▼              ▼           │ V2 (8MP)
                              Servo Motors   Ultrasonic x3   Arm Servo
                                                         LEDs + Buzzer
```

**Data flow:**
1. Arduino reads QTR sensor → sends to Propeller via UART
2. Propeller follows line, scans ultrasonics, controls servos and arm
3. Propeller signals Raspberry Pi to scan via Start Pin (GPIO)
4. Raspberry Pi captures frame, detects ArUco markers, classifies friend/foe
5. RPi sets Friend/Foe pin high → Propeller reads and acts

## Hardware

| Component | Details |
|-----------|---------|
| **Vision** | Raspberry Pi 3B + Camera Module V2 (8MP Sony IMX219) |
| **Master** | Parallax Propeller Activity Board (8-core P8X32A-Q44) |
| **Slave** | Arduino Nano (QTR sensor interface) |
| **Line sensor** | Pololu QTR-8RC (8 digital IR sensors) |
| **Ultrasonic sensors** | 3x Ping))) (left, front, right) |
| **Motors** | 2x continuous rotation servos |
| **Arm servo** | 1x standard servo for threat elimination |
| **Indicators** | 2x LEDs, 1x buzzer |

### Pin Mapping

**Arduino:**
| Pin | Connection |
|-----|-----------|
| D2–D9 | QTR sensor pins 8–1 |
| D10 | QTR emitter LED control |
| D11 | UART RX (to Propeller TX) |
| D12 | UART TX (to Propeller RX) |

**Propeller:**
| Pin | Connection |
|-----|-----------|
| P1–P2 | LED 1, LED 2 |
| P3–P5 | Right, Front, Left ultrasonic (Ping))) signal) |
| P9 | RPi Start Trigger (output to RPi) |
| P10 | Foe Pin (input from RPi) |
| P11 | Friend Pin (input from RPi) |
| P12 | Buzzer |
| P13 | UART RX (to Arduino TX) |
| P14 | UART TX (to Arduino RX) |
| P15 | Arm servo signal |
| P16 | Left drive servo signal |
| P17 | Right drive servo signal |

**Raspberry Pi (GPIO):**
| Pin | Connection |
|-----|-----------|
| GPIO2 (Pin 3) | Friend Pin → Propeller P11 |
| GPIO3 (Pin 5) | Foe Pin → Propeller P10 |
| GPIO4 (Pin 7) | Start Pin ← Propeller P9 |
| GND (Pin 6) | Common ground with Propeller |

## Software

### Raspberry Pi (Python)

- Uses OpenCV ArUco module to detect markers
- `FriendOrFoe(TagID)` — plays "Hasta La Vista Baby" for friends (ID ≤ 9), "MK.mp3" for foes (ID > 9)
- Sets GPIO Friend/Foe pin high to signal Propeller
- Camera runs at 5 FPS to reduce false triggers

### Propeller (C — Multi-COG)

State machine with 5 parallel COGs:
- **getPos()** — reads QTR data from Arduino via UART
- **getDist()** — reads all 3 ultrasonic sensors (also flags `lftSense`/`frtSense`/`rgtSense`)
- **lineFollower()** — PID-style line tracking error correction
- **spinServos()** — controls drive and arm servos
- **soundLights()** — handles buzzer and LED sequences

Navigation stages build on the Propeller project with added friend/foe logic:
- **Case 3–4 extended:** When left sensor detects an object at a delivery point, the robot stops, signals RPi to scan, reads friend/foe status, and activates the arm servo (1800µs = attack) if a foe is identified. If a friend is detected, the robot continues without action.

### Arduino (C++)

- Reads QTR sensor via Pololu library
- Sends 9 values (position + 8 sensor readings) to Propeller at 115200 baud
- Calibration sequence runs on startup

## Files

| File | Description |
|------|-------------|
| [`code3.c`](code3.c) | Propeller C code (multi-COG control, friend/foe logic, arm servo) |
| [`RPiAndPropInteraction.py`](RPiAndPropInteraction.py) | Raspberry Pi Python code (ArUco detection, friend/foe classification) |
| [`Final Term-Project.pdf`](Final%20Term-Project.pdf) | Project report |
| [`Final Term-Project.docx`](Final%20Term-Project.docx) | Project report (source) |
| [`20220523_053715_compressed.mp4`](20220523_053715_compressed.mp4) | Full demo video (9.8MB — compressed) |
| [`Demo Video.mp4`](Demo%20Video.mp4) | Short demo clip (13MB) |
| [`Hasta La Vista Baby.mp3`](Hasta%20La%20Vista%20Baby.mp3) | Sound played when friend detected |
| [`MK.mp3`](MK.mp3) | Sound played when foe detected |

## Videos

### Full Demo

<video src="20220523_053715_compressed.mp4" width="600" controls></video>

*Full demonstration — robot navigating, detecting friendlies and enemies*

### Short Clip

<video src="Demo%20Video.mp4" width="600" controls></video>

*Highlight of the friend/foe detection and arm response*

## Result

In testing, 4 friendlies and 4 foes were placed at B5 and A4 with an obstacle at i2. The robot navigates from home down the center lane, detects the obstacle, reverses to i1, branches to B1, scans B1–B4 using the camera for ArUco tags, then moves to A4 and scans A4–A1. Friendlies trigger an audio cue; foes trigger the arm.
