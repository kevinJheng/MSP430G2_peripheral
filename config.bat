@echo off


@echo ----gen. content of sources.mak----
cat sourcesList.txt|sort|uniq|grep -iv '\.h$'|tr '\n' ' '| sed -f script\fixsource.sed > sources.mak
rem cat sources.mak



@echo ----gen. content of searchpath.mak----
cat sourcesList.txt | sed -rf script/dir.sed | sort|uniq > c_path.list
cat c_path.list | sed -rf script/dir2.sed | tr '\n' ' ' |sed "s/\./-I ./g" > searchpath.mak


@echo ----gen. content of depend.mak----
cat sources.mak|tr '\r' ' ' | sed -e "s/SOURCE :=//" -e "s/^/msp430-gcc -MM/" > mmcheck.bat
cat searchpath.mak >> mmCheck.bat
call mmCheck.bat |sed -f script\fixdepend.sed -e "/msp430-gcc/d" > depend.mak


rem post-process
rm mmCheck.bat
cat searchpath.mak | sed "s/^/INCLUDES := /" > searchpath2.mak
mv searchpath2.mak searchpath.mak




