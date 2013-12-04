#Enter SubDir here
#SUBDIR := script 
#SUBDIR += .
#---------locate source & Objs
#SOURCE := $(wildcard \
#    	  	$(addsuffix /*.c,$(SUBDIR) )  \
#           )
SHELL := cmd.exe

include sources.mak
OBJS := $(notdir $(SOURCE:.c=.o))

#-----------------------------

TARGET     = LaunchpadRev1_5
MCU        = msp430g2553
# List all the source files here
# eg if you have a source file foo.c then list it here
# Include are located in the Include directory

#INCLUDES := include
include searchpath.mak

# Add or subtract whatever MSPGCC flags you want. There are plenty more
#######################################################################################
#CFLAGS   = -mmcu=$(MCU) -g -Os -Wall -Wunused $(INCLUDES) -lm
CFLAGS   = -mmcu=$(MCU) -Os -Wall -Wunused -lm -Wextra $(INCLUDES)
ASFLAGS  = -mmcu=$(MCU) -x assembler-with-cpp -Wa,-gstabs
LDFLAGS  = -mmcu=$(MCU) -Wl,-Map=$(TARGET).map
########################################################################################
CC      = msp430-gcc
OBJCOPY = msp430-objcopy
SIZE    = msp430-size
OBJDUMP = msp430-objdump.exe
RM      = rm -f
TAGER   = ctag
LOADER  = MSP430Flasher.exe

#LD      = msp430-ld
#AR      = msp430-ar
#AS      = msp430-gcc
#GASP    = msp430-gasp
#NM      = msp430-nm
#RANLIB  = msp430-ranlib
#STRIP   = msp430-strip
#READELF = msp430-readelf
#MV      = mv
#MAKETXT = srec_cat
#CP      = cp -p


.PHONY: all clean load dev
all: $(TARGET).elf $(TARGET).hex $(TARGET).list tags 
load: $(TARGET).lad
# link all objects into an elf File & report size
$(TARGET).elf: $(OBJS)
	@echo ---- Linking $@ ----
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET).elf
	$(SIZE) $(@)

#generate hex format
%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@

#generate source list file
%.list: %.elf
	$(OBJDUMP) -DS $< > $@

#generate load record & load into MCU
%.lad: %.hex	
	@echo "---- Load $< into $(MCU)----"
	$(LOADER) -n $(MCU) -w $(<) -v -z [VCC] && touch $@

tags: $(SOURCE)
	ctags -R

include depend.mak

#for development purpose
dev:
	@echo INCLUDES=$(INCLUDES)

clean:
	$(RM) *.mak *.o  *.d *.elf *.bin *.hex *.srec *.list *.orig *.lad tags *.tmp

