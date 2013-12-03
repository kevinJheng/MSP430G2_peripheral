@echo off

cat sourcesList.txt | tr '\n' ' ' | sed -f script\fixsource.sed > sources.mak
@echo ----content of sources.mak----
cat sources.mak
echo.

cat sourcesList.txt | xargs msp430-gcc -MM | sed -f script\fixdepend.sed > depend.mak
rem find .|grep '\.[cC]$'|xargs msp430-gcc -MM > depend
@echo ----content of depend.mak----
cat depend.mak


