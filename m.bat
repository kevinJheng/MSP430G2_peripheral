@echo off

call config.bat
make %1 %2 %3 %4 |sed -r "s/\-I [^ ]+ //g"
