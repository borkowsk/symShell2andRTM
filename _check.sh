#!/bin/bash
#Checking for required dependencies
echo "Running" `realpath $0`
source "screen.ini"

# SIMPLE CHECK FOR WINDOWS
env | grep Windows_NT >> check_out.tmp
if [[ "$?" == 0 ]]
then #TO JEST WINDOWS
	echo -e $COLOR1"Under MS Windows only MS VISUAL STUDIO with C++ & cmake plugins is required & tested".$NORMCO
	
	echo -e "\n"'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\'

	"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" ./RTM/
	if [[ "$?" == 0 ]]
	then
		echo -e $COLOR2"It looks like you have MS VC++ instaled."$NORMCO
		exit
	fi

	echo -e "\n"'C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\'

	"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\devenv.exe" ./RTM/
	if [[ "$?" == 0 ]]
	then
		echo -e $COLOR2"It looks like you have MS VC++ instaled."$NORMCO
		exit
	fi

	echo -e $COLOR1"\nSearching 'C:\Program files'"$NORMCO
	find "/c/Program Files/" -name devenv.exe > msvc.lst
	CNT=`cat msvc.lst | wc -l - `
	echo #"\"$CNT\""
	if [[ "$CNT" == "1 -" ]]
	then
		head msvc.lst
	else
		
		echo -e $COLOR1"Searching C:\Program files (x86)"$NORMCO
		find "/c/Program Files (x86)/" -name devenv.exe > msvc.lst
		CNT=`cat msvc.lst | wc -l - `
		echo #"\"$CNT\""

		if [[ "$CNT" == "1 -" ]]
		then
			head msvc.lst
		fi
	fi
	
	"`head msvc.lst`" ./RTM/
	
	if [[ "$?" == 0 ]]
	then
		echo -e $COLOR2"It looks like you have MS VC++ instaled".$NORMCO
	fi
	
	exit
fi


EDIT=nano

set -e # https://intoli.com/blog/exit-on-errors-in-bash-scripts/
echo -e $COLOR2"\n\tThis script stops on any error!\n\tWhen it stop, remove source of the error & run it again!\n"$NORMCO
     
echo -e $COLOR1"Test for required software:\n"$NORMCO

gcc --version 
echo -e $COLOR2"\n GCC  OK\n"$NORMCO
g++ --version
echo -e $COLOR2"\n G++  OK\n"$NORMCO
make --version 
echo -e $COLOR2"\n MAKE  OK\n"$NORMCO
cmake --version 
echo -e $COLOR2"\n CMAKE  OK\n"$NORMCO

echo -e $COLOR1"Test for required library packages:\n"$NORMCO

cat << EOF >> tmpX11.cpp
#include <X11/Xlib.h>   // sudo apt install libx11-dev ? 
#include <X11/Xutil.h>
#include <X11/xpm.h>    // sudo apt install libxpm-dev ?
EOF

g++ -c tmpX11.cpp
echo -e $COLOR1"\n libxpm-dev  OK\n"$NORMCO

rm -f tmpX11.cpp tmpX11.o*

#Dependencies installation under UBUNTU:
# $ sudo apt install gcc
# $ sudo apt install g++
# $ sudo apt install make
# $ sudo apt install cmake
# echo Instaling development versions of non standard libraries
# echo libxpm-dev depends on libx11-dev, so both will be installed by this command:
# $ sudo apt install libxpm-dev

