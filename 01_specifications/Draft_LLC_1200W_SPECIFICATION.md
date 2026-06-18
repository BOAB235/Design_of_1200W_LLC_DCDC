# Specification — 1200 W LLC Resonant DC-DC Converter

| Field           | Value            |
| --------------- | ---------------- |
| **Document ID** | SPEC-LLC1200-001 |
| **Revision**    | A                |
| **Date**        | 2026-04-02       |
| **Status**      | Released         |
| **Author**      | —                |

---

## 1. Purpose & Scope

This document defines the electrical, mechanical, environmental, and functional requirements for a **1 200 W isolated DC-DC converter** that steps the high-voltage traction-battery bus of an electric vehicle down to the 48 V auxiliary rail. It covers the converter as a standalone assembly (power board + control board) and does not cover the vehicle-level wiring harness or battery-management system interface.

---

## 2. System Context

```
┌──────────────┐          ┌─────────────────────┐          ┌──────────────┐
│  HV Traction │  360-400 │   1200 W LLC DC-DC  │  42-54 V │  48 V Aux    │
│  Battery     │───V DC──▶│   Converter          │────DC───▶│  Bus (loads) │
│  (≈ 96S Li)  │          │   (this product)     │          │              │
└──────────────┘          └─────────────────────┘          └──────────────┘
                                    │
                                    │ UART telemetry (debug)
                                    ▼
                              Host / Logger
```

**Typical 48 V loads in an EV:** electric power steering, HVAC blower, headlamps (LED matrix), infotainment head-unit, ADAS radar/camera, body-control modules, seat heaters, DC-DC pre-charge relay coils.

---

## 3. Applicable Standards & References

| Standard / Document   | Relevance                                                         |
| --------------------- | ----------------------------------------------------------------- |
| **CISPR 25** (Ed. 5)  | Conducted & radiated emissions limits for vehicles                |
| **ISO 16750-2**       | Road vehicles — Environmental conditions — Electrical loads       |
| **ISO 16750-4**       | Road vehicles — Environmental conditions — Climatic loads         |
| **IEC 60664-1**       | Insulation coordination — Creepage & clearance                    |
| **IEC 61558-2-16**    | Safety of power transformers for switch-mode power supplies       |
| **LV 124** (OEM ref.) | Electrical & environmental requirements for automotive components |
| **AEC-Q101**          | Qualification of discrete semiconductors (MOSFET reliability)     |

> *Note — Full standard compliance is a design target; formal certification is outside the scope of this open-source project.*

---

## 4. Electrical Specifications

### 4.1 Input (High-Voltage Side)

| Parameter                    | Min | Nom | Max | Unit | Remark                                |
| ---------------------------- | --- | --- | --- | ---- | ------------------------------------- |
| Input voltage                | 360 | 380 | 400 | V DC | Traction battery range (≈ 96S Li-ion) |
| Input surge (< 1 s)          | —   | —   | 420 | V DC | Transient per ISO 16750-2             |
| Input current (steady-state) | —   | 3.3 | 3.6 | A DC | At rated load and η ≥ 95 %            |
| Input under-voltage lockout  | 340 | —   | —   | V DC | Converter shuts down safely           |

### 4.2 Output (Low-Voltage Side)

| Parameter                               | Min | Nom   | Max   | Unit | Remark                            |
| --------------------------------------- | --- | ----- | ----- | ---- | --------------------------------- |
| Output voltage (regulated)              | 42  | 48    | 54    | V DC | Covers 48 V Li-ion charge profile |
| Output voltage accuracy                 | —   | —     | ±3    | %    | Steady-state at 25 °C             |
| Output voltage ripple (pk-pk)           | —   | —     | 250   | mV   | At full load, 100 kHz BW          |
| Output current (continuous)             | 0   | 25    | 28    | A    | 110 % overload for < 60 s         |
| Output power (continuous)               | —   | 1 200 | —     | W    |                                   |
| Output power (overload, 60 s)           | —   | —     | 1 320 | W    | Thermal derating may apply        |
| Transient response (50–100 % load step) | —   | —     | 5     | % V  | Peak excursion, recovery < 5 ms   |

### 4.3 Efficiency

| Condition                               | Min | Typ | Unit |
| --------------------------------------- | --- | --- | ---- |
| 100 % load, V_in = 380 V, T_amb = 25 °C | 94  | 95  | %    |
| 50 % load, V_in = 380 V, T_amb = 25 °C  | 93  | 95  | %    |
| 25 % load                               | 88  | 91  | %    |

