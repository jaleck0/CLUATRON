# CLUATRON
repository for the CLUATRON - Made by JALECKO
project registred on github since 17/3/24

The CLUATRON is a RP2040 based general purpose computer inspired by the maximite line of micro computers and the PICO-8 fantasy console.
The goal with this project is to have a functional microcomputer that has an operating system with a terminal and a text editor.
Programs for the CLUATRON will be written in a modified version of LUA 5.4.6 with a couple of additional commands that allow for easier program development.


# Code build instructions 
This will create the desired excecutable, this instalation does require the use of CMake and the PI PICO SDK with a compiled pico-extras library.

```
cd CLUATRON/Core
mkdir build
cd build
cmake ..
make
```

# Hardware (currently)
- MCU: RP2040
	- 264 kb SRAM
	- 250 MHz it used to be200 MHz (overclocked)
	- Dual core ARM M0+
	- USB host/device capabilities
	- Programmable I/O capabilities

- Storage: QSPI Flash memory
	 - 2 MB of storage

- Resistor array
	- Digital analog converter voor VGA output
	- 15 bit signaal


# Schematic
![Alt](/pictures/VGA_connection_schematic.png)
This is the schematic used for connecting the pi pico the VGA screen.

![Alt](/pictures/board_exampl.png)
Here is an example of a development board for the Cluatron.

# Usage
Programs and commands are written in LUA 5.4.6.
