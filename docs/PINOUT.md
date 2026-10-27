# Pinout

## UART Debug (USART2)

| Signal | Pin | Peripheral / AF | Connects to |
|---|---|---|---|
| USART2_TX | PA2 | USART2, AF07 | STLINK-V3SET(MB1440B) CN3 Pin 1 T-VCP-TX(1) |
| USART2_RX | PA3 | USART2, AF07 | STLINK-V3SET(MB1440B) CN3 Pin 2 T-VCP-RX(2) |

1. Input for STLINK-V3SET. Must be connected to UART_TX on target.
2. Output for STLINK-V3SET. Must be connected to UART_RX on target.


## I2C (MPU-6050)

| Signal | Pin | Peripheral / AF | Connects to |
|---|---|---|---|
| I2C1_SCL  | PB6 | I2C1, AF04   | MPU6050 SCL |
| I2C1_SDA  | PB7 | I2C1, AF04   | MPU6050 SDA |

