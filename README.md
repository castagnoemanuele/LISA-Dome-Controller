# LISA-Dome-Controller
![Observatory](https://github.com/castagnoemanuele/LISA-Dome-Controller/blob/main/Assets/LISA_Observatory.jpg)

This repository contains the project of the controller system for the LISA observatory, located in Genoa. The old controller board was defective and needed to be replaced, and by doing that the system was also modernized and updated to support, for example, the WiFi connection. 

![Diagram](https://github.com/castagnoemanuele/LISA-Dome-Controller/blob/main/Assets/System%20Diagram.png)
The system is composed of three main sub-systems:
* Telescope
* Motor system
* Control system



## Motor system
![Motor System](https://github.com/castagnoemanuele/LISA-Dome-Controller/blob/main/Assets/MotorSubSystem.png)

The motor system is composed of three parts:
1. End switch: a simple switch that gets closed at a certain position, which will be referred as "position zero" from now on.
2. Encoder: has three pins (ENCODER1, ENCODER2, ENCODER3) plus a common one. Each one is activated every third of a rotation of the motor
3. 12V DC motor (60w power draw)

The only modification done to this part of the system was a RC circuit connected in parallel with the end switch to filter out noise that was causing problems (ghost change of state of the end switch).
The rotative encoder functioning is is simplified in the following animation:

![Encoder_rotation](https://github.com/castagnoemanuele/LISA-Dome-Controller/blob/main/Assets/EncoderDiagram.gif)

In the way it works it is more similar to a rotation counter, but given it's function it will be called encoder.


## Controller system
Composed of four parts:
* Two 12V power supplies, one for the motor and one for the Control system
* The controller board
* The relay board
* The external interface

### Relay board
This board has been developed for the purpose of powering the 12V motor. In needs to be able to switch the direction, so two SPDT relays have been used to form an h-bridge. The microcontroller drives the relays throught two npn transistors, and two LEDs have been added to signal which relay is active. The connections are the following:
| PIN | Description|
| ----|------------|
|S1   |Selector pin for relay 1, when HIGH turns the coil of relay 1 on|
|S2   |Selector pin for relay 2, when HIGH turns the coil of relay 2 on|
|GND  |Ground, connected to the gnd of the step down on the control board (note that it is isolated from the 12V part of the circuit) 
|5V   | Connected to the output of the step down on the control board
|GND(High power part)|Connected to the motor power supply|
|12V | Connected to the motor power supply|
|MOTOR| connection to the motor

The board was custom made using EasyEda and printed by JLCPCB. the PCB has copper traces wide and thick enough to support more than 5A of current. Schematics are avaiable on the apposite folder of this repo.

#### Components of the Relay Board
* 2* RT114005 SPDT relays
* 2* 2N2222 transistors
* 2*220 Ohm Resistor
* 2*1KOhm Resistor
* 2* Flyback diode
* Screw terminal connectors



### Controller board
![Controller Board Diagram](https://github.com/castagnoemanuele/LISA-Dome-Controller/blob/main/Assets/ControllerBoard.png)

The controller board hosts the microcontroller and all the interfaces it needs to check the sensors and control the other devices in the system. It also hosts three buttons, which have the same functionality as the ones in the external interface. The microcontroller is a WEMOS ESP32-S2 mini, chosen for the low cost and WiFi support.
the power supply that was present in the electrical box had an output of 12V DC, so a variable step down converter was added, and it is set to 5V and connected to the Vin pin of the microcontroller. 
The I²C interface is powered from the 3.3V pin of the microcontroller and has two 10K pullup resistors, on the back of the perf board.
All the connections are reported in the schematic on the apposite folder. The pinscan be described as it follows:

| PIN | Description|
| ----|------------|
|5V| Connection to the Vin pin of the MCU and the OUT+ of the buck converter|
|GND| Ground =0|
|S2 | Connected to the S2 Pin of the relayboard. When high activates relay 2. It is also connected to the External S2 Button of the physical interface|
|S1 | Connected to the S2 Pin of the relayboard. When high activates relay 1. It is also connected to the External S1 Button of the physical interface|
|EXT_RST| Connected to the reset Button of the external interface
|Encoder1,2,3| Connected to the encoder pins (in the motor system|
|TELESC_RX| Connected to the RX pin of the serial to RS232 Adapter|
|TELESC_TX| Connected to the TX pin of the serial to RS232 Adapter|
|END_SWITCH| Connected to the end switch in the motor system!
|3.3V| Connected to the 3.3V pin of the ESP32|
|IN+| Positive Input pin for the buck converter. In the system connected to a 12V PSU|
|IN-| Negative pin for the buck converter. In the system connected to a 12V PSU|


### External interface
![External Interface](https://github.com/castagnoemanuele/LISA-Dome-Controller/blob/main/Assets/PhysicalInterface.jpg)

The buttons present on the controller board are not avaiable when the electrical box is closed, so to be able to control the Dome posision manually it was necessary to connect the (already present) external keypad and add the external display with a 3d printed enclosure, which was posted on [Thingiverse](https://www.thingiverse.com/thing:3080488) by SteveTecx.

## Complete system 
Here is a more complete overview on the contents of the elecrical box:

![External Interface](https://github.com/castagnoemanuele/LISA-Dome-Controller/blob/main/Assets/CompleteSystem.png)

## Notes
The .ino file in the simulator folder was used to simulate the behaviour of the dome to program the firmware outside of the dome. More details are available in my thesis pdf file in the documents folder.


