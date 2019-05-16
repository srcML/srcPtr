# srcPtr
A lightweight, fast, and flexible 'close to the code' pointer analysis tool.


### Installation

srcPtr is available as a Linux binary and preconfigured virtual machine image. Otherwise, you can build it yourself.

#### Binary
[Ubuntu 18.04.1 x64](http://www.sdml.cs.kent.edu/lmcrs/srcPtr/srcptr_ubuntu_18-04.zip)

#### Virtual Machine
[VM Image](http://www.sdml.cs.kent.edu/lmcrs/srcPtr/srcptr_vm_ubuntu_18-04.zip)
( username: user, password: srcptr )

#### Building 

To build srcPtr yourself, first clone srcPtr by running  `git clone --recursive https://github.com/srcML/srcPtr.git`

srcPtr uses cmake for building. To build srcptr create a build folder anywhere and go inside it. Then run the command 

`cmake path_to_srcptr_dir`

replace path_to_srcptr_dir with the path to the srcPtr directory.

After this, cmake should have generated the proper make files. Now run `make`. The srcPtr executable will be built and will appear in the bin directory. 
