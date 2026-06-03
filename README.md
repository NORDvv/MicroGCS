# MicroGCS

**MicroGCS** is a miniature ground-control station prototype built with an STM32 microcontroller and a Qt6 desktop application.

The project demonstrates a complete hardware/software communication loop:

```text
STM32 simulated vehicle → UART telemetry → Qt ground station
Qt command controls     → UART command   → STM32 vehicle state update
STM32 command response  → ACK/ERR         → Qt event log
```

The goal is not to build a real UAV stack.
The goal is to demonstrate practical ground-control software concepts: telemetry parsing, command/response workflows, serial communication, event logging, stale-link detection, and robustness across a hardware/software boundary.

---

## Project Overview

MicroGCS consists of two main parts:

### 1. STM32 Vehicle Simulator

The STM32 firmware simulates a small vehicle/drone-like system. It maintains internal vehicle state and periodically sends telemetry over UART.

Simulated state includes:

* armed/disarmed state
* flight/control mode
* battery percentage
* altitude
* x/y position
* waypoint index
* health/status string

The firmware also receives commands from the desktop app and replies with `ACK` or `ERR` responses.

### 2. Qt6 Ground Station

The Qt desktop app acts as the ground-control station. It connects to the STM32 over a USB-UART adapter, displays live telemetry, sends operator commands, monitors telemetry health, and records a session log.

The UI includes:

* serial connection controls
* live vehicle state display
* arm/disarm and mode controls
* command acknowledgement display
* telemetry health monitoring
* event log window
* CSV session logging

---

## Current Features

### Firmware

* STM32F103C8T6 firmware written in C
* UART telemetry output
* UART command receive path using interrupt-driven byte reception
* simulated vehicle state model
* command parser for:

  * `ARM`
  * `DISARM`
  * `MODE=MANUAL`
  * `MODE=AUTO`
  * `MODE=RTL`
  * `RESET`
* ACK/ERR command responses
* safe default state: vehicle starts disarmed

### Desktop App

* Qt6/C++ desktop ground station
* serial port discovery and connection
* live telemetry receive and parsing
* dashboard display for vehicle state
* operator controls for arm/disarm, mode selection, and reset
* command timeout detection
* ACK/ERR display
* telemetry packet counter
* last telemetry timestamp
* stale telemetry detection
* CSV session logging
* diagnostic event log

---

## Hardware Used

* STM32F103C8T6 “Blue Pill” board
* ST-LINK V2 programmer/debugger
* USB-to-UART adapter
* jumper wires
* Linux development machine

UART wiring:

```text
STM32 PA9  / USART1_TX → USB-UART RXD
STM32 PA10 / USART1_RX ← USB-UART TXD
STM32 GND              ↔ USB-UART GND
```

ST-LINK wiring:

```text
ST-LINK SWDIO → STM32 SWDIO
ST-LINK SWCLK → STM32 SWCLK
ST-LINK GND   → STM32 GND
ST-LINK 3.3V  → STM32 3.3V reference
```

---

## System Architecture

```text
┌──────────────────────────────┐
│       Qt6 Ground Station     │
│                              │
│  - Serial connection         │
│  - Telemetry parser          │
│  - Vehicle state display     │
│  - Command controls          │
│  - ACK/ERR handling          │
│  - Telemetry health monitor  │
│  - Session logger            │
└───────────────┬──────────────┘
                │
                │ UART / USB serial
                │
┌───────────────v──────────────┐
│      STM32 Vehicle Simulator │
│                              │
│  - VehicleState              │
│  - TelemetryGenerator        │
│  - CommandParser             │
│  - UART TX/RX                │
└──────────────────────────────┘
```

---

## Protocol

MicroGCS uses a simple newline-terminated text protocol.

### Telemetry Packet

Example:

```text
TEL;armed=1;mode=AUTO;batt=98.8;alt=10.0;x=22.4;y=11.2;wp=3;status=OK
```

Fields:

| Field    | Meaning                      |
| -------- | ---------------------------- |
| `armed`  | `0` or `1`                   |
| `mode`   | `MANUAL`, `AUTO`, or `RTL`   |
| `batt`   | simulated battery percentage |
| `alt`    | simulated altitude           |
| `x`      | simulated x position         |
| `y`      | simulated y position         |
| `wp`     | current waypoint index       |
| `status` | vehicle status string        |

### Commands

Examples:

```text
CMD;ARM
CMD;DISARM
CMD;MODE=MANUAL
CMD;MODE=AUTO
CMD;MODE=RTL
CMD;RESET
```

### ACK Responses

Examples:

```text
ACK;cmd=ARM;result=OK
ACK;cmd=MODE;result=OK;mode=AUTO
ACK;cmd=RESET;result=OK
```

### ERR Responses

Examples:

