# Chatroom V1 for CS4850 - Networks @ Mizzou

## Compilation Instructions:

   ### Requirements:
   
   * Note: this program was written on an intel Mac running MacOS 13.2.1    
   * GCC/G++v11 or higher
   * CMakeList version 3.8 or higher

1) Clone or download this repository to your local disk
2) `cd` into the project (i.e. `cd Chatroom-V1`) 
3) run `mkdir build` then `cd build` 
4) Once in the build directory: run `cmake ..` to link all libraries needed
5) Run `make` which will compile everything into 2 executables : (1) server (2) client
7) Execute `./server` first to open a socket
8) With the server still running, in another terimal instance: run `./client`
9) In the running `client` instance you can then start using the chatroom and it's commands:
                   Commands: {`login`, `newuser`, `send`, `logout`}

