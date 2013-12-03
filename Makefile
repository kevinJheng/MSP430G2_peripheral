#Enter SubDir here
#SUBDIR := script 
#SUBDIR += .
#---------locate source & Objs
#SOURCE := $(wildcard \
#    	  	$(addsuffix /*.c,$(SUBDIR) )  \
#           )
include sources
OBJS := $(notdir $(SOURCE:.c=.o))
SHELL := cmd.exe
#-----------------------------

TARGET     = LaunchpadRev1_5
MCU        = msp430g2553
# List all the source files here
# eg if you have a source file foo.c then list it here
# Include are located in the Include directory
#INCLUDES = -IInclude
# Add or subtract whatever MSPGCC flags you want. There are plenty more
#######################################################################################
#CFLAGS   = -mmcu=$(MCU) -g -Os -Wall -Wunused $(INCLUDES) -lm
CFLAGS   = -mmcu=$(MCU)     -Os -Wall -Wunused -lm -Wextra 
ASFLAGS  = -mmcu=$(MCU) -x assembler-with-cpp -Wa,-gstabs
LDFLAGS  = -mmcu=$(MCU) -Wl,-Map=$(TARGET).map
########################################################################################
CC      = msp430-gcc
LD      = msp430-ld
AR      = msp430-ar
AS      = msp430-gcc
GASP    = msp430-gasp
NM      = msp430-nm
OBJCOPY = msp430-objcopy
RANLIB  = msp430-ranlib
STRIP   = msp430-strip
SIZE    = msp430-size
READELF = msp430-readelf
OBJDUMP = msp430-objdump.exe
RM      = rm -f
TAGER   = ctag
LOADER  = MSP430Flasher.exe
#MV      = mv
#MAKETXT = srec_cat
#CP      = cp -p


.PHONY: all clean load
all: $(TARGET).elf $(TARGET).hex $(TARGET).list tags 
load: $(TARGET).lad

$(TARGET).elf: $(OBJS)
	@echo ---- Linking $@ ----
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET).elf
	$(SIZE) $(TARGET).elf

%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@

%.list: %.elf
	$(OBJDUMP) -DS $< > $@

tags: $(SOURCE)
	ctags -R

%.lad: %.hex
	@echo "---- Load $< into $(MCU)----"
	$(LOADER) -n $(MCU) -w $(<) -v -z [VCC] && touch $@

include depend


clean:
	$(RM) *.o  *.d *.elf *.bin *.hex *.srec *.list *.orig *.lad tags

