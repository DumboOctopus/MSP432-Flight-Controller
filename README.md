# MSP432-Flight-Controller

This repo is a simple flight controller for a manually controlled 4 channel fixed wing aircraft.

## Hardware

* TI MSP 432P401R 
* Tri State buffer chip 
* Level Shifter
* [Electronic Speed Control with UBEC](https://www.amazon.com/gp/product/B071GRSFBD/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1)
* SG90 Servos
* 2 Amp fuses
* [SPM4650 Micro Receiver](https://www.spektrumrc.com/Products/Default.aspx?ProdID=SPM4650)
* 2S Lipo Battery
* [2-4S Brushless DC Motor](https://www.amazon.com/gp/product/B07TNQ6HJN/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1)

The Tristate buffers are used to interface between half duplex UART that the micro receiver uses and full duplex UART that the microcontroller uses.

## Software 

The TI MSP 432 microcontroller controls the entire aircraft. This was selected because is a low power microcontroller, has a better processor than most arduinos and supports real time operating systems.

The TI MSP 432 communicates with the SPM4650 micro receiver using the SRXL2 protocol. The micro receiver will receive commands from the Spektrum transmitter that the operator uses. 
Then it will encode the information into SRXL2 Control Data packets and sends them on a UART line. The microcontroller unpackages this data and updates PWM outputs which
control the servos and the Electronic Speed Control (ESC). The ESC then controls the speed of the main propeller.

# Future Goals

Many RC aircraft have a flight envelope feature. This restricts the pitch, roll and yaw to limited values to prevent the aircraft from flying at extreme orientations and possibly crashing or stalling.

Eventually, I would like to implement this using an IMU.

Additionally, I would like to modify the code to make this aircraft a VTOL aircraft. Creating a VTOL aircraft would be an interesting challenge and would allow my to fly planes even in the small parks around my home.
