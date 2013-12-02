@echo off
echo.

find .|grep '\.[cC]$'| tr '\n' ' ' | sed -f script\fixsource.sed > sources
@echo ----content of sources----
cat sources
echo.

find .|grep '\.[cC]$'|xargs msp430-gcc -MM | sed -f script\fixdepend.sed > depend
rem find .|grep '\.[cC]$'|xargs msp430-gcc -MM > depend
@echo ----content of depend----
cat depend


