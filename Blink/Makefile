# Makefile
#
# ****************************************************************************************************
# Author:
#	Nipun Gunawardena
#
# Credits:
#	Countless makefile tutorials
#
# Requirements:
#	None
#
# Description:
#	Makefile for use with arm-non-eabi-gcc and Tiva Launchpad. Upload code to Launchpad with 
#	'lm4flash blink.bin'
# ****************************************************************************************************

all:
	arm-none-eabi-gcc blink.c startup_gcc.c -g -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -ffunction-sections -fdata-sections -MD -std=c99 -Wall -pedantic -DPART_TM4C123GH6PM -c -I/Users/Nipun/Documents/TivaWare -DTARGET_IS_BLIZZARD_RA1
	arm-none-eabi-ld -T blink.ld --entry ResetISR -o a.out startup_gcc.o blink.o --gc-sections
	arm-none-eabi-objcopy -O binary a.out blink.bin
