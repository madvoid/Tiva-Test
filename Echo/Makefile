# Makefile
#
# ****************************************************************************************************
# Author:
#	Nipun Gunawardena
#
# Credits:
#	Countless makefile tutorials; makedefs from TivaWare
#	Mauro Scomparin's Stellaris Launchpad Makefile - https://github.com/scompo/stellaris-launchpad-template-gcc/blob/master/Makefile
#
# Requirements:
#	None
#
# Description:
#	Simple makefile for use with arm-none-eabi-gcc and Tiva Launchpad. Upload code to Launchpad with 
#	'lm4flash timers.bin'
# ****************************************************************************************************


# ----------------------------------------------------------------------------------------------------
# Filepaths
# ----------------------------------------------------------------------------------------------------
ROOT = /Users/Nipun/Documents/TivaWare
DRIVOBJROOT = ${ROOT}/driverlib/gcc
UTILSROOT = ${ROOT}/utils




# ----------------------------------------------------------------------------------------------------
# Project properties
# ----------------------------------------------------------------------------------------------------
FILENAME = echo
STARTUP_FILE = startup_gcc
LINKER_FILE = ${FILENAME}.ld
EXTERN_FILES = ${UTILSROOT}/uartstdio.c




# ----------------------------------------------------------------------------------------------------
# Definitions
# ----------------------------------------------------------------------------------------------------

# Compiler prefix
PREFIX = arm-none-eabi

# Compiler, linker, etc.
CC = ${PREFIX}-gcc
LD = ${PREFIX}-ld
OBJCOPY = ${PREFIX}-objcopy
OBJDUMP = ${PREFIX}-objdump

# Compiler CPU/FPU options
CPU = -mcpu=cortex-m4
FPU = -mfpu=fpv4-sp-d16 -mfloat-abi=softfp
PART = TM4C123GH6PM

# Compiler flags
CFLAGS=-g                  \
       -c                  \
       -mthumb             \
       ${CPU}              \
       ${FPU}              \
       -ffunction-sections \
       -fdata-sections     \
       -MD                 \
       -std=c99            \
       -Wall               \
       -pedantic           \
       -DPART_${PART}      \
       -Os                 \
       -I${ROOT}           \
       -DTARGET_IS_BLIZZARD_RB1 \

# Linker flags
LDFLAGS=--entry ResetISR   \
	--gc-sections      \

# Objectcopy flags
CPFLAGS = -O binary

# Objectdump flags
ODFLAGS = -S

# Lm4flash flags
UPFLAGS = -v

# Libgcc, libc, and libm paths
LIB_GCC_PATH=${shell ${CC} ${CFLAGS} -print-libgcc-file-name}
LIBC_PATH=${shell ${CC} ${CFLAGS} -print-file-name=libc.a}
LIBM_PATH=${shell ${CC} ${CFLAGS} -print-file-name=libm.a}

# Files
SRC = ${wildcard *.c} ${EXTERN_FILES}
OBJS = ${SRC:.c=.o}




# ----------------------------------------------------------------------------------------------------
# Rules
# ----------------------------------------------------------------------------------------------------
all: ${OBJS} ${FILENAME}.axf ${FILENAME}

%.o: %.c ${EXTERN_FILES}
	@echo Compiling ${<}...
	@${CC} ${CFLAGS} ${<} -o ${@}

${FILENAME}.axf: ${OBJS}
	@echo Linking...
	@${LD} -T ${LINKER_FILE} ${LDFLAGS} -o ${FILENAME}.axf ${OBJS} ${DRIVOBJROOT}/libdriver.a ${LIBM_PATH} ${LIBC_PATH} ${LIB_GCC_PATH}

${FILENAME}: ${FILENAME}.axf
	@echo Copying...
	@${OBJCOPY} ${CPFLAGS} ${FILENAME}.axf ${FILENAME}.bin
	@echo Dumping...
	@${OBJDUMP} ${ODFLAGS} ${FILENAME}.axf > ${FILENAME}.lst

upload: ${FILENAME}
	lm4flash ${UPFLAGS} ${FILENAME}.bin

clean:
	rm -fv *.o *.d *.axf *.bin *.lst
