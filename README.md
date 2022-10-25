# C++ - Logger
Simple logger class with multithreading for C++ - Projects

## Compilation
As this logger works with files and multithreading, two additional libraries are needed. Link them with *-lboost_filesystem* and *-lpthread*.

If multithreading is not needed/wanted and thus the code should also be able to compile without *-lpthread*, simply set ENABLE_MULTITHREADING in Logger.hpp to 0.

This code was tested with C++17 and gcc 9.4.0 on Ubuntu 20.04.01