```text
ERR;cmd=MODE;reason=INVALID_MODE
ERR;cmd=UNKNOWN;reason=INVALID_COMMAND
ERR;cmd=UNKNOWN;reason=INVALID_ARGUMENT
```

---

## Telemetry Health Monitoring

The ground station tracks telemetry freshness.

It displays:

* telemetry state: OK, waiting, stale, or disconnected
* last telemetry timestamp
* telemetry age in milliseconds
* packet count

If telemetry stops arriving for more than the configured timeout, the UI marks the link as stale and logs a warning.

This is useful for demonstrating how an operator station can detect link degradation instead of silently showing old data.

---

## Session Logging

MicroGCS writes a CSV log for each app session.

The log records:

* application start/stop
* serial connect/disconnect events
* transmitted commands
* received telemetry
* received ACK/ERR responses
* parse errors
* telemetry stale/recovered events
* command timeouts

Example:

```csv
timestamp,direction,type,payload
2026-06-02T21:54:12.123,SYS,START,"MicroGCS session started"
2026-06-02T21:54:14.200,SYS,CONNECT,"Connected to ttyUSB0 at 115200"
2026-06-02T21:54:15.000,RX,TEL,"TEL;armed=0;mode=MANUAL;batt=99.9;alt=0.0;x=0.0;y=0.0;wp=0;status=OK"
2026-06-02T21:54:16.100,TX,CMD,"CMD;ARM"
2026-06-02T21:54:16.130,RX,ACK,"ACK;cmd=ARM;result=OK"
2026-06-02T21:54:19.000,SYS,WARN,"Telemetry stale, age_ms=2300"
```

---

## Build and Run

### Firmware

The firmware is generated from an STM32CubeMX/CMake project.

Typical workflow:

1. Open the STM32 firmware project.
2. Configure USART1 for asynchronous UART at 115200 baud.
3. Build the firmware.
4. Flash the STM32 using ST-LINK.
5. Connect the USB-UART adapter to PA9/PA10/GND.

### Ground Station

The desktop app is built with Qt6 and CMake.

Required Qt components:

* Qt6 Widgets
* Qt6 SerialPort

Typical workflow:

```bash
cmake -S . -B build
cmake --build build
```

Then run the generated `MicroGCS` executable.

---

## Demo Flow

A typical demo sequence:

1. Start the STM32 firmware.
2. Launch the MicroGCS Qt application.
3. Select the USB-UART port.
4. Connect to the vehicle simulator.
5. Show live telemetry updating.
6. Arm the vehicle.
7. Observe ACK response and telemetry state change.
8. Switch to AUTO mode.
9. Observe simulated movement.
10. Switch to RTL mode.
11. Observe the vehicle return toward the origin before descending.
12. Send RESET.
13. Show session log file.
14. Optionally disconnect telemetry and show stale-link detection.

---

## Robustness Testing

During development, the system was tested against several failure cases:

* disconnected UART link
* corrupted serial input during reconnect
* malformed telemetry packets
* invalid commands
* missing command acknowledgements
* stale telemetry

Observed behavior:

* Qt app does not crash on corrupted telemetry.
* Bad packets are logged as parse errors or unknown input.
* Missing telemetry causes the link health indicator to become stale.
* Missing ACK/ERR responses trigger a command timeout warning.
* Session logs make failure cases easier to diagnose.

---

## Known Limitations

This is a prototype, not a production UAV control system.

Known limitations:

* protocol has no checksum or CRC
* no packet sequence numbers
* no authentication or encryption
* no real flight control
* no real GPS or sensor input
* no persistent vehicle state across MCU reset
* no automatic reconnect
* simple line-based text protocol
* vehicle dynamics are simulated

These limitations are intentional for the current project scope. The focus is on telemetry, command handling, UI responsiveness, logging, and robustness across a device/software boundary.

---

## Future Improvements

Possible future upgrades:

* packet sequence numbers
* checksum/CRC validation
* automatic reconnect
* richer mission/waypoint panel
* telemetry graphing
* log replay mode
* USB CDC support
* multiple simulated vehicles
* more structured binary protocol
* unit tests for telemetry and command parsing
* hardware watchdog/reset diagnostics

---

## Development Notes

A major debugging lesson from the project was that hardware bring-up issues are often physical rather than logical.

During UART bring-up, the firmware, clock configuration, ST-LINK flashing, and USB-UART adapter were verified independently. The final issue was traced to unreliable jumper-wire contact. This reinforced the importance of validating the physical layer before debugging higher-level firmware or protocol logic.

---

## One-Sentence Summary

MicroGCS is a small STM32 + Qt ground-control prototype that demonstrates live telemetry, command/response control, ACK/ERR handling, stale-link detection, and session logging across a hardware/software boundary.
