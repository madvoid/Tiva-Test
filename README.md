# Tiva Test #
---
## Introduction ##
This repository is a testbed for code that is to be used with Texas Instruments' [Tiva Launchpad](http://www.ti.com/tool/ek-tm4c123gxl). In this README, I try to use the Blink project as the example for most of the instructions. Many of the programs will have minor functionality changes with commits, so go through the file history to see different things being done. This project is a work in progress...

## Installation and Setup##
Since I develop on a Mac, here's what I did to get everything working properly:

1. Install Xcode and Xcode command line tools
2. Install Macports
3. Using Macports, install ```arm-none-eabi-gcc``` and ```libusb```. As of now, the included makefiles only work with ```arm-none-eabi``` tools obtained through Macports. They do *not* work with ```arm-none-eabi``` tools obtained from [launchpad.net](https://launchpad.net/gcc-arm-embedded). If I have time, I will try to find a fix for that.
4. Install [lm4tools](https://github.com/utzig/lm4tools)
5. Download [TivaWare](http://www.ti.com/tool/sw-tm4c) from Texas Instruments. It will probably be an exe file. Extract it and move the TivaWare folder to wherever you would like to keep it. `cd` into the folder and run `make`. This should be enough to get TivaWare fully compiled on a Mac.
6. Test everything:
	1. `cd` into the TivaWare folder
	2. Once in the TivaWare folder, `cd` to `examples/boards/ek-tm4c123gxl/blinky/gcc/`
	3. Ensure the Launchpad switch is set to "Debug"
	4. Plug in the Launchpad
	5. Run `lm4flash blinky.bin`. The built-in LED should start blinking

## Compiling & Uploading##
All code can be compiled by running ```make``` in the working directory. The makefile has been designed so that all files inside the working directory will be automatically compiled. However, any ```*.c``` files that live in external folders will need to be added to the ```EXTERN_FILES``` line, all separated by spaces. To summarize, the following steps will need to be completed to modify the makefile for other projects:

1. Change the TivaWare root path on line 24 to whatever you're using.
2. Change the filename on line 33 to whatever your filename is called. This assumes that your filename is also the one used for the linker file, and that your startup file is called ```startup_gcc.c```.
3. Add any external ```*.c``` files to line 36, separated by spaces.

Programs can be uploaded by running ```make upload```. It should not be necessary to compile before running ```make upload```. Cleaning can be done with ```make clean```.


## Stack Size ##
The default stack size for the TivaWare examples are fairly small (256 bytes). To change the stack size to something larger, one must edit the `pui32Stack` variable on line 51 of `startup_gcc.c`. The default array size for this variable is 64, which is 256 bytes. What it should be changed to is context dependent.

# Project Descriptions #
*	**Blink** - Blinks an LED on and off
*	**BMP180** - Interfaces with Bosch BMP180 pressure sensor on SensorHub Boosterpack
*	**Countdown** - Counts down from 10 on serial monitor/LEDs and signals end of time
*	**Debug Test** - Used to test debugging. Code just blinks LED. See folder for instructions on how to debug.
*	**Echo** - Repeats user-entered serial input back to user
*	**ISL29023** - Interfaces with Intersil ISL29023 ambient light and infrared sensor on SensorHub Boosterpack
*	**Print** - Prints to COM port and notifies user of LED status changes
*	**SHT21** - Interfaces with Sensirion SHT21 sensor on SensorHub Boosterpack
*	**Sleep** - Demonstrates Launchpad hibernate mode. Goes into hibernate mode automatically, press SW2 to put Launchpad into programming mode.
*	**Templates** - Basic templates for use in projects
*	**Timers** - Blinks LEDs based on timer interrupts
*	**Watchdog** - Enables watchdog timer