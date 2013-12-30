# Tiva Test #
---
## Introduction ##
This repository is a testbed for code that is to be used with Texas Instruments' [Tiva Launchpad](http://www.ti.com/tool/ek-tm4c123gxl). In this README, I try to use the Blink project as the example for most of the instructions. This project is a work in progress...

## Installation and Setup##
Since I develop on a Mac, here's what I did to get everything working properly:

1. Install Xcode and Xcode command line tools
2. Install Macports
3. Using Macports, install arm-none-eabi-gcc and libusb
4. Install [lm4tools](https://github.com/utzig/lm4tools)
5. Download [TivaWare](http://www.ti.com/tool/sw-tm4c) from Texas Instruments. It will probably be an exe file. Extract it and move the TivaWare folder to wherever you would like to keep it. `cd` into the folder and run `make`. This should be enough to get TivaWare fully compiled on a Mac.
6. Test everything:
	1. `cd` into the TivaWare folder
	2. Once in the TivaWare folder, `cd` to `examples/boards/ek-tm4c123gxl/blinky/gcc/`
	3. Ensure the Launchpad switch is set to "Debug"
	4. Plug in the Launchpad
	5. Run `lm4flash blinky.bin`. The built-in LED should start blinking

## Compiling ##
The last three lines of the Makefile from [this](https://github.com/madvoid/Tiva-Test/commit/182c6fb441e81f4dc0d1d8470427f55eae63c145) commit show the commands that I use to compile code and upload it to the Launchpad. You will need to change the file names to whatever you are using, and change the path after the `-I` flag to wherever your TivaWare base folder is located. I gathered all of these flags by a combination of [this](http://recursive-labs.com/blog/2012/10/28/stellaris-launchpad-gnu-linux-getting-started/) tutorial and examining the flags from the makefiles included with TivaWare. The files `startup_gcc.c` and `blink.ld` were taken directly from the TivaWare examples.

Alternatively, you could just run the makefile to compile the code. As an alternative to running lm4flash, `make upload` will upload the binary application file to the Launchpad.

Due to my inability to write proper makefiles, for any `driverlib` files that are used in the code, the corresponding `.o` file must be manually added to the Makefile's linker flags. In addition, any `.c` files used from the TivaWare `utils` folder must be added manually. See the makefiles for examples.

## Project Descriptions ##
*	**Blink** - Blinks an LED on and off
*	**Print** - Prints to COM port and notifies user of LED status changes