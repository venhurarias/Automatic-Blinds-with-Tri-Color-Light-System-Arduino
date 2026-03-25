# Automatic Blinds with Tri-Color Light System

This project is an Arduino-based **automated blinds control system** that adjusts window blinds based on environmental conditions and user input. It integrates light sensing, temperature monitoring, motorized control, and IR remote functionality.

Additionally, the system includes a **tri-color light indicator** to visually represent environmental status or system conditions.

---

## Project Overview

The system is designed to:

- automatically adjust blinds based on light intensity  
- provide manual control via IR remote  
- monitor temperature using a DHT sensor  
- ensure safe movement using limit switches  
- indicate system status using tri-color lighting  

It is suitable for:
- smart home automation  
- energy-efficient lighting control  
- indoor environmental monitoring  

---

## Features

### ☀️ Automatic Light Tracking
- Uses two LDR sensors (left and right)
- Detects light direction
- Adjusts blinds position to:
  - maximize natural light  
  - reduce glare  

---

### 🌡️ Temperature Monitoring
- Uses **DHT22 sensor**
- Reads ambient temperature
- Can be used to:
  - adjust blinds for heat control  
  - trigger system logic  

---

### 🎮 IR Remote Control
- Allows manual override of the system  
- Supports commands for:
  - opening blinds  
  - closing blinds  
  - adjusting position  

---

### 🔄 Motorized Blinds Control

Controls three directions:

- **Left motor** → horizontal adjustment  
- **Right motor** → horizontal adjustment  
- **Vertical motor** → opening/closing blinds  

---

### 🛑 Limit Switch Safety
- Prevents over-rotation or damage  
- Stops movement when:
  - left limit reached  
  - right limit reached  
  - vertical limit reached  

---

### 🚦 Tri-Color Light Indicator
- Uses LED indicators to show system status:

| Color  | Meaning                |
|--------|------------------------|
| Green  | Normal operation       |
| Yellow | Adjusting / active     |
| Red    | Limit reached / error  |

---

## System Workflow

### 1. Startup
- Initializes sensors and IR receiver  
- Sets motor pins and limit switches  

---

### 2. Automatic Mode
- Reads LDR values:
  - compares left vs right light intensity  
- Adjusts blinds direction accordingly  

---

### 3. Manual Mode (IR Remote)
- User sends command via remote  
- Overrides automatic behavior  
- Controls:
  - open / close blinds  
  - direction movement  

---

### 4. Safety Monitoring
- Continuously checks limit switches  
- Stops motors when limits are reached  

---

### 5. Status Indication
- Tri-color LED updates based on:
  - system state  
  - movement  
  - errors  

---

## Pin Configuration

| Component            | Arduino Pin |
|---------------------|------------|
| DHT22 Sensor        | 2          |
| LDR Left            | A0         |
| LDR Right           | A1         |
| Motor Left A        | 3          |
| Motor Left B        | 4          |
| Motor Right A       | 5          |
| Motor Right B       | 6          |
| Motor Vertical A    | 7          |
| Motor Vertical B    | 8          |
| Limit Right         | 9          |
| Limit Left          | 10         |
| Limit Vertical      | 11         |
| IR Receiver         | 13         |

---

## Hardware Components

- Arduino (Uno or Mega)  
- 3x DC Motors (or motor drivers)  
- Motor Driver Modules (L298N or similar)  
- 2x LDR Sensors  
- DHT22 Temperature Sensor  
- IR Receiver Module  
- IR Remote  
- Limit Switches (3x)  
- Tri-color LED (RGB LED or 3 LEDs)  

---

## Wiring Overview

### LDR Sensors
- Connected as voltage divider:
  - LDR → Analog pin (A0, A1)  
  - with resistor to GND  

---

### Motors
- Controlled via motor driver:
  - each motor uses two control pins (A/B)  
- Requires external power supply  

---

### Limit Switches
- Connected to digital pins with **INPUT_PULLUP**
- Active LOW when triggered  

---

### IR Receiver
- Signal → Pin 13  
- VCC → 5V  
- GND → GND  

---

### DHT22 Sensor
- Data → Pin 2  
- VCC → 5V  
- GND → GND  

---

### Tri-Color LED
- Connected to digital pins (not shown in code but included in system design)
- Each color controlled independently  

---

## Code Reference

📄 Source code:  
:contentReference[oaicite:0]{index=0}  

---

## Notes

- Uses **Chrono library** for non-blocking timing  
- IR control requires decoding remote values  
- Motor control assumes proper driver circuitry  
- Limit switches are critical for safety  

---

## Limitations

- No IoT or remote monitoring (offline system)  
- Light detection is threshold-based (no PID control)  
- Requires calibration for LDR sensitivity  
- IR remote depends on proper signal decoding  

---

## Summary

This project demonstrates a **smart automated blinds system** that combines:

- environmental sensing (light + temperature)  
- motorized control  
- safety mechanisms (limit switches)  
- user interaction (IR remote)  
- visual feedback (tri-color LED)  

It is suitable for:
- smart home automation  
- energy-efficient building systems  
- embedded control applications  
