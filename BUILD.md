# Building srcPtr

srcPtr is built with CMake (www.cmake.org). The following instructions are designed for a Unix-based OS.

1. Clone the repository and install CMake. If you are on a Debian-based Linux distribution, run `sudo apt-get install cmake`. Otherwise, get it from the website or your respective package manager. 

2. Once CMake is installed, navigate to the folder you want to generate build files in and run `cmake path_to_srcptr`, where path_to_srcptr is the path to the cloned srcPtr repository.

3. When CMake finishes generating the build files, run the command `make`. Upon completion, the `srcptr` executable will be in the bin folder. The commands `make clean` and `make test` are also supported with their expected operation. 
