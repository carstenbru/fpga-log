fpga-log
========
**fpga-log is a modular datalogger based on an FPGA.**

Modular means that you can simply add different modules to your logger, configure and connect them to get the **datalogger which fits your needs!**  
The modules can have various functions. For example there are modules available to connect to devices like an ADC (analog-to-digital converter), modules to store the data (e.g. on an sd-card) and other modules to process the data and control the data fetching like filters or timers.  
Additionally the modular design makes it very **easy to add new devices, data sinks or any other function.**

Since one aim of the project is to build an **easy-to-use** datalogger it comes with a graphical configuration utility. This should enable even inexperienced users without knowledge of FPGAs and even without programming skills to use fpga-log.

*This product includes software developed by the TU Dresden Chair for Embedded Systems and its contributors.*

## Why on an FPGA?
An FPGA has the advantage of very high flexibility which supports the modular design. In a classical microcontroller approach you are always limited to the peripherals the uC offers. Sure, you can always buy the next more powerful device but at some point this will get very difficult: imagine you have to log data from 10 UART devices. Also if you now want to use the logger in another situation you very likely have to get a new one.  
In contrast an FPGA is only limited by the logic it can carry and the pins it offers. Also with an FPGA some functions can be realized which cannnot be done with a microcontroller, e.g. timestamps for the incoming data are generated in hardware and can as consequence contain the exact clock-cylce of the event!  
Of course an FPGA has also some drawbacks. Implementing everything in hardware would fastly lead to a big design and need an very large FPGA. But since an FPGA is freely configurable logic you can also build a microcontroller inside this chip. For that the SpartanMC SoC-Kit is used (http://www.mr.inf.tu-dresden.de/forschung/spartanmc/).  
So all in all we have a uC with exactly the peripherals we need and everything which needs to be done in hardware (or can be accelerated by hardware) can be done in hardware, everything else in software. Of course the user has nothing to do with the hardware generation, this is done automatically in the background by the configuration tool and the SpartanMC SoC-Kit.

The project is developped on a Papilio Pro FPGA-board (http://papilio.cc) but should work on any board which is supported by SpartanMC or is added to this kit by you (most boards with a Xilinx Spartan FPGA should work).

## Installation
fpga-log need the SpartanMC SoC-Kit. As consequence it only works on linux based systems. So before installing fpga-log you have to download SpartanMC (http://www.mr.inf.tu-dresden.de/forschung/spartanmc/) and follow their installation instructions.

After this you can install fpga-log by running the install scipt located in the root folder of the project. This will link the new hardware peripherals and the fpga-log software library to the SpartanMC directories.

To use the configuration tool you have to compile it as the repository only contains the source. The simplest way to do this is to open the project file "config-tool/config-tool.pro" with Qt Creator (http://qt-project.org/downloads#qt-creator) and start the build.

## Directories
directory		|description
------------------------|------------
bin			|binaries, currently only a patched gcc for SpartanMC
config-tool-files	|files needed by the configuration utility (system template, module descriptions,...)
config-tool		|configuration utility source code (C++ with Qt)
doxygen			|doxygen file to generate a documentation of fpga-log (not config-tool)
PC			|addtional files to compile for PC (native), see Simulation
source			|source code of fpga-log (library for SpartanMC written in C)
spartanmc		|additional hardware (peripherals for SpartanMC, verilog code)
spmc-soc-kit-beta	|adaption of SpartanMC (inofficial beta version) to Papilio Pro board 
spmc-soc-kit-online	|adaption of SpartanMC (official downloadable version) to Papilio Pro board
testsystem		|a test system as example project and to test new modules
uml			|UML diagram of the fpga-log library

## Simulation
The project can also be compiled to run natively on a linux PC. The purpose of this future is easier developpment of new modules so this is only for experienced users with at least programming skills and if the module needs hardware support also some knowledge about FPGAs.  
To compile you can use the Makefile in the directory "PC" or the eclipse project submitted in this repository. The eclipse project also contains targets for the FPGA device.  
Most peripherals are simulated using linux fifos (pipes). One pipe per direction and device is generated (see "pc_peripherals.h" file generated by config-tool for numbering). This way you can simulate your hardware in almost every way you like: a simulation program writing to/reading from the pipes, by hand from a simple terminal window or even with a real physical device by redirecting the fifos to that hardware device, e.g. with:  
```
cat fifo_out_X > /dev/DEVICE_FILE
cat /dev/DEVICE_FILE > fifo_in_X
```
