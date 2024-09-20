# LISA-Dome-Controller

This repository contains the project of the controller system for the LISA observatory, located in Genoa. The old controller board was defective and needed to be replaced. THe system is composed of three main sub-systems:
* Telescope
* Motor system
* Control system

## Motor system
The motor system is composed of three parts:
* 12V DC motor (60w power draw)
* Encoder: has three pins (ENCODER1, ENCODER2, ENCODER3) plus a common one. Each one is activated every third of a rotation of the motor
* End switch: a simple switch that gets closed at a certain position, which will be referred as "position zero" from now on.
The only modification done to this part of the system was a RC circuit connected in parallel with the end switch to filter out noise that was causing problems (ghost change of state of the end switch).

##Controller system
Composed of four parts:
* Two 12V power supplies, one for the motor and one for the controller board
* The controller board
* The relay board
* The external interface






## Components
![Immagine WhatsApp 2023-11-21 ore 09 59 53_9c08ac0d](https://github.com/castagnoemanuele/LISA-Dome-Controller/assets/100867215/487692d3-7d65-4a4e-9536-0691e81c85d1)

* Lolin ESP32 S2 Mini
* Controller Board
* Relay Board
* I2C Oled Display
	