### 4.4 Isolation

| Parameter                       | Value | Unit | Remark                           |
| ------------------------------- | ----- | ---- | -------------------------------- |
| Galvanic isolation              | Yes   | —    | Primary-to-secondary transformer |
| Hi-pot test (type test, 1 min)  | 2 500 | V AC | Primary ↔ secondary              |
| Working isolation voltage       | 800   | V pk | Per IEC 60664-1                  |
| Creepage (primary ↔ secondary)  | ≥ 8   | mm   | On PCB, per pollution degree 2   |
| Clearance (primary ↔ secondary) | ≥ 5.5 | mm   |                                  |

---

## 5. Functional Requirements

### 5.1 Topology

Half-bridge **LLC series-resonant** converter with synchronous rectification on the secondary. Frequency-modulated control at near-50 % duty cycle to maintain ZVS across the operating range.

### 5.2 Resonant Tank

| Parameter                            | Value               | Unit |
| ------------------------------------ | ------------------- | ---- |
| Resonant frequency ($f_0$)           | ≈ 100               | kHz  |
| Magnetising inductance ($L_m$)       | ≈ 65                | µH   |
| Transformer turns ratio ($n$)        | 4:1                 | —    |
| Switching frequency range            | 95 – 150            | kHz  |
| Inductance ratio ($L_n = L_m / L_r$) | per design notebook | —    |

### 5.3 Control

| Feature                   | Requirement                                            |
| ------------------------- | ------------------------------------------------------ |
| Regulation method         | Digital PI (proportional-integral) frequency control   |
| Regulation loop bandwidth | > 1 kHz                                                |
| ADC sampling              | 12-bit, 8-channel, ≈ 60 kHz loop rate                  |
| Soft-start                | Frequency ramp from f_max → f_nom, limiting inrush     |
| Thermal derating          | Automatic power roll-back when heat-sink T > threshold |
| Dead-time                 | Configurable via DTG register, nominal ≈ 100 ns        |

### 5.4 Protection

| Protection                 | Method                                    | Threshold                |
| -------------------------- | ----------------------------------------- | ------------------------ |
| Output over-voltage (OVP)  | HW latch (74HC74) + FW monitor            | > 81 V (1.5 × V_out,max) |
| Output under-voltage (UVP) | FW monitor                                | < 36 V                   |
| Over-current (OCP)         | HW latch (74HC74) + current sensor        | > 30 A output            |
| Over-temperature (OTP)     | FW — thermistor on heat-sink              | > 95 °C heat-sink        |
| Input under-voltage        | FW monitor                                | < 340 V                  |
| Short-circuit              | OCP + hiccup restart (frequency sweep up) | —                        |

HW protection response time: **< 500 ns** (flip-flop latch directly disables gate driver).

---

## 6. EMC Requirements

| Parameter                              | Limit   | Standard |
| -------------------------------------- | ------- | -------- |
| Conducted emissions (150 kHz – 30 MHz) | Class 5 | CISPR 25 |
| Radiated emissions (30 – 1 000 MHz)    | Class 5 | CISPR 25 |

### 6.1 EMI Filter

| Component             | Value            | Purpose               |
| --------------------- | ---------------- | --------------------- |
| Common-mode choke     | PA2750NL, 400 µH | CM attenuation        |
| Y capacitors          | 4 × 2.2 nF       | CM return path        |
| X capacitor           | 100 nF           | DM filtering          |
| Damper                | 27 Ω + 2.2 nF    | Resonance suppression |
| DM leakage inductance | 1.4 µH           | Series DM element     |

---

## 7. Mechanical & Thermal

### 7.1 Form Factor

| Parameter                    | Value                        | Unit |
| ---------------------------- | ---------------------------- | ---- |
| PCB outline (approx.)        | 180 × 120                    | mm   |
| PCB layers                   | 4 (min)                      | —    |
| Copper weight (power layers) | 2                            | oz   |
| Primary connector            | Screw terminal / Anderson PP | —    |
| Secondary connector          | Screw terminal / busbar      | —    |
| Mounting                     | 4 × M3 standoffs             | —    |

### 7.2 Thermal

