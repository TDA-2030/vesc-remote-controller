<!--
 * @Author: zhouli
 * @Date: 2020-04-04 15:26:37
 * @LastEditTime: 2020-04-06 23:45:59
 * @Description: file content
 -->
# VESC Remote Controller

This remote controller is used to control the VESC motor controller. It can easily control the electric skateboard equipped with VESC drive.



| Branch | Build Status                                                 |
| ------ | ------------------------------------------------------------ |
| master | ![Build Status](https://camo.githubusercontent.com/0cce57dd531f9e16706cb1456696e504a9b32d38/68747470733a2f2f7472617669732d63692e6f72672f6d6164636f777377652f4f44726976652e706e673f6272616e63683d6d6173746572) |
| dev    | ![Build Status](https://camo.githubusercontent.com/0cce57dd531f9e16706cb1456696e504a9b32d38/68747470733a2f2f7472617669732d63692e6f72672f6d6164636f777377652f4f44726976652e706e673f6272616e63683d6d6173746572) |

# Feature
- Low-power Consumption
- Support Remote Power Control
- Real-time Data Backhaul
- Show Real-time packet loss rate
- Battery Current Monitoring
- Input Voltage Range of 12 V to 60 V
- Two LED Headlights
- Reserved UART to Connect Modules such as GPS
- Acceleration Sensor MPU6050

# Repository Structure

- **transmitter**: Transmitter code
- **receiver**: Receiver code
- **image**: pcb or schematic
- **doc**: Documentation

# how to build

Transmitter you need to use IAR IDE to compile, while Receiver uses STM32 to compile with MDK-V5

# Hardware

## Transmitter model rendering

![transmitter](image/transmitter.png)

## Receiver PCB

![](image/board-f.png)



## Schematic

PDF [doc/SCH-Transmitter.pdf](doc/SCH-Transmitter.pdf)

PDF [doc/SCH-Receiver.pdf](doc/SCH-Receiver.pdf)






