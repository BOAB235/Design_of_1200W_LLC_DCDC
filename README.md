<div align="center">

# 1200W 400V/48V LLC Resonant DC-DC Converter

**Open-Source Design, Firmware & Experimental Validation**
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](#license)

<br>

<img src="https://boab235.github.io/pages/llc/imgs/dcdc_all2.png" alt="1200W LLC DC-DC Converter" width="750">

<br>

*A complete open-source LLC resonant DC-DC converter project — from mathematical design justification through PCB layout, bare-metal STM32 firmware, EMC testing, and efficiency characterization.*

<br>

[**Project Page & Interactive Gallery**](https://boab235.github.io/pages/llc/llc.html)

</div>

---

## Table of Contents

- [Overview](#overview)
- [Key Specifications](#key-specifications)
- [Repository Structure](#repository-structure)
- [Hardware Design](#hardware-design)
  - [LLC Resonant Tank](#llc-resonant-tank)
  - [Magnetic Components](#magnetic-components)
  - [MOSFET Selection](#mosfet-selection)
  - [Capacitor Design](#capacitor-design)
  - [EMI Filter](#emi-filter)
  - [Gate Driver & Protection](#gate-driver--protection)
  - [Schematic & PCB](#schematic--pcb)
- [Firmware](#firmware)
  - [Architecture](#architecture)
  - [Control Strategy](#control-strategy)
  - [Soft Start](#soft-start)
- [Validation & Testing](#validation--testing)
- [Tools & Technologies](#tools--technologies)
- [Getting Started](#getting-started)
- [Project Page](#project-page)
- [License](#license)

---

## Overview

This project presents the **complete design and experimental evaluation** of a **1200 W LLC resonant DC-DC converter** converting **400 V DC to 48 V DC**. It covers every stage of the development lifecycle:

1. **Analytical Design** — Resonant tank parameter optimization, magnetic core selection, MOSFET thermal analysis, EMI filter sizing, and PCB trace calculations, all documented in Jupyter notebooks with reproducible equations.
2. **Hardware Implementation** — Full KiCad schematic, BOM, Gerber files, and a custom PCB with planar magnetics.
3. **Bare-Metal Firmware** — 21+ iterative control firmware versions on an STM32F103C8T6 (72 MHz Cortex-M3), progressing from fixed-frequency operation to an adaptive PI controller with thermal derating.
4. **Comprehensive Testing** — EMC conducted emissions testing (CISPR 25 setup), safety tests, LLC gain & tank characterization, and full-load efficiency measurements.

This repository serves as an **effective introductory platform** for understanding the practical and industrial challenges associated with power electronics design.

---

## Key Specifications

| Parameter               | Value                                                  |
| ----------------------- | ------------------------------------------------------ |
| **Topology**            | LLC Resonant (Half-Bridge)                             |
| **Input Voltage**       | 360 – 400 V DC                                         |
| **Output Voltage**      | 42 – 54 V DC (nominal 48 V)                            |
| **Rated Output Power**  | 1200 W                                                 |
| **Switching Frequency** | 95 kHz – 150 kHz (nominal 100 kHz)                     |
| **Microcontroller**     | STM32F103C8T6 (ARM Cortex-M3, 72 MHz)                  |
| **Control Method**      | Digital PI with adaptive step-delay & thermal derating |
| **Dead Time**           | ~100 ns (configurable via DTG register)                |
| **Gate Driver Logic**   | UCC21520Q                                              |
| **EMI Standard**        | CISPR 25 conducted emissions setup                     |
| **PCB Version**         | V5 (latest)                                            |

---

## Repository Structure

```
Design_of_1200W_LLC_DCDC/
│
├── 01_specifications/             # System-level specifications
├── 02_architecture/               # High-level architecture
│
├── 03_hw_design/                  # ── Hardware Design ──────────────────
│   ├── 01_design_justification/   # Parametric analysis & calculations
│   │   ├── 01_llc_tank/           #   Resonant tank optimization (Jupyter)
│   │   ├── 01_transformer_and_Lr_cores/  # Magnetic core selection
│   │   ├── 02_mosfets/            #   MOSFET thermal & switching analysis
│   │   ├── 03_resonant_and_output_capacitor/  # Capacitor design
│   │   ├── 04_measurements/       #   Sensor calibration & design
│   │   ├── 05_gate_driver/        #   Gate drive circuit design
│   │   ├── 06_emi_filters/        #   EMI filter analysis & justification
│   │   ├── 07_power_supp/         #   Auxiliary power supply
│   │   ├── 08_HeatSink/           #   Thermal management calculations
│   │   ├── 09_Hw_protection/      #   OVP/OCP protection design
│   │   └── 10_cb_calculations/    #   PCB trace & via sizing
│   ├── 02_schematic_and_bom/      # KiCad schematics, BOM, Gerber files
│   └── 03_components/             # Datasheets library (129 components)
│
├── 04_sw_design/                  # ── Firmware & Software ──────────────
│   ├── 01_investigations/         # STM32 & ESP32 bring-up experiments
│   ├── 04_PWM_Freq_Change/        # Switching frequency modulation tests
│   ├── 05_Analog_ADC/             # ADC calibration & serial acquisition
│   ├── 06_Analog_ADC_TempInterp*/ # Temperature interpolation (V1–V4)
│   ├── CONTROL/                   # Main control firmware (V1–V21)
│   ├── new_control/               # Latest firmware (V22–V23, adaptive PI)
│   └── soft_start/                # Soft-start sequence analysis
│
├── 05_validations/                # ── Testing & Validation ─────────────
│   ├── 02_emc/                    # EMC conducted emissions (CISPR 25)
│   ├── 03_safety/                 # Safety & insulation tests
│   ├── 04_driver_test/            # Gate driver validation
│   ├── 05_Gain_and_LLC_Tank_charac/ # LLC gain & impedance characterization
│   ├── 06_Fly_Back/               # Auxiliary flyback validation
│   ├── 07_IP_controller/          # Controller stability & transient tests
│   ├── 07_Prim_sec_capa/          # Primary/secondary capacitor validation
│   ├── 09_LLC_Efficiency/         # Full-load efficiency measurement
│   ├── 10_Buck_converter/         # Output buck converter testing
│   ├── 11_HwProtection/           # Protection circuit trip-point tests
│   ├── 12_Divers/                 # Miscellaneous characterization
│   └── soft_start/                # Soft-start waveform validation
│
├── 99_archive/                    # Legacy code & utilities
└── to_be_orgnized/                # Draft simulations & preliminary work
```

---

## Hardware Design

All hardware design decisions are **fully justified** with reproducible calculations in Jupyter notebooks, supported by LTspice simulations and component datasheets.

### LLC Resonant Tank

The resonant tank parameters (resonant frequency, inductance ratio $L_n = L_m / L_r$, quality factor $Q$) were optimized to achieve the target voltage gain across the full input voltage range (360–400 V) while maintaining zero-voltage switching (ZVS).

> **Design notebook:** [`03_hw_design/01_design_justification/01_llc_tank/`](03_hw_design/01_design_justification/01_llc_tank/)
> 
> **Design report (PDF):** [LLC Tank Design Note](https://boab235.github.io/pages/llc/pdfs/01_LLC_48v_LLC_tank_design_note_v2.pdf)

### Magnetic Components

Planar magnetic cores were selected for both the transformer ($L_m$) and resonant inductor ($L_r$), optimizing for core losses, winding AC resistance, and thermal constraints at the rated operating frequency.

> **Design notebook:** [`03_hw_design/01_design_justification/01_transformer_and_Lr_cores/`](03_hw_design/01_design_justification/01_transformer_and_Lr_cores/)
> 
> **Design report (PDF):** [Magnetic Cores Design](https://boab235.github.io/pages/llc/pdfs/01_LLC_48v_LLC_Magnetic_Cores_V3.pdf)

### MOSFET Selection

Primary-side 600 V class MOSFETs (SPW17N80C3, STW48N60DM2, SIHR080N60E evaluated) were selected based on switching loss analysis, $R_{DS(on)}$ thermal budgets, and snubber design calculations.

> **Design notebook:** [`03_hw_design/01_design_justification/02_mosfets/`](03_hw_design/01_design_justification/02_mosfets/)

### Capacitor Design

Output and resonant capacitor selection was based on ripple voltage requirements, RMS current stress, voltage derating, and ESR thermal limits.

> **Design notebook:** [`03_hw_design/01_design_justification/03_resonant_and_output_capacitor/`](03_hw_design/01_design_justification/03_resonant_and_output_capacitor/)
> 
> **Design report (PDF):** [Output Capacitor Design](https://boab235.github.io/pages/llc/pdfs/03_LLC_48v_LLC_output_capa_v4.pdf)

### EMI Filter

A two-stage common-mode / differential-mode EMI filter was designed:

| Component         | Value                           | Purpose               |
| ----------------- | ------------------------------- | --------------------- |
| Common-mode choke | PA2750NL (400 µH)               | CM noise attenuation  |
| Y capacitors      | 2 × 2.2 nF (before & after CMC) | CM return path        |
| X capacitor       | 100 nF                          | DM filtering          |
| Damper network    | 27 Ω + 2.2 nF                   | Resonance suppression |

CM attenuation grows from ~377 Ω reactance at 150 kHz to ~25 kΩ at 10 MHz. DM LC cutoff at ~425 kHz provides strong harmonic suppression.

> **Design report:** [`03_hw_design/01_design_justification/06_emi_filters/justification.md`](03_hw_design/01_design_justification/06_emi_filters/justification.md)

### Gate Driver & Protection

- **Gate driver** with bootstrap capacitor and level-shifting circuit
- **Hardware protection** using 74HC74 D flip-flop logic for fast OVP/OCP shutdown
- **Voltage & current sensors** with calibrated ADC feedback

> **Design files:** [`05_gate_driver/`](03_hw_design/01_design_justification/05_gate_driver/) · [`09_Hw_protection/`](03_hw_design/01_design_justification/09_Hw_protection/)

### Schematic & PCB

The PCB was designed in **KiCad** with careful attention to high-current trace widths, via thermal management, and copper weight optimization (0.5 oz – 2 oz options evaluated).

| Document                 | Link                                                                                        |
| ------------------------ | ------------------------------------------------------------------------------------------- |
| **Schematic (PDF)**      | [LLC_DCDC_V5](https://boab235.github.io/pages/llc/pdfs/LLC_DCDC_V5.pdf)                     |
| **Gerber & Layout**      | [Gerber PDF](https://boab235.github.io/pages/llc/pdfs/LLC_DCDC_V5_Gerber_and_layout.pdf)    |
| **Bill of Materials**    | [BOM PDF](https://boab235.github.io/pages/llc/pdfs/LLC_DCDC_V5_BOM.pdf)                     |
| **Design Modifications** | [Modifications PDF](https://boab235.github.io/pages/llc/pdfs/LLC_DCDC_V5_Modifications.pdf) |

> **Source files:** [`03_hw_design/02_schematic_and_bom/`](03_hw_design/02_schematic_and_bom/)

---

## Firmware

### Architecture

The firmware runs on an **STM32F103C8T6** (Blue Pill) with **bare-metal register-level** programming (no HAL) for deterministic real-time performance:

```
┌──────────────────────────────────────────────┐
│  STM32F103C8T6 @ 72 MHz                     │
│                                              │
│  TIM1_CH2  (PA9)  ──► High-side gate drive  │
│  TIM1_CH2N (PB14) ──► Low-side gate drive   │
│  Dead-time: DTG=5  (~70 ns)                 │
│                                              │
│  ADC1 (PA0–PA7)   ◄── V_in, V_out, I_sense │
│  GPIO (PB10/PB12) ──► Flip-flop enable/rst  │
│  TIM2 (PA10)      ──► Fan PWM control       │
│                                              │
│  UART (CH340)     ◄─► Serial monitoring      │
└──────────────────────────────────────────────┘
```

### Control Strategy

The firmware went through **21+ iterations** of increasing sophistication:

| Phase              | Versions      | Strategy                                      |
| ------------------ | ------------- | --------------------------------------------- |
| Fixed frequency    | V1 – V11      | Open-loop constant-frequency switching        |
| PI controller      | V12 – V16     | Closed-loop PI voltage regulation             |
| Variable frequency | V17 – V19     | Frequency modulation (period control)         |
| IP controller      | V20 – V21     | Alternative controller topology               |
| **Adaptive PI**    | **V22 – V23** | **Adaptive step-delay with thermal derating** |

The latest firmware (`PI_V23_adaptive_step_delay_derating`) implements:

- **Proportional-Integral** voltage regulation
- **Adaptive step delay** for transient response optimization
- **Thermal derating** — output power automatically reduced at elevated temperatures
- **Soft-start** sequence for controlled startup

> **Latest firmware:** [`04_sw_design/new_control/PI_V23_adaptive_step_delay_derating/`](04_sw_design/new_control/PI_V23_adaptive_step_delay_derating/)
> 
> **Control firmware archive (V1–V21):** [`04_sw_design/CONTROL/`](04_sw_design/CONTROL/)

### Soft Start

The soft-start strategy was designed and validated through LTspice simulation. The converter ramps switching frequency from a high initial value down to the target operating frequency, limiting inrush current and ensuring stable resonant tank excitation.

> **Analysis:** [`04_sw_design/soft_start/`](04_sw_design/soft_start/)

---

## Validation & Testing

The converter was put through a comprehensive validation campaign across 12 test categories:

### Assembly-Level Tests

| Test                        | Description                                          | Report                                                                                     |
| --------------------------- | ---------------------------------------------------- | ------------------------------------------------------------------------------------------ |
| **EMC Conducted Emissions** | CISPR 25 LISN setup, conducted emissions measurement | [PDF Report](https://boab235.github.io/pages/llc/pdfs/EMC_Conducted_Emissions_Test_V4.pdf) |
| **Efficiency**              | Full-load efficiency mapping and loss breakdown      | [PDF Report](https://boab235.github.io/pages/llc/pdfs/LLC_1200W_Efficiency_test.pdf)       |
| **Safety Tests**            | Capacitor discharge test & insulation test           | [PDF Report](https://boab235.github.io/pages/llc/pdfs/LLC_Safety_tests_V0.pdf)             |

### Subsystem-Level Tests

| Test                             | Description                                                     | Report                                                                                       |
| -------------------------------- | --------------------------------------------------------------- | -------------------------------------------------------------------------------------------- |
| **Gain & Tank Characterization** | LLC frequency response, $L_r$ and $L_m$ measurement (4 methods) | [PDF Report](https://boab235.github.io/pages/llc/pdfs/Gain_and_tank_characterization_V0.pdf) |
| **Gate Driver Validation**       | Bootstrap capacitor transient & dead-time verification          | [`05_validations/04_driver_test/`](05_validations/04_driver_test/)                           |
| **Controller Stability**         | PI transient response & stability analysis                      | [`05_validations/07_IP_controller/`](05_validations/07_IP_controller/)                       |
| **HW Protection**                | OCP/OVP trip-point characterization                             | [`05_validations/11_HwProtection/`](05_validations/11_HwProtection/)                         |
| **Soft-Start Validation**        | Inrush current & startup waveform capture                       | [`05_validations/soft_start/`](05_validations/soft_start/)                                   |

> **All test data:** [`05_validations/`](05_validations/)

---

## Tools & Technologies

| Category                | Tools                                                                 |
| ----------------------- | --------------------------------------------------------------------- |
| **Schematic & PCB**     | KiCad                                                                 |
| **Circuit Simulation**  | LTspice                                                               |
| **Design Calculations** | Python 3 · Jupyter Notebooks · NumPy · SciPy · Matplotlib · handcalcs |
| **Firmware IDE**        | Arduino IDE with STM32 core (bare-metal register access)              |
| **Microcontroller**     | STM32F103C8T6 (primary) · ESP32 (evaluated)                           |
| **USB Interface**       | CH340 USB-to-UART @ 115200 baud                                       |
| **Data Analysis**       | Pandas · Matplotlib                                                   |
| **PCB Fabrication**     | JLC PCB / PCBWay                                                      |

---

## Getting Started

### Browsing the Design

1. Start with the [**LLC Tank Design Note**](03_hw_design/01_design_justification/01_llc_tank/01_LLC_48v_LLC_tank_design_note_v2.ipynb) to understand the resonant converter theory and parameter choices.
2. Review the [**Schematic (PDF)**](https://boab235.github.io/pages/llc/pdfs/LLC_DCDC_V5.pdf) for the full circuit.
3. Check the [**BOM**](https://boab235.github.io/pages/llc/pdfs/LLC_DCDC_V5_BOM.pdf) for component sourcing.

### Building the Firmware

1. Install the [Arduino IDE](https://www.arduino.cc/en/software) with the **STM32duino** board package.

2. Select board: **Generic STM32F1 series → STM32F103C8**.

3. Open the latest firmware:
   
   ```
   04_sw_design/new_control/PI_V23_adaptive_step_delay_derating/PI_V23_adaptive_step_delay_derating.ino
   ```

4. Connect the STM32 Blue Pill via ST-Link or USB-to-Serial (CH340).

5. Compile and upload.

---

## Project Page

An **interactive project page** with documentation, downloadable PDFs, and a photo gallery is available at:

**[https://boab235.github.io/pages/llc/llc.html](https://boab235.github.io/pages/llc/llc.html)**

---

## License

This project is open-source. See the repository for license details.

<div align="center">

**[Report an Issue](https://github.com/BOAB235/Design_of_1200W_LLC_DCDC/issues) · [Project Page](https://boab235.github.io/pages/llc/llc.html) · [GitHub](https://github.com/BOAB235/Design_of_1200W_LLC_DCDC)**

</div>