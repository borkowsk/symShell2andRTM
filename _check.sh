#!/bin/bash
#Checking for required dependencies
echo "Running" `realpath $0`

# SIMPLE CHECK FOR WINDOWS
env | grep Windows_NT >> check_out.tmp
if [[ "$?" == 0 ]]
then #TO JEST WINDOWS
	echo "Under MS Windows only MS VISUAL STUDIO with C++ & cmake plugins is required & tested".
	
	"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" ./RTM/
	if [[ "$?" == 0 ]]
	then
		echo "It looks like you have MS VC++ instaled".
		exit
	fi

	"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\devenv.exe" ./RTM/
	if [[ "$?" == 0 ]]
	then
		echo "It looks like you have MS VC++ instaled".
		exit
	fi
	
	echo "Searching C:\Program files"
	find "/c/Program Files/" -name devenv.exe > msvc.lst
	CNT=`cat msvc.lst | wc -l - `
	echo #"\"$CNT\""
	if [[ "$CNT" == "1 -" ]]
	then
		head msvc.lst
	else
		
		echo "Searching C:\Program files (x86)"
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
		echo "It looks like you have MS VC++ instaled".
	fi
	
	exit
fi


EDIT=nano

set -e # https://intoli.com/blog/exit-on-errors-in-bash-scripts/
echo -e "\n\tThis script stops on any error!\n\tWhen it stop, remove source of the error & run it again!\n"
     
echo -e "Test for required software:\n" 
gcc --version 
echo -e "\n GCC  OK\n"
g++ --version
echo -e "\n G++  OK\n"
make --version 
echo -e "\n MAKE  OK\n"
cmake --version 
echo -e "\n CMAKE  OK\n"

echo -e "Test for required library packages:\n"
cat << EOF >> tmpX11.cpp
#include <X11/Xlib.h>   // sudo apt install libx11-dev ? 
#include <X11/Xutil.h>
#include <X11/xpm.h>    // sudo apt install libxpm-dev ?
EOF

g++ -c tmpX11.cpp
echo -e "\n libxpm-dev  OK\n"

rm -f tmpX11.cpp tmpX11.o*

#Dependencies installation under UBUNTU:
# $ sudo apt install gcc
# $ sudo apt install g++
# $ sudo apt install make
# $ sudo apt install cmake
# echo Instaling development versions of non standard libraries
# echo libxpm-dev depends on libx11-dev, so both will be installed by this command:
# $ sudo apt install libxpm-dev

