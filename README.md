# CAN_DRIVEN_VEHICLE_MONITORING_AND_DRIVER_ASSISTANCE_SYSTEM
To develop a multi-node distributed embedded system using CAN bus communication to perform real-time vehicle parameter monitoring (fuel, temperature) and provide driver assistance features (reverse distance safety alerts, indicator controls).

<div align="center">

# 🚗 CAN-Driven Vehicle Monitoring & Driver Assistance System

**A distributed automotive embedded system built with Controller Area Network (CAN) protocol**

[![Platform](https://img.shields.io/badge/Platform-LPC2129%20ARM7-blue?style=for-the-badge&logo=arm)](https://www.nxp.com)
[![Protocol](https://img.shields.io/badge/Protocol-CAN%202.0B%20125%20kbps-red?style=for-the-badge)](https://en.wikipedia.org/wiki/CAN_bus)
[![Language](https://img.shields.io/badge/Language-Embedded%20C-brightgreen?style=for-the-badge&logo=c)](https://en.wikipedia.org/wiki/Embedded_C)
[![IDE](https://img.shields.io/badge/IDE-Keil%20%C2%B5Vision-orange?style=for-the-badge)](https://www.keil.com/)
[![Status](https://img.shields.io/badge/Status-Complete-success?style=for-the-badge)]()

</div>

---

## 📌 Overview

The **CAN-Driven Vehicle Monitoring and Driver Assistance System** is a distributed automotive embedded solution designed to handle vehicle telematics and active driver assistance across a high-speed **125 kbps Controller Area Network (CAN)** bus. 

Built using three independent **NXP LPC2129 ARM7TDMI-S** microcontrollers, the system communicates deterministically via CAN message frames to monitor analog fuel level, ambient engine temperature via 1-Wire protocol, ultrasonic reverse distance, and directional turn indicators.

> 💡 This system replaces central wiring harnesses with a fault-tolerant, 2-wire differential CAN bus architecture, enabling decentralized real-time sensor sampling, driver alerts, and actuator responses.

---

## ✨ Features

| Feature | Description |
|---|---|
| 📡 **Distributed CAN Architecture** | 3 multi-node LPC2129 microcontrollers communicating at 125 kbps via CAN1 |
| ⛽ **Fuel Level Monitoring** | ADC-based fuel level sensing ($0-3.3\text{V} \rightarrow 0-100\%$) transmitted via CAN ID 1 |
| 🌡️ **Temperature Monitoring** | 1-Wire DS18B20 digital sensor monitoring temperature in °C with sub-zero support |
| 🦇 **Ultrasonic Reverse Ranging** | Timer0 echo pulse measurement calculating obstacle distance in centimeters (CAN ID 2) |
| 🚨 **Three-Tier Collision Warning** | Automatic distance classification (`SAFE` / `WARNING` / `STOP`) on main display |
| 🔊 **Audible Distance Alerts** | Pulsed buzzer warning frequency triggered during reverse mode based on obstacle proximity |
| 💡 **Sequential Turn Indicators** | EINT1 & EINT2 interrupt-driven left/right indicator controls with LED running sequences |
| 🔄 **Vehicle Direction Toggle** | EINT0 switch interrupt toggling between FORWARD and REVERSE operation modes |
| 🖥️ **Custom Character LCD Interface** | Custom CGRAM icons for indicators and dynamic 4-stage fuel tank graphics |

---

## 🚦 Driver Assistance & Warning Logic

### 1️⃣ Reverse Distance Thresholds

```
Distance (cm)           Safety Status       Buzzer Warning Pattern
──────────────────     ───────────────     ───────────────────────────
> 50 cm           →    🟢 SAFE         →    Blink 5 times (Slow pulse)
21 cm – 50 cm     →    🟡 WARNING      →    Blink 2 times (Normal alert)
≤ 20 cm           →    🔴 STOP         →    Blink 2 times (Fast alert)
```

### 2️⃣ Operational Mode View Logic

* **FORWARD MODE (`dir == 1`):** Displays turn indicators (← / →), fuel percentage, fuel level icon, and DS18B20 live temperature.
* **REVERSE MODE (`dir == 0`):** Displays obstacle distance (`DIST: XX CM`) and real-time safety status (`SAFE` / `WARNING` / `STOP`).

---

## 🏗️ System Architecture

<div align="center">

```
                   +---------------------------------------+
                   |         CAN BUS (125 kbps)            |
                   +--+-----------------+---------------+--+
                      |                 |               |
                      | ID 1            | ID 2          | ID 3 / ID 4
                      v                 v               v
+-----------------------+     +-------------------+     +-----------------------+
|  FUEL NODE (LPC2129)  |     | MAIN NODE (LPC2129|     | REVERSE NODE (LPC2129)|
|                       |     |                   |     |                       |
| - ADC0 (Fuel Sensor)  |---->| - CAN RX / TX     |<---| - Ultrasonic (Trigger/ |
| - LCD Display         |     | - DS18B20 Temp    |     |   Echo Pins P0.21/22) |
| - CAN TX (ID 1)       |     | - EINT0 (Fwd/Rev) |     | - LED Indicators      |
+-----------------------+     | - EINT1/2 (Turn)  |     | - Warning Buzzer      |
                              | - 20x4 LCD        |     +-----------------------+
                              +-------------------+
```

</div>

**Block overview:**
- **Fuel Node (`FUELNODE.c`)** — Converts analog fuel voltage into percentage, displays locally on LCD, transmits frame `ID 1`.
- **Main Node (`MAINNODE.c`)** — Processes incoming CAN frames, handles external interrupts (EINT0/1/2), reads DS18B20 temp sensor, renders central driver HUD display, and sends indicator CAN messages (`ID 3`/`ID 4`).
- **Reverse Node (`Reversenode.c`)** — Measures distance using HC-SR04 ultrasonic sensor via Timer0, transmits frame `ID 2`, drives 8-LED sequential indicator sequences, and actuates warning buzzer.

---

## 🔌 Hardware Components

| # | Component | Part / Spec | Role |
|---|---|---|---|
| 1 | Microcontrollers | 3× NXP LPC2129 (ARM7TDMI-S, 60 MHz) | Distributed processing nodes |
| 2 | Bus Interface | On-Chip CAN1 Controllers (125 kbps) | Differential serial communication |
| 3 | Displays | HD44780 Character LCDs (16×2 / 20×4) | Local node status & main dashboard |
| 4 | Temperature Sensor | DS18B20 Digital Sensor (1-Wire Protocol) | Engine/ambient temp sensing |
| 5 | Distance Sensor | HC-SR04 Ultrasonic Ranging Sensor | Rear obstacle detection |
| 6 | Fuel Level Input | Potentiometer / Analog Sensor ($0-3.3\text{V}$) | Fuel gauge input |
| 7 | Indicators | 8× General Purpose LEDs | Left & Right turn signal visualizers |
| 8 | Alarm | Active Buzzer (P0.23) | Distance alert sounder |
| 9 | Push Buttons | 3× External Interrupt Switches | EINT0 (Direction), EINT1 (Left), EINT2 (Right) |

---

## 📍 Pin Configuration

### ⛽ Fuel Node (LPC2129)

| Pin | Function | Role |
|---|---|---|
| P0.25 | CAN1 TX | CAN Transmit line |
| P0.27 | ADC CH0 | Analog fuel sensor input |
| P0.8 – P0.15 | LCD_DATA | LCD 8-bit data bus |
| P0.16 | LCD_RS | LCD Register Select |
| P0.17 | LCD_EN | LCD Enable |
| P0.18 | LCD_RW | LCD Read/Write |

### 🖥️ Main Node (LPC2129)

| Pin | Function | Role |
|---|---|---|
| P0.25 | CAN1 TX | CAN Transmit line |
| P0.1 | EINT0 | Forward/Reverse mode toggle button |
| P0.3 | EINT1 | Left Indicator toggle button |
| P0.7 | EINT2 | Right Indicator toggle button |
| P0.16 | 1-WIRE DATA | DS18B20 digital temperature line |
| P0.8 – P0.15 | LCD_DATA | 20×4 LCD 8-bit data bus |

### 🦇 Reverse Node (LPC2129)

| Pin | Function | Role |
|---|---|---|
| P0.25 | CAN1 TX | CAN Transmit line |
| P0.21 | TRIG_PIN | Ultrasonic trigger output |
| P0.22 | ECHO_PIN | Ultrasonic echo input |
| P0.23 | BUZZER | Active buzzer alarm output |
| P0.0 – P0.7 | LED_OUTPUTS | 8-LED sequential indicator array |

---

## 📡 CAN Message Identifier Specification

| CAN ID | Frame Type | DLC | Source Node | Destination | Data Payload (`data1`) |
|---|---|---|---|---|---|
| **`1`** | Data Frame | 4 Bytes | Fuel Node | Main Node | Fuel percentage integer ($0-100$) |
| **`2`** | Data Frame | 4 Bytes | Reverse Node | Main Node | Distance integer (cm) |
| **`3`** | Data Frame | 4 Bytes | Main Node | Reverse Node | Left Indicator command (`1`) |
| **`4`** | Data Frame | 4 Bytes | Main Node | Reverse Node | Right Indicator command (`1`) |

---

## 📁 Project Structure

The project features a modular multi-file structure cleanly separating peripheral drivers, protocol stacks, and individual node executable codebases.

```
CAN-Vehicle-Monitoring-System/
│
├── inc/                          ← Header files (declarations & hardware macros)
│   ├── types.h                   ← Custom data types (u8, s8, u16, u32, s32, f32)
│   ├── defines.h                 ← Bitwise manipulation macros (SETBIT, CLRBIT, WRITENBITS)
│   ├── can.h                     ← CAN driver function prototypes & `canf` frame struct
│   ├── can_defines.h             ← CAN bit timing constants, baud rate registers (125 kbps)
│   ├── adc.h                     ← ADC driver API
│   ├── can_adc_defines.h         ← ADC clock dividers & register bit definitions
│   ├── ds18b20.h                 ← 1-Wire DS18B20 temperature driver API
│   ├── ultra_sonic.h             ← Ultrasonic trigger & timer-based pulse calculation API
│   ├── lcd.h                     ← LCD display control API
│   ├── lcd_defines.h             ← HD44780 command codes & pin mappings
│   └── delay.h                   ← Software delay prototypes (us, ms, sec)
│
├── src/                          ← Driver implementations
│   ├── CAN.c                     ← CAN1 initialization (`init_can1`), TX (`can1_tx1`), RX (`can1_rx1`)
│   ├── ADC.c                     ← 10-bit ADC conversion routines (`read_adc`)
│   ├── DS18B20.c                 ← 1-Wire bit-banging & temperature calculation routines
│   ├── ULTRASONIC.c              ← HC-SR04 pulse measurement using Timer0
│   ├── LCD.c                     ← LCD 8-bit mode driver & custom CGRAM icon builder
│   └── Delay.c                   ← Precise delay loop implementations
│
├── nodes/                        ← Standalone Node Main Application Code
│   ├── FUELNODE.c                ← Fuel Node main executable (ADC + CAN TX ID 1)
│   ├── MAINNODE.c                ← Main Dashboard main executable (Interrupts + CAN RX/TX + Temp)
│   └── Reversenode.c             ← Reverse Node main executable (Ultrasonic + Buzzer + LEDs)
│
└── Makefile / .uvproj            ← Keil µVision Build Configuration
```

---

## ⚙️ How It Works

### 1️⃣ Network Initialization & Startup
```
Power ON → Each Node initializes CAN1 peripheral (125 kbps, C1BTR config)
         → Fuel Node: Initialized ADC Channel 0 & Local LCD
         → Reverse Node: Configures GPIOs for Ultrasonic, Buzzer & Indicator LEDs
         → Main Node: Configures LCD, External Interrupts (EINT0, EINT1, EINT2) & DS18B20
```

### 2️⃣ Fuel Telematics Loop (`FUELNODE.c`)
- Measures ADC voltage from fuel sensor:
  $$\text{Voltage} = \frac{\text{ADC\_Val} \times 3.3}{1023}$$
- Computes percentage:
  $$\text{Percentage} = \left(\frac{\text{Voltage}}{3.3}\right) \times 100$$
- Transmits CAN frame (`ID: 1`, `DLC: 4`, `data1: per`).

### 3️⃣ Main Dashboard Hub (`MAINNODE.c`)
- **CAN Receiver:** Non-blocking check via `C1GSR` register for incoming CAN frames.
- **Interrupt Signals:**
  - `EINT0_isr`: Toggles `dir` state between Forward (`1`) and Reverse (`0`).
  - `EINT1_isr`: Sends CAN `ID: 3` to trigger left running LEDs.
  - `EINT2_isr`: Sends CAN `ID: 4` to trigger right running LEDs.
- **Display Rendering:** Dynamically paints 4-line LCD HUD displaying temperatures, dynamic battery/fuel icons, indicator status, or obstacle safety alerts.

### 4️⃣ Driver Assistance & Actuation (`Reversenode.c`)
- Fires ultrasonic trigger pulse ($10\,\mu\text{s}$) and measures echo pulse width using Timer0 (`T0TC`).
- Calculates distance:
  $$\text{Distance (cm)} = \frac{\text{Timer0 Pulse Counts}}{59.0}$$
- Transmits distance to Main Node via CAN (`ID: 2`).
- Listens for CAN `ID 3`/`ID 4` commands to trigger running LED indicator visualizers on Port 0 (`P0.0–P0.7`).
- Sounds buzzer pulses at threshold-dependent intervals.

---

## 🧮 Mathematical Formulas

### 1️⃣ CAN Baud Rate Generation (125 kbps)
$$\text{PCLK} = \frac{\text{CCLK}}{4} = \frac{60\text{ MHz}}{4} = 15\text{ MHz}$$
$$\text{BRP} = \frac{\text{PCLK}}{\text{Bitrate} \times \text{Quanta}} = \frac{15,000,000}{125,000 \times 15} = 8$$

### 2️⃣ Analog-to-Digital Fuel Conversion
$$\text{Fuel Voltage (V)} = \frac{\text{ADC\_Value} \times 3.3\text{V}}{1023}$$

### 3️⃣ Ultrasonic Distance Formula
$$\text{Distance (cm)} = \frac{\text{Timer0 Pulse Counts}}{59.0}$$

---

## 🛠️ Development Environment

| Tool | Details |
|---|---|
| **IDE** | Keil µVision 4 / 5 (ARM-MDK) |
| **Compiler** | Keil RealView ARM C Compiler |
| **Target MCU** | NXP LPC2129 — ARM7TDMI-S @ 60 MHz |
| **Peripheral Clock** | 15 MHz |
| **CAN Speed** | 125 kbps |
| **Flashing Tool** | Flash Magic (UART ISP) |
| **Simulation** | Proteus VSM / Keil Simulator |
| **Language** | Embedded C |

---

## 🚀 Getting Started

### Prerequisites
- Keil µVision IDE installed.
- 3× LPC2129 development boards or a multi-node CAN test bed.
- CAN Transceiver ICs (e.g., SN65HVD230 or MCP2551) connected to `CAN1`.
- USB-to-UART converter / Flash Magic tool.

### Build & Flash Steps

```bash
# 1. Clone the repository
git clone https://github.com/YOUR_USERNAME/CAN-Vehicle-Monitoring-System.git
cd CAN-Vehicle-Monitoring-System

# 2. Build Fuel Node Target
#    - Open Keil project, select Target: FUEL_NODE
#    - Compile FUELNODE.c, CAN.c, ADC.c, LCD.c, Delay.c
#    - Generate FUEL_NODE.hex and flash to Node 1 via Flash Magic

# 3. Build Reverse Node Target
#    - Select Target: REVERSE_NODE
#    - Compile Reversenode.c, CAN.c, ULTRASONIC.c, LCD.c, Delay.c
#    - Generate REVERSE_NODE.hex and flash to Node 2 via Flash Magic

# 4. Build Main Node Target
#    - Select Target: MAIN_NODE
#    - Compile MAINNODE.c, CAN.c, DS18B20.c, LCD.c, Delay.c
#    - Generate MAIN_NODE.hex and flash to Node 3 via Flash Magic

# 5. Hardware Interconnect
#    - Connect CAN_H and CAN_L lines across all 3 nodes with 120Ω termination resistors.
#    - Power up all 3 nodes simultaneously.
```

---

<div align="center">

**Built with ❤ on ARM7 | LPC2129 | CAN Bus Protocol | Embedded C**

</div>
