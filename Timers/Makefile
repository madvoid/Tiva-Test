# Makefile
#
# ****************************************************************************************************
# Author:
#	Nipun Gunawardena
#
# Credits:
#	Countless makefile tutorials; makedefs from TivaWare
#
# Requirements:
#	None
#
# Description:
#	Simple makefile for use with arm-non-eabi-gcc and Tiva Launchpad. Upload code to Launchpad with 
#	'lm4flash timers.bin'
# ****************************************************************************************************




# ----------------------------------------------------------------------------------------------------
# Definitions
# ----------------------------------------------------------------------------------------------------

FILENAME=timers

# Compiler, linker, etc.
CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld
OBJCOPY=arm-none-eabi-objcopy

# Compiler CPU/FPU options
CPU=-mcpu=cortex-m4
FPU=-mfpu=fpv4-sp-d16 -mfloat-abi=softfp

# Filepaths
ROOT=/Users/Nipun/Documents/TivaWare
DRIVOBJROOT=${ROOT}/driverlib/gcc

# Compiler flags
CFLAGS=${FILENAME}.c       \
       startup_gcc.c       \
       ${ROOT}/utils/uartstdio.c \
       -g                  \
       -mthumb             \
       ${CPU}              \
       ${FPU}              \
       -ffunction-sections \
       -fdata-sections     \
       -MD                 \
       -std=c99            \
       -Wall               \
       -pedantic           \
       -DPART_TM4C123GH6PM \
       -c                  \
       -Os                 \
       -I${ROOT}           \
       -DTARGET_IS_BLIZZARD_RB1 \

# Linker flags
# Note: Any object files from any driverlib files needs to be appended to the LDFLAGS below in the 
#	format of ${DRIVEOBJROOT}/driverlibfile.o
LDFLAGS=-T                 \
        ${FILENAME}.ld     \
        --entry ResetISR   \
	--gc-sections      \
	-o a.out startup_gcc.o ${FILENAME}.o \
        ${DRIVOBJROOT}/fpu.o    \
	${DRIVOBJROOT}/gpio.o   \
        ${DRIVOBJROOT}/interrupt.o \
	${DRIVOBJROOT}/sysctl.o \
        ${DRIVOBJROOT}/uart.o   \
        uartstdio.o \



# ----------------------------------------------------------------------------------------------------
# Rules
# ----------------------------------------------------------------------------------------------------
all:
	${CC} ${CFLAGS}
	${LD} ${LDFLAGS} 
	${OBJCOPY} -O binary a.out ${FILENAME}.bin

upload:
	lm4flash ${FILENAME}.bin

clean:
	rm -fv *.o *.d a.out ${FILENAME}.bin
