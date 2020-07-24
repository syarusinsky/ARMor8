TARGET = main

# stm32cube files directory
STM32_CUBE_FILES_DIR = ../lib/STM32f302x8-HAL/stm32cubef3
STM32_CUBE_INCLUDE_DIR = $(STM32_CUBE_FILES_DIR)/include

# llpd files directory
LLPD_FILES_DIR = ../lib/STM32f302x8-HAL/llpd
LLPD_INCLUDE_DIR = $(LLPD_FILES_DIR)/include
LLPD_SRC_DIR = $(LLPD_FILES_DIR)/src

# ARMor8 files directory
ARMOR8_FILES_DIR = ../
ARMOR8_INCLUDE_DIR = $(ARMOR8_FILES_DIR)/include
ARMOR8_SRC_DIR = $(ARMOR8_FILES_DIR)/src

# SAL files directory
SAL_FILES_DIR = ../lib/SAL
SAL_INCLUDE_DIR = $(SAL_FILES_DIR)/include
SAL_SRC_DIR = $(SAL_FILES_DIR)/src

# SIGL files directory
SIGL_FILES_DIR = ../lib/SIGL
SIGL_INCLUDE_DIR = $(SIGL_FILES_DIR)/include
SIGL_SRC_DIR = $(SIGL_FILES_DIR)/src

# include directories
INCLUDE =  -I$(STM32_CUBE_INCLUDE_DIR)
INCLUDE += -I$(LLPD_INCLUDE_DIR)
INCLUDE += -I$(ARMOR8_INCLUDE_DIR)
INCLUDE += -I$(SAL_INCLUDE_DIR)
INCLUDE += -I$(SIGL_INCLUDE_DIR)

# linker script
LD_SCRIPT = $(STM32_CUBE_FILES_DIR)/ld/STM32F302R8Tx_FLASH.ld

# mcu definition
MCU_SPEC = cortex-m4

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
ASFLAGS += -mthumb
ASFLAGS += -mhard-float
ASFLAGS += -mfloat-abi=hard
ASFLAGS += -mfpu=fpv4-sp-d16
ASFLAGS += -Wall
# (set error messages to appear on a single line.)
ASFLAGS += -fmessage-length=0

# c/c++ compilation directives
CFLAGS =  -std=gnu++11
CFLAGS += -mcpu=$(MCU_SPEC)
CFLAGS += -mthumb
CFLAGS += -mhard-float
CFLAGS += -mfloat-abi=hard
CFLAGS += -mfpu=fpv4-sp-d16
CFLAGS += -Wall
# (set error messages to appear on a single line.)
CFLAGS += -fmessage-length=0
# (set system to ignore semihosted junk)
CFLAGS += -specs=nosys.specs
CFLAGS += -fno-exceptions
CFLAGS += -fno-rtti
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
# (allow SIGL to use software rendering)
CFLAGS += -DSOFTWARE_RENDERING

# linker directives.
LSCRIPT = ./$(LD_SCRIPT)
LFLAGS =  -std=c++11
LFLAGS += -mcpu=$(MCU_SPEC)
LFLAGS += -mhard-float
LFLAGS += -mfloat-abi=hard
LFLAGS += -mfpu=fpv4-sp-d16
LFLAGS += -mthumb
LFLAGS += -Wall
LFLAGS += -specs=nosys.specs
LFLAGS += -lstdc++_nano
LFLAGS += -Wl,--gc-sections
LFLAGS += -T$(LSCRIPT)

# assembly files to compile
AS_SRC = ./$(STM32_CUBE_FILES_DIR)/startup_stm32f302x8.s

# c files to compile
C_SRC =  ./$(STM32_CUBE_FILES_DIR)/system_stm32f3xx.c
C_SRC += ./$(LLPD_SRC_DIR)/LLPD.c

# c++ files to compile
CPP_SRC = ./main.cpp
CPP_SRC += $(wildcard $(ARMOR8_SRC_DIR)/*.cpp)
CPP_SRC += $(wildcard $(SAL_SRC_DIR)/*.cpp)
CPP_SRC += $(wildcard $(SIGL_SRC_DIR)/*.cpp)

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
	$(CC) -c -O3 $(CFLAGS) $(INCLUDE) $< -o $@

%_release.o: %.cpp
	$(CC) -c -O3 $(CFLAGS) $(INCLUDE) $< -o $@

$(TARGET)_release.elf: $(RELEASE_OBJS)
	$(CC) $^ $(LFLAGS) -o $@

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
	rm -f $(RELEASE_OBJS)
	rm -f $(DEBUG_OBJS)
	rm -f $(TARGET)_release.elf
	rm -f $(TARGET)_release.bin
	rm -f $(TARGET)_debug.elf
	rm -f $(TARGET)_debug.bin
