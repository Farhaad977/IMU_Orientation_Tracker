# Hardware

## Parts

| Part | Exact Model | Interface | 
|---|---|---|
| Microcontroller Board | DFR0864  STM32 Black Pill(STM32F411CE) WeAct Studio v3.1 | USB-C, SWD |
| MPU-6050 | 3886  Adafruit MPU-6050 6-DoF | I2C | 
| Programmer/Debugger | STLINK-V3SET | SWD, USB 2.0 | 
| OLED Display | DFR0650  Monochrome 0.96'' 128x64 OLED Display | I2C, SPI|
| Logic Analyzer | Kingst Logic Analyzer LA2016 | USB |
| Jumper Wires and USB Cable | - | - |
 
## Datasheets and documents

- MPU-6050 Datasheet: [Datasheet PDF](https://product.tdk.com/system/files/dam/doc/product/sensor/mortion-inertial/imu/data_sheet/mpu-6000-datasheet1.pdf)
- MPU-6050 Register Map: [Register Map PDF](https://d17t6iyxenbwp1.cloudfront.net/s3fs-public/2026-04/rm-mpu-6000a-00-mpu-6000-register-map.pdf?VersionId=jEYAYvYOKeDIa.5IJgZPIlWgNTRCs.9m)
- STM32F411 Datasheet: [STM32F411 Datasheet](https://www.st.com/resource/en/datasheet/stm32f411ce.pdf)
- STM32F411 Reference Manual: [STM32F411 Reference Manual](https://www.st.com/resource/en/reference_manual/rm0383-stm32f411xce-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- SSD1306 Datasheet: [OLED Datasheet](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)
- STM32F411 Black Pill Development Board(Schematic and Pinout): [WeAct Black Pill v3.1 Schematic](https://github.com/WeActStudio/WeActStudio.MiniSTM32F4x1/blob/master/Hardware/MiniF4x1Cx_V31%20SchDoc.pdf)
- DFRobot DFR0650 OLED Page: [OLED Info](https://wiki.dfrobot.com/dfr0650/)
- STLINK-V3SET User Manual: [STLINK V3-SET UM PDF](https://www.st.com/resource/en/user_manual/um2448-stlinkv3set-debuggerprogrammer-for-stm8-and-stm32-stmicroelectronics.pdf)

## Board Check

| Item | Expected | 
|---|---|
| LED Pin | PC13 |
| User Button Pin | PA0 |
| Crystal | 25 MHz |
| SWD Pins | SWDIO = PA13, SWCLK = PA14, plus GND |
| 3V3 Pin | On the Headers and SWD Header |
