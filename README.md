MSP430G2_peripheral
===================

to build a MSP430G2(or above) line's lib as eaiser as possible


auto detect & grenerate dependency Tool: build.bat
    it generates file "sources" & "depend" which describe all source files and it's dependency recpectively. and the makefile read them as the compilation parameter. 
    simplicity:
	1.detect file & it's dependency: config.bat
	2.compile project: make
    consequently: auto generate proper makefile -> compile -> load into MSP430

    requirement: make、find、sed、grep、tr
    (all included in PowerShell)