| Parameter                     | Value                    | Unit | Remark                        |
| ----------------------------- | ------------------------ | ---- | ----------------------------- |
| Ambient operating temperature | −20 to +65               | °C   | Automotive under-hood lite    |
| Storage temperature           | −40 to +85               | °C   |                               |
| Heat-sink type                | Extruded aluminium + fan | —    | Fan PWM controlled by MCU     |
| Max junction temp (MOSFETs)   | 150                      | °C   | Per datasheet absolute max    |
| Thermal derating start        | 85                       | °C   | Heat-sink surface temperature |
| Full shutdown                 | 105                      | °C   | Heat-sink surface temperature |

### 7.3 Cooling

Forced-air cooling via 12 V brushless DC fan, PWM-modulated by the STM32 (TIM2, PA10). Fan speed scales with measured heat-sink temperature.

---

## 8. Component Summary

| Function              | Part Number        | Qty   | Key Parameter              |
| --------------------- | ------------------ | ----- | -------------------------- |
| Primary MOSFET        | IPW60R037CM8XKSA1  | 2     | 600 V, 3.7 mΩ, TO-247      |
| Secondary MOSFET      | FDH055N15A         | 2     | 150 V, 110 A, TO-247       |
| Gate driver           | UCC21520Q          | 1     | Isolated, ±15 V output     |
| Transformer core      | TDK N97 planar     | 1 set | Optimised for 100 kHz      |
| Output capacitor      | B40910A8127M000    | 6     | 120 µF, 150 °C, ESR 17 mΩ  |
| CM choke              | PA2750NL           | 1     | 400 µH, 1.4 µH leakage     |
| Current sensor        | ACS70331EOLCTR-005 | 1     | Hall-effect, isolated      |
| MCU                   | STM32F103C8T6      | 1     | Cortex-M3, 72 MHz          |
| Aux supply (flyback)  | TNY280GNTL         | 1     | Isolated ±15 V for drivers |
| Aux supply (LV)       | LM2575             | 1     | 3.3 V / 5 V for MCU        |
| HW latch (protection) | 74HC74             | 1     | D flip-flop fast shutdown  |

---

## 9. Interface Definition

### 9.1 Power Connectors

| Connector       | Pins | Rating            |
| --------------- | ---- | ----------------- |
| HV input (+/−)  | 2    | 400 V / 4 A       |
| LV output (+/−) | 2    | 54 V / 30 A       |
| Chassis GND     | 1    | Safety earth bond |

### 9.2 Signal / Debug

| Interface        | Connector           | Protocol          | Purpose                    |
| ---------------- | ------------------- | ----------------- | -------------------------- |
| UART             | USB micro-B (CH340) | 115 200 baud, 8N1 | Telemetry & debug log      |
| SWD              | 4-pin header        | ARM SWD           | Firmware programming       |
| Fan              | 2-pin header        | 12 V PWM          | Cooling fan control        |
| Protection reset | Push-button         | GPIO              | Manual OVP/OCP latch reset |

---

## 10. Qualification & Test Plan Summary

| ID   | Test                               | Pass Criteria                                    | Reference  |
| ---- | ---------------------------------- | ------------------------------------------------ | ---------- |
| T-01 | Input voltage sweep (340–420 V)    | V_out within ±3 %, no oscillation                | §4.1, §4.2 |
| T-02 | Full-load efficiency               | η ≥ 94 % at 380 V / 1 200 W                      | §4.3       |
| T-03 | Output ripple                      | < 250 mV pp at 25 A                              | §4.2       |
| T-04 | Transient response (50–100 % step) | < 5 % overshoot, < 5 ms settling                 | §4.2       |
| T-05 | OVP trip                           | Shuts down within 500 ns at 81 V                 | §5.4       |
| T-06 | OCP trip                           | Shuts down within 500 ns at 30 A                 | §5.4       |
| T-07 | Soft-start                         | No inrush > 2 × I_nom, monotonic ramp            | §5.3       |
| T-08 | Thermal derating                   | Power reduces above 85 °C hs, shutdown at 105 °C | §7.2       |
| T-09 | Conducted EMC                      | CISPR 25 Class 5                                 | §6         |
| T-10 | Hi-pot                             | 2 500 V AC, 1 min, no breakdown                  | §4.4       |
| T-11 | Thermal cycling                    | −20 °C to +65 °C, 100 cycles, no degradation     | §7.2       |
| T-12 | Continuous run (burn-in)           | 48 h at 100 % load, no drift > 1 %               | §4.2       |

---

## 11. Revisions

| Rev | Date       | Description                                 |
| --- | ---------- | ------------------------------------------- |
| A   | 2026-04-02 | Initial release based on V5 hardware design |

---

*End of specification.*
