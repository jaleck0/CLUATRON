# CLUATRON
repository for the CLUATRON - Made by JALECKO
project registred on github since 17/3/24

The CLUATRON is a RP2040 based general purpose computer inspired by the maximite line of micro computers and the PICO-8 fantasy console.
The goal with this project is to have a functional microcomputer that has an operating system with a terminal and a text editor.
Programs for the CLUATRON will be written in a modified version of LUA 5.4.6 with a couple of additional commands that allow for easier program development.


# Build instructions 
This will create the desired excecutable, this instalation does require the use of CMake and the PI PICO SDK with a compiled pico-extras library 

```
cd CLUATRON/Core
mkdir build
cd build
cmake ..
make
```
