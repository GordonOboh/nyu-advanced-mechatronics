# Advanced Mechatronics — NYU

Three successive line-following robot projects built for an Advanced Mechatronics course at NYU. Each iteration adds complexity — from a single Arduino controller to a multi-processor system with computer vision.

**Group 19:** Dajr Alfred, Mohammed Nauman Shariff, Gordon Oboh

---

## Projects

### 1. Arduino Project

A line-following robot using an **Arduino Nano** with a Pololu QTR-8RC reflectance sensor. Navigates a parking-lot track, detects intersections using 8 IR sensors, and scans for objects using 3 HC-SR04 ultrasonic sensors (left, front, right). Uses continuous rotation servos for differential drive.

**Key features:** Single-microcontroller design, state-machine control flow, contact-less object approach within 7cm.

[▶ Read more →](Arduino%20Project/README.md)

### 2. Propeller Project

Upgrades to a **Parallax Propeller + Arduino Nano** dual-processor architecture. The Arduino handles QTR sensor reading; the Propeller runs 5 parallel COGs for line-following, ultrasonic scanning, servo control, and signaling. Enables simultaneous operation — no need to stop and scan.

**Key features:** Multi-core processing, obstacle avoidance on a grid track, 10 delivery points.

[▶ Read more →](Propeller%20Project/README.md)

### 3. Term Project

The final, most complex version: adds a **Raspberry Pi 3B with Camera Module V2** for computer vision. The robot detects ArUco markers and classifies them as friend or foe using OpenCV. A servo arm responds to threats, and audio cues play based on the tag type.

**Key features:** Three-processor system (Arduino + Propeller + RPi), real-time ArUco detection, friend/foe classification, actuated arm response.
- **Friend (ID ≤ 9):** Plays "Hasta La Vista Baby" — no action taken
- **Foe (ID > 9):** Plays "MK.mp3" — arm servo deploys

[▶ Read more →](Term%20Project/README.md)

---

## Hardware Evolution

| Component | Arduino Project | Propeller Project | Term Project |
|-----------|:---------------:|:-----------------:|:------------:|
| Arduino Nano | ✓ QTR reader + control | ✓ QTR reader only | ✓ QTR reader only |
| Propeller | — | ✓ Master controller | ✓ Master controller |
| Raspberry Pi 3B | — | — | ✓ Vision processing |
| Camera Module V2 | — | — | ✓ ArUco detection |
| Pololu QTR-8RC | ✓ | ✓ | ✓ |
| HC-SR04 / Ping))) | 3x | 3x | 3x |
| Servo motors | 2x drive | 2x drive | 2x drive + 1x arm |
| LEDs | Blue + Red | 2x | 2x |
| Buzzer | — | ✓ | ✓ |

## Software Evolution

| Capability | Arduino | Propeller | Term |
|------------|:-------:|:---------:|:----:|
| Line-following | ✓ | ✓ | ✓ |
| Intersection detection | ✓ | ✓ | ✓ |
| Ultrasonic object scan | ✓ | ✓ | ✓ |
| Multi-core parallelism | — | ✓ | ✓ |
| Obstacle avoidance | — | ✓ | ✓ |
| Grid track navigation | — | ✓ | ✓ |
| ArUco marker detection | — | — | ✓ |
| Friend/Foe classification | — | — | ✓ |
| Servo arm actuation | — | — | ✓ |
| Audio cues | — | — | ✓ |

## Track Scenario

All three projects navigate a grid-like track inspired by Manhattan/Brooklyn street patterns:

- 2 home positions (H1, H2)
- 16 intersections (i0–i5, B1–B5, A1–A5)
- Up to 11 delivery points
- Bi-directional center lane with one-way side streets
- Randomly placed obstacle in the center lane

---

## File_Desc.md

See [`File_Desc.md`](File_Desc.md) for a complete inventory of all media files, code, and reports.
