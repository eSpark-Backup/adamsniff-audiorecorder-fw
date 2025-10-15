#****************************************************************************
# Makefile
# openacousticdevices.info
# November 2020
#****************************************************************************

# Set the toolpath parameter to the location of the GNU Arm Embedded Toolchain

TOOLCHAIN_PATH = C:/GNU_Arm_Embedded_Toolchain
JLINK_PATH = "C:/Program Files/SEGGER/JLink"

TOOLCHAIN_VERSION = 10_2021.10

# Determine the path to the toolchain binary

TOOLPATH = $(TOOLCHAIN_PATH)/$(TOOLCHAIN_VERSION)/bin/

# These are the locations of the source and header files

INC = ../cmsis ../device/inc ../emlib/inc ../emusb/inc ../drivers/inc ../fatfs/inc ../ioexpanders/inc ../inc ../gps/inc
SRC = ../device/src ../emlib/src ../emusb/src ../drivers/src ../fatfs/src ../ioexpanders/src ../src ../gps/src

# Set the name of the output files

FILENAME = audiomoth

# This is the location of the resulting object files

OBJPATH = ./objects/

# Set the target EFM32WG to that used in AudioMoth

TARGET = EFM32WG380F256

# The following code generates the list of objects and the search path of source and header files

VPATH = $(SRC)

IFLAGS = $(foreach d, $(INC), -I$d)

_CSRC = $(notdir $(foreach d, $(SRC), $(wildcard $d/*.c)))
_SSRC = $(notdir $(foreach d, $(SRC), $(wildcard $d/*.s)))

_OBJ = $(_CSRC:.c=.o) $(_SSRC:.s=.o)

OBJ = $(sort $(foreach d, $(_OBJ), $(OBJPATH)$d))

DEP = $(OBJ:.o=.d)

# These are the compilation settings

CC = $(TOOLPATH)arm-none-eabi-gcc

CSIZE = $(TOOLPATH)arm-none-eabi-size

COBJCOPY = $(TOOLPATH)arm-none-eabi-objcopy

COBJDUMP = $(TOOLPATH)arm-none-eabi-objdump

CFLAGS = -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mthumb -Wall '-DARM_MATH_CM4=1' '-D$(TARGET)=1'

DFLAGS = -MMD

# Finally the build rules

$(OBJPATH)%.o: %.c
# 	@mkdir -p $(OBJPATH)
	@echo "Building" $@
	@$(CC) $(CFLAGS) $(DFLAGS) -O3 -ffunction-sections -fdata-sections -std=c99 -c -o "$@" "$<" $(IFLAGS)

$(OBJPATH)%.o: %.s
# 	@mkdir -p $(OBJPATH)
	@echo "Building" $@
	@$(CC) $(CFLAGS) $(DFLAGS) -x assembler-with-cpp -c -o "$@" "$<" $(IFLAGS)

$(FILENAME).bin: $(OBJ)
	@echo "Building" $(FILENAME).axf
	@$(CC) $(CFLAGS) $(DFLAGS) -T "audiomoth.ld" -Xlinker --gc-sections -Xlinker -Map="$(FILENAME).map" --specs=nano.specs -o $(FILENAME).axf $(OBJ) -lm -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo "Building" $(FILENAME).lst
	@$(COBJDUMP) -d -t -S "$(FILENAME).axf" > "$(FILENAME).lst" 
	@echo "Building" $(FILENAME).hex
	@$(COBJCOPY) -O ihex "$(FILENAME).axf" "$(FILENAME).hex"
	@echo "Building" $(FILENAME).bin
	@$(COBJCOPY) -O binary "$(FILENAME).axf" "$(FILENAME).bin"
	@$(CSIZE) -A "$(FILENAME).axf"

# Rule to flash the firmware to the MCU
flash: $(FILENAME).bin
	@echo "Flashing firmware $(FILENAME).bin to the MCU..."
# 	Create a temporary JLink script to use dynamic filename
	@echo "si 1" > flash_script.jlink
	@echo "speed 4000" >> flash_script.jlink
	@echo "r" >> flash_script.jlink
	@echo "h" >> flash_script.jlink
	@echo "loadfile $(FILENAME).bin 0x00000000" >> flash_script.jlink
	@echo "reset" >> flash_script.jlink
	@echo "go" 	 >> flash_script.jlink
	@echo "exit" >> flash_script.jlink

# 	Run the JLink command with the generated script
	$(JLINK_PATH)/JLink.exe -device EFM32WG380F256 -if SWD -speed 4000 -autoconnect 1 -CommandFile flash_script.jlink

	@echo "Firmware $(FILENAME).bin successfully flashed!"

# Optional: Launch the SWO Viewer if the 'monitor' target is specified
monitor:
	@echo "Launching SWO Viewer..."
	$(JLINK_PATH)/JLinkSWOViewer.exe


erase:
	@echo "Erasing the MCU..."
# 	Create a temporary JLink script to erase the flash
	@echo "si 1" > erase_script.jlink        # Select interface
	@echo "speed 4000" >> erase_script.jlink # Set speed
	@echo "r" >> erase_script.jlink         # Reset the MCU
	@echo "h" >> erase_script.jlink         # Halt the MCU
	@echo "erase" >> erase_script.jlink     # Erase the MCU flash
	@echo "exit" >> erase_script.jlink      # Exit the JLink Commander

# 	Run the JLink command with the generated script
	$(JLINK_PATH)/JLink.exe -device EFM32WG380F256 -if SWD -speed 4000 -autoconnect 1 -CommandFile erase_script.jlink

	@echo "MCU successfully erased!"

-include $(DEP)

.PHONY: clean
clean:
	rm -f $(OBJPATH)*.o
	rm -f $(OBJPATH)*.d
	rm -f $(FILENAME).axf
	rm -f $(FILENAME).lst
	rm -f $(FILENAME).bin
	rm -f $(FILENAME).hex
	rm -f $(FILENAME).map
