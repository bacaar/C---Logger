# C++ - Logger
Simple logger class with multithreading for C++ - Projects

## Compilation
As this logger uses multithreading, the corresponding library is needed. Link it with *-lpthread*.

If multithreading is not needed/wanted and thus the code should also be able to compile without *-lpthread*, simply set ENABLE_MULTITHREADING in Logger.hpp to 0.

This code was tested with C++20 and gcc 11.2.0 on Windows 10 64-bit
