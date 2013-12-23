# Tiva Test #
---
## Introduction ##
This repository is a testbed for code that is to be used with Texas Instruments' [Tiva Launchpad](http://www.ti.com/tool/ek-tm4c123gxl). It's a work in progress...
## Installation ##
Since I develop on a Mac, here's what I did to get everything working properly:

1. Install Xcode and Xcode command line tools
2. Install Macports
3. Using Macports, install arm-none-eabi-gcc and libusb
4. Install [lm4tools](https://github.com/utzig/lm4tools)
5. Download [TivaWare](http://www.ti.com/tool/sw-tm4c) from Texas Instruments. It will probably be an exe file. Extract it and move the TivaWare folder to wherever you would like to keep it. `cd` into the folder and run `make`. This should be enough to get TivaWare fully compiled on a Mac.
6. Test everything
	1. `cd` into the TivaWare folder
	2. Once in the TivaWare folder, `cd` to `examples/boards/ek-tm4c123gxl/blinky/gcc/`
	3. Ensure the Launchpad switch is set to "Debug"
	4. Plug in the Launchpad
	5. Run `lm4flash blinky.bin`. The built-in LED should start blinking

