# srcPtr
A lightweight, fast, and flexible 'close to the code' pointer analysis tool.


### Installation

srcPtr is available as a Linux binary and preconfigured virtual machine image. Otherwise, you can build it yourself.

#### Binary
[Ubuntu 16.04.3 x64](http://www.sdml.cs.kent.edu/lmcrs/srcPtr/srcPtr-Ubuntu16.04-64.zip)

#### Virtual Machine
[VM Image](http://www.sdml.cs.kent.edu/lmcrs/srcPtr/srcPtr_VM.zip)
( username: user, password: srcptr )

#### Building 

To build srcPtr yourself, first clone srcPtr by running  `git clone --recursive https://github.com/srcML/srcPtr.git`

srcPtr uses cmake for building. To build srcptr create a build folder anywhere and go inside it. Then run the command 

`cmake path_to_srcptr_dir`

replace path_to_srcptr_dir with the path to the srcPtr directory.

After this, cmake should have generated the proper make files. Now run `make`. The srcPtr executable will be built and will appear in the bin directory. 
