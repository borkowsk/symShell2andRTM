# symShell2andRTM
##Currently actively developed version of Symulation Shell and RTM function &amp; classes used in my C++ simulation projects.

The WBRTM and SYMSHELL libraries were created in the 90s of the twentieth century, when the variety of C/C++ compilers 
made it very difficult to use the same code on DOS/Windows and different unix/linux variants.
In those days, wide deficiencies in the language standard necessitated additions, which today are already standardly 
available, and minor differences in syntax and library extensions necessitated conditional compilation.
Many traces of those ancient ideas still remain in the library.

Adaptation of the library to the C++11 standard is slow, as most of the simulation applications based on it 
are no longer used. Unchecked codes are placed in the documentation in the **OBSOLETE** group and marked with 
a warning when compiling.

Requirements
------------

C++v11, cmake, or clang, x11 for SYMSHELL graphics.

Tools:  *DEBIAN*

        sudo apt install cmake
        sudo apt install ninja-build
        open https://clang.llvm.org/get_started.html

Lib X11: *DEBIAN/UBUNTU*
        
        sudo apt install libx11-dev
        sudo apt install libxpm-dev

Lib X11: *CentOS/RHEL?*

        sudo yum install libX11-devel
        sudo yum install libXpm-devel
        
Lib X11: Apple?
        
        X11 or text/svg mode only.

Lib X11: Windows
   
        Not needed. Native graphics supported.        

LICENCE
-------

Currently only for compilation my publicly available simulation programs
 
