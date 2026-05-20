# File_Desc.md

Suggested repo name: **`nyu-advanced-mechatronics`**

A clean, descriptive name reflecting the course and content.

---

## Arduino Project

| File | Type | Size | Description |
|------|------|------|-------------|
| `Mechatronic_Project_V4.ino` | Code | — | Main Arduino sketch — line-following state machine with obstacle detection |
| `Arduino Project.docx (1).pdf` | Report | 2.5MB | Project report (PDF) |
| `Arduino Project.docx (1).docx` | Report | — | Project report (source, DOCX) |
| `Demonstration.mp4` | Video | 26MB | Demo of the robot navigating the parking-lot track and detecting objects |

## Propeller Project

| File | Type | Size | Description |
|------|------|------|-------------|
| `ArduSerial.c` | Code | — | Propeller C code — multi-COG control: line-following, ultrasonic, servos, buzzer, friend/foe logic |
| `ard_interface.ino` | Code | — | Arduino sketch — reads QTR sensor values and sends to Propeller via UART |
| `Propeller Report.pdf` | Report | 1.6MB | Project report (PDF) |
| `Propeller Report.docx` | Report | — | Project report (source, DOCX) |
| `IMG_2307.mp4` | Video | 513KB | Demo of the robot navigating the grid track with obstacle avoidance (compressed from 136MB .MOV) |
| `IMG_2305.jpg` | Image | — | Photo of the robot hardware (converted from HEIC) |

## Term Project

| File | Type | Size | Description |
|------|------|------|-------------|
| `code3.c` | Code | — | Propeller C code — adds arm servo control, friend/foe GPIO interaction, and extended state machine |
| `RPiAndPropInteraction.py` | Code | — | Raspberry Pi Python script — captures camera frames, detects ArUco markers, classifies friend/foe, plays audio cues |
| `Final Term-Project.pdf` | Report | 4.9MB | Project report (PDF) |
| `Final Term-Project.docx` | Report | — | Project report (source, DOCX) |
| `20220523_053715_compressed.mp4` | Video | 9.8MB | Full demo — robot navigating, detecting friends and foes, arm deployment (compressed from 294MB) |
| `Demo Video.mp4` | Video | 13MB | Short highlight clip of friend/foe detection and arm response |
| `Hasta La Vista Baby.mp3` | Audio | 59KB | Sound played when a friendly ArUco tag is detected (ID ≤ 9) |
| `MK.mp3` | Audio | 29KB | Sound played when a foe ArUco tag is detected (ID > 9) |

### Original Uncompressed Files (kept for reference)

| File | Size | Notes |
|------|------|-------|
| `IMG_2307.MOV` | 136MB | Original Propeller project demo — exceeds GitHub's 100MB limit; use `IMG_2307.mp4` instead |
| `20220523_053715.mp4` | 294MB | Original Term Project demo — exceeds GitHub's 100MB limit; use `20220523_053715_compressed.mp4` instead |
| `IMG_2305.HEIC` | 623KB | Original photo — HEIC format; use `IMG_2305.jpg` instead |
