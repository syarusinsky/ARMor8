TARGET = main

# stm32cube files directory
STM32_CUBE_FILES_DIR = ../../lib/STM32h745zi-HAL/stm32cubeh7
STM32_CUBE_INCLUDE_DIR = $(STM32_CUBE_FILES_DIR)/include

# llpd files directory
LLPD_FILES_DIR = ../../lib/STM32h745zi-HAL/llpd
LLPD_INCLUDE_DIR = $(LLPD_FILES_DIR)/include
LLPD_SRC_DIR = $(LLPD_FILES_DIR)/src

# devlib files directory
DEVLIB_FILES_DIR = ../../lib/DevLib
DEVLIB_INCLUDE_DIR = $(DEVLIB_FILES_DIR)/include
DEVLIB_SRC_DIR = $(DEVLIB_FILES_DIR)/src

# armor8 files directory
ARMOR8_FILES_DIR = ../..
ARMOR8_INCLUDE_DIR = $(ARMOR8_FILES_DIR)/include
ARMOR8_SRC_DIR = $(ARMOR8_FILES_DIR)/src

# sal files directory
SAL_FILES_DIR = ../../lib/SAL
SAL_INCLUDE_DIR = $(SAL_FILES_DIR)/include
SAL_SRC_DIR = $(SAL_FILES_DIR)/src

# include directories
INCLUDE =  -I$(STM32_CUBE_INCLUDE_DIR)
INCLUDE += -I$(LLPD_INCLUDE_DIR)
INCLUDE += -I$(DEVLIB_INCLUDE_DIR)
INCLUDE += -I$(ARMOR8_INCLUDE_DIR)
INCLUDE += -I$(SAL_INCLUDE_DIR)

# linker scripts
LD_SCRIPT = $(STM32_CUBE_FILES_DIR)/ld/CM7/STM32H745ZITX_FLASH.ld

# mcu definition
MCU_SPEC = cortex-m7

# toolchain definitions
TOOLCHAIN = /usr/bin
CC  = $(TOOLCHAIN)/arm-none-eabi-gcc
CPP = $(TOOLCHAIN)/arm-none-eabi-g++
AS  = $(TOOLCHAIN)/arm-none-eabi-as
LD  = $(TOOLCHAIN)/arm-none-eabi-ld
OC  = $(TOOLCHAIN)/arm-none-eabi-objcopy
OD  = $(TOOLCHAIN)/arm-none-eabi-objdump
OS  = $(TOOLCHAIN)/arm-none-eabi-size

# assembly directives.
ASFLAGS += -c
ASFLAGS += -O0
ASFLAGS += -mcpu=$(MCU_SPEC)
ASFLAGS += -mthumb-interwork
ASFLAGS += -mhard-float
ASFLAGS += -mfloat-abi=hard
ASFLAGS += -mfpu=fpv5-sp-d16
ASFLAGS += -Wall
# (set error messages to appear on a single line.)
ASFLAGS += -fmessage-length=0

# c/c++ compilation directives
CFLAGS =  -std=gnu++11
CFLAGS += -mcpu=$(MCU_SPEC)
CFLAGS += -mthumb-interwork
CFLAGS += -mhard-float
CFLAGS += -mfloat-abi=hard
CFLAGS += -mfpu=fpv5-sp-d16
CFLAGS += -Wall
CFLAGS += -Wdouble-promotion
CFLAGS += -Wfloat-conversion
CFLAGS += -fsingle-precision-constant
CFLAGS += -ffast-math
# (set error messages to appear on a single line.)
CFLAGS += -fmessage-length=0
# (set system to ignore semihosted junk)
CFLAGS += -specs=nano.specs
CFLAGS += -specs=nosys.specs
CFLAGS += -fno-exceptions
CFLAGS += -fno-rtti
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += -DCORE_CM7
CFLAGS += -DSTM32H745xx
CFLAGS += -DTARGET_BUILD
CFLAGS += -DARM_MATH_CM7
CFLAGS += -D__FPU_PRESENT

# linker directives.
LSCRIPT = ./$(LD_SCRIPT)
LFLAGS =  $(CFLAGS)
LFLAGS += -lstdc++_nano
LFLAGS += -Wl,--gc-sections
LFLAGS += -flto
LFLAGS += -T$(LSCRIPT)

# assembly files to compile
AS_SRC = $(STM32_CUBE_FILES_DIR)/startup_stm32h745zitx.s

# c files to compile
C_SRC =  $(STM32_CUBE_FILES_DIR)/system_stm32h7xx_dualcore_boot_cm4_cm7.c
C_SRC += $(wildcard $(ARMOR8_SRC_DIR)/*.c)

# c++ files to compile
CPP_SRC = ./main.cpp
CPP_SRC += ./$(LLPD_SRC_DIR)/LLPD.cpp
CPP_SRC += $(wildcard $(DEVLIB_SRC_DIR)/*.cpp)
CPP_SRC += $(wildcard $(ARMOR8_SRC_DIR)/*.cpp)
CPP_SRC += $(wildcard $(SAL_SRC_DIR)/*.cpp)
CPP_SRC += $(wildcard $(SIGL_SRC_DIR)/*.cpp)

CPP_SRC_TMP := $(CPP_SRC)
CPP_SRC = $(filter-out $(ARMOR8_SRC_DIR)/ARMor8UiManager.cpp, $(CPP_SRC_TMP))

RELEASE_OBJS =  $(AS_SRC:.s=_release.o)
RELEASE_OBJS += $(C_SRC:.c=_release.o)
RELEASE_OBJS += $(CPP_SRC:.cpp=_release.o)

DEBUG_OBJS =  $(AS_SRC:.s=_debug.o)
DEBUG_OBJS += $(C_SRC:.c=_debug.o)
DEBUG_OBJS += $(CPP_SRC:.cpp=_debug.o)

.PHONY: release
release: $(TARGET)_release.bin

%_release.o: %.s
	$(CC) -x assembler-with-cpp $(ASFLAGS) $< -o $@

%_release.o: %.c
	$(CC) -c -g -O3 $(CFLAGS) $(INCLUDE) $< -o $@

%_release.o: %.cpp
	$(CC) -c -g -O3 $(CFLAGS) $(INCLUDE) $< -o $@

$(TARGET)_release.elf: $(RELEASE_OBJS)
	$(CC) $^ $(LFLAGS) -O3 -o $@

$(TARGET)_release.bin: $(TARGET)_release.elf
	$(OC) -S -O binary $< $@
	$(OS) $<

.PHONY: debug
debug: $(TARGET)_debug.bin

%_debug.o: %.s
	$(CC) -x assembler-with-cpp $(ASFLAGS) $< -o $@

%_debug.o: %.c
	$(CC) -c -g $(CFLAGS) $(INCLUDE) $< -o $@

%_debug.o: %.cpp
	$(CC) -c -g $(CFLAGS) $(INCLUDE) $< -o $@

$(TARGET)_debug.elf: $(DEBUG_OBJS)
	$(CC) $^ $(LFLAGS) -o $@

$(TARGET)_debug.bin: $(TARGET)_debug.elf
	$(OC) -S -O binary $< $@
	$(OS) $<

.PHONY: clean
clean:
	@echo "CPP_SRC_TMP is: $(CPP_SRC_TMP)"
	rm -f $(RELEASE_OBJS)
	rm -f $(DEBUG_OBJS)
	rm -f $(TARGET)_release.elf
	rm -f $(TARGET)_release.bin
	rm -f $(TARGET)_debug.elf
	rm -f $(TARGET)_debug.bin

all:
