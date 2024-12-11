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
Programs can be edited in the Cluatron Editing tool which is accesed with F2.
Commands can be written and executed in the Cluashell which is accesed with F1.

## Programming
As mentioned before programs and commands are written in LUA 5.4.6.
The Cluatron also provides extra functions baked into the system for more functionality these functions can be sorted into different groups.

### System control functions
- reset() : Resets the Cluatron.
- bootsel() : Puts the Cluatron in bootsel mode.
- millis() : Returns the amount of milliseconds the Cluatron has been active.

### Terminal control functions
- pcol( c) : Sets the text color of the terminal, "c" is the color index within a range of 0 - 15.
- bcol( c) : Sets the background color of the terminal, "c" is the color index within a range of 0 - 15. 

### Graphics functions
- rgb( r, g, b) : Returns a 15 bit color value, "r" "g" "b" are within range of 0 - 255. 
- pal( c, setcol) : sets color value for index "c" to "setcol", "c" is within range of 0 - 15 and "setcol" is within range of 0 - 32767. Use the rgb() function to accurately determine the "setcol".
- dot( x, y, c)
- cls( c)
- line( x1, y1, x2, y2, c)
- rect( x, y, w, h, c)
- rectfill( x, y, w, h, c)
- tri( x1, y1, x2, y2, x3, y3, c)
- circ( x, y, r, c)
- circfill( x, y, r, c)
- oval( x, y, w, h, c)
- ovalfill( x, y, w, h, c)
- text( str, x, y, c)
- textfill( str, x, y, c0, c1)

*As of now all parameters must be entered with integers so to draw a rectangle to a variable position on the screen one must enter:
```
rect(math.floor(x), math.floor(y), 16, 32, 8)
```

### USB input functions
- init_input()
- read_input()
- keyp( kbkey)
- keyh( kbkey)
- keyr( kbkey)
- mdx()
- mdy()
- mscr()
- mbp( mbutton)
- mbh( mbutton)
- mbr( mbutton)

*As of now all of these functions return integers and reading them out must be treated so
This is how one must read out the value of a keyboard button pressed
```
if (keyp(80) == 1) then
	moveleft()
end
```

### USB keyboard input constants
![Alt](/pictures/USBkbmap.png)

### USB mouse input constants
0. Left button
1. Middle button
2. Right button

For extra programming references look into the /Programs/-folder.
