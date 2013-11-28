del msp430.hex
@echo off
rem msp430-gcc -mmcu=msp430g2553 -O2 -Wall -g main.c -o msp430.elf  -fno-builtin-log  

msp430-gcc -mmcu=msp430g2553 -O2 -Wall -g main.c -o msp430.elf -lm
msp430-objcopy -O ihex msp430.elf msp430.hex
msp430-objdump -DS msp430.elf > msp430.lst

