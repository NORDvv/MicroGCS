# MicroGCS
A miniature ground-control station prototype using an STM32 vehicle simulator and a Qt desktop app for live telemetry, commands, acknowledgements, and logging.


## Hardware Bring-Up Notes

During initial UART bring-up, the firmware, clock configuration, ST-LINK flashing, and USB-UART adapter were verified independently. The final issue was traced to an intermittent physical connection on the UART jumper wiring. This reinforced the importance of validating the physical layer before debugging higher-level firmware or protocol logic.