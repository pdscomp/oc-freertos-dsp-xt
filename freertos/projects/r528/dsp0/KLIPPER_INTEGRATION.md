# Klipper MCU Firmware Integration

This directory contains a port of the Klipper MCU firmware integrated with FreeRTOS for the Allwinner R528 / Xtensa HiFi4 DSP core.

## Overview

Klipper is a 3D printer firmware that offloads the computation of printer movements to a host computer. The MCU firmware handles real-time operations like:
- Stepper motor control
- Temperature sensor reading
- Endstop monitoring
- PWM control for heaters and fans
- Communication with the host

## Integration Architecture

### Main Components

1. **FreeRTOS Task**: Klipper runs as a single FreeRTOS task created in `src/main.c`
   - Task name: "Klipper"
   - Stack size: 8KB (0x2000 words)
   - Priority: 2 (higher than idle task)

2. **Klipper Scheduler**: The task calls `sched_main()` which:
   - Initializes all Klipper subsystems
   - Sets up the main scheduling loop
   - Handles timer-based events
   - Processes commands from the host

### Directory Structure

```
src/
├── main.c                          # FreeRTOS integration and task creation
├── include/                        # Klipper header files
│   ├── board/                      # Board-specific interfaces
│   ├── bus/                        # Bus communication (msgbox, uart, usb)
│   ├── generic/                    # Generic utilities
│   ├── hal_call/                   # HAL abstraction layer
│   └── printer/                    # Printer-specific logic
└── klipper_r528/                   # Klipper implementation
    ├── board/                      # R528-specific board support
    ├── bus/                        # Communication implementations
    ├── generic/                    # Generic implementations
    ├── hal_call/                   # HAL wrappers
    ├── printer/                    # Printer control logic
    └── ui/                         # UI support (buttons, LCDs)
```

## Build System Integration

### Makefile Changes

The `Makefile` has been updated to include all Klipper object files:

- Bus subsystem (msgbox, uart, usb, shared memory)
- HAL calls (ADC, GPIO, I2C, SPI, PWM, sensors)
- Printer subsystem (scheduler, commands, stepper control)
- UI subsystem (buttons, LCD displays)
- Generic utilities (memory allocation, timers, IRQ handling, CRC)
- Board support (GPIO, ADC, I2C, SPI, PWM, watchdog)

### Dependencies

Klipper depends on:
- **FreeRTOS**: Task management and scheduling
- **HAL (Hardware Abstraction Layer)**: From `rtos-hal/` directory
  - `hal_gpio.h` - GPIO control
  - `sunxi_hal_timer.h` - Hardware timers
  - `sunxi_hal_pwm.h` - PWM control
  - `sunxi_hal_spi.h` - SPI communication
  - `hal_msgbox.h` - Message box for inter-processor communication
  - Additional HAL modules as needed

### Include Paths

The main Makefile already includes the necessary paths:
- `-Iprojects/r528/dsp0/src` - Provides access to Klipper includes
- `-Iprojects/r528/dsp0/include` - FreeRTOS config
- HAL headers from `rtos-hal/include/hal/`

## Changes to Parent Project

### Minimal Changes

As requested, changes to the parent FreeRTOS project, OSAL, and HAL have been minimized:

1. **projects/r528/dsp0/Makefile**: Updated to include Klipper object files
2. **projects/r528/dsp0/src/main.c**: Modified to create Klipper task instead of example tasks
3. **No changes to**: FreeRTOS kernel, OSAL, HAL, drivers, or components

### Rationale

- **main.c modification**: Required to replace the example application with Klipper
- **Makefile update**: Standard practice for adding new source files to build
- **No HAL changes**: Klipper code already uses existing HAL interfaces

## Building the Firmware

### Prerequisites

1. Xtensa toolchain (RI-2020.4-linux or later)
2. Configured build environment (see main README.md)

### Build Steps

```bash
cd freertos
. build/envsetup.sh
env_xcc
make clean
make
make pack
```

The output firmware will be in `out/r528/dsp0.bin`

## Klipper Configuration

### Key Features

- **Real-time Scheduling**: Klipper's scheduler handles time-critical operations
- **Command Interface**: Communicates with host via message box or UART
- **Hardware Abstraction**: Uses R528 HAL for hardware access
- **FreeRTOS Integration**: Runs cooperatively with other FreeRTOS tasks

### Task Behavior

The Klipper task:
1. Initializes all subsystems via `ctr_run_initfuncs()`
2. Allocates message queue for communication
3. Sets up error handling with `setjmp/longjmp`
4. Enters main run loop in `run_tasks()`
5. Should not return under normal operation

If the task returns unexpectedly, an error message is printed and the task deletes itself.

## Notes

- The Klipper code is adapted from the CentauriCarbon project
- Original Klipper is licensed under GPLv3
- Integration maintains separation between FreeRTOS (MIT) and Klipper (GPLv3) code
- This integration allows Klipper to leverage FreeRTOS features while maintaining its real-time characteristics
