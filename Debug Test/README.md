# Debugging How-To #
---
## About ##
Short introduction on how to setup debugging on the Tiva Launchpad.

## Credits ##
Thanks to pjkim from [this](http://forum.stellarisiti.com/topic/1741-using-gdb-and-openocd-to-remote-debug-tivastellaris-on-a-mac-os/) thread at the stellaristi forums and [kernalhacks](http://kernelhacks.blogspot.com/2012/11/the-complete-tutorial-for-stellaris_23.html) for the excellent previous work.

## Preparation ##
1. Use the included makefile to compile the code. I've tried using other makefiles but they didn't work - I would get "couldn't read" errors from ```gdb```. But I also don't really know what I'm doing so there's that.
2. Using Macports, install ```openocd``` using the ```ti``` variant. The command to do that is ```sudo port install openocd +ti```. If you've previously installed ```openocd``` using Macports, but you didn't do the ```ti``` variant, you will need to uninstall ```openocd``` and reinstall it with the correct variant.
3. Ensure that ```arm-none-eabi-gdb``` is installed on your system. If you installed ```arm-none-eabi-gcc``` using Macports, it should be already installed.
4. Copy ```ek-tm4c123gxl.cfg``` from ```/opt/local/share/openocd/scripts/board/``` to the working directory. Alternatively, just use the one in this folder.

## Use ##
After the code has been uploaded to the board, follow the following steps to activate gdb:

1. Run ```openocd --file ek-tm4c123gxl.cfg```. You may need to use sudo.
2. Open another terminal or terminal tab
3. Run ```arm-none-eabi-gdb blink.axf```. I used to get errors here a lot but unfortunately I don't really know how to fix them.
4. Once in ```gdb```, run the following commands. This connects gdb to ```openocd```, loads the program, and initializes a debug session.

```
target extended-remote :3333
monitor reset halt
load
monitor reset init
```
```GDB``` should now completely be initialized. You can enable breakpoints, step through code, etc. You can exit ```gdb``` by using the ```q``` command, and you can exit ```openocd``` using ```<CTRL> + C```.

Good luck!