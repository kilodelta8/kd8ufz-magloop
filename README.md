# KD8UFZ-MagLoop
**A Remote-Tuned Magnetic Loop Antenna Controller**

A high-performance, Raspberry Pi Pico-based magnetic loop controller designed for stealth and high-power operations (up to 100W). This system uses a dual-core RP2040 to manage precision stepper motor tuning at the antenna while providing a sleek, rotary-encoder-driven UI at the operating position via Cat5.

## 🚀 Key Features
* **Remote Brain Architecture:** Pico and Stepper Driver are located at the antenna to minimize RFI.
* **Precision Tuning:** Geared NEMA 17 stepper support with micro-stepping for razor-thin SWR dips.
* **Smart Presets:** Save and recall positions for 7 amateur bands (40m–10m) via onboard flash memory.
* **Integrated SWR Bridge:** Real-time SWR bar graph on a 16x2 I2C LCD.
* **Single-Cable Link:** Control signals and power delivered via a single shielded Cat5/6 cable.

---

## 📂 Repository Structure
```text
KD8UFZ-MagLoop/
├── firmware/                # Pico C++ (SDK / PlatformIO)
│   ├── src/main.cpp         # State machine and I2C polling
│   └── src/stepper.cpp      # Core 1 dedicated motor timing
├── hardware/                # Schematics and PCB layouts
│   ├── pcb/                 # Stockton Bridge & Pico Shield
│   └── exports/             # PDF Schematics for the workbench
├── mechanical/              # 3D and CAD files
│   ├── enclosure/           # Control Box STL files
│   └── antenna_stand/       # PVC swivel mount plans
└── docs/                    # BOM and Documentation
```

---

## 🛠 Hardware Configuration
### Cat5/6 Pinout Map (Remote Link)
| Pair | Color | Signal | Purpose |
| :--- | :--- | :--- | :--- |
| **1** | Orange/White | **+12V DC** | Main system power (Double up for current) |
| **2** | Blue/White | **I2C SDA/SCL**| Data for 16x2 LCD and Encoder |
| **3** | Green/White | **Ground** | Common logic/power ground |
| **4** | Brown/White | **Home/Sense** | Signal for capacitor "Zero" limit switch |

### Critical Components
* **Microcontroller:** Raspberry Pi Pico (RP2040)
* **Stepper Driver:** TMC2208 (SilentStepStick)
* **Antenna Capacitor:** OEP Transmitting Air Variable (High Voltage)
* **UI:** 16x2 LCD (I2C) + PEC11R Rotary Encoder

---

## 💻 Firmware Setup
The firmware is written in **C++** using the **Pico SDK**. 

### Build Instructions
1.  Install **VS Code** and the **PlatformIO** extension.
2.  Clone this repo: `git clone https://github.com/kilodelta8/KD8UFZ-MagLoop.git`
3.  Open the `firmware/` folder in PlatformIO.
4.  Run `PIO Build` to compile the `.uf2` file.
5.  Connect Pico in Bootsel mode and drag/drop the binary.

### UI Logic (The "Single-Knob" Workflow)
* **Rotate Knob:** Scrolls through band presets or fine-tunes SWR.
* **Short Press:** Toggles between `Band Selection` and `Fine Tune` modes.
* **Long Press (3s):** Overwrites the current band preset with the current step count.

---

## ⚠️ Safety Warning
This antenna is designed for high-voltage RF. At 100W, the capacitor plates can exceed **5,000V**. 
* **Do not** touch the antenna while transmitting.
* **Do not** operate without the SWR bridge properly calibrated.
* Ensure the control box is properly shielded in a die-cast aluminum enclosure to prevent I2C "ghosting" from near-field RF.

---

**License:** MIT  
**Author:** KD8UFZ  
**Project Status:** Active Development (Phase 1: Prototyping)

---


