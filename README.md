# RPi2-FreeRTOS

## Original README
Latest stable (9.0.0) FreeRTOS port on Raspberry Pi 

Tested on Raspbery Pi 2B with uart bootloader by David Welch - https://github.com/dwelch67/raspberrypi

This port is based on https://github.com/Forty-Tw0/RaspberryPi-FreeRTOS and https://github.com/tonyhccdev/RaspberryPi-FreeRTOS

## New README

# Introduction

This repistory is a fork of the main rep, and has extended functionnality, for instance it includes 1 benchmark task from **TACLEBENCH** and a few mains, depending on what you want to run on the RPI. Furthermore, it includes drivers for **spi** and **mcp2515** to make 2 or more RPI 2 communicate together over CAN BUS.

# How to install and run FreeRTOS on a RPI 2

Before using the command make, ensure that you have a "main.c" file in the /Demo folder, just rename a "..._main.c" file to "main.c"

Clone the repository

```

git clone https://github.com/Davidpereira2803/RPi2-FreeRTOS-CAN.git

```

Go to the repository

```

cd RPI-2FreeRTOS-CAN

```

Type the make command

```

make

```

You should see something similar to this:

**arm-none-eabi-size kernel7.elf**  

**text	   data	    bss	    dec	    hex	filename**  

**20592	     12	   5484	  26088	   65e8	kernel7.elf**  

Copy the kernel7.img file to the SD-Card of your RPI and turn the RPI on, rename the original kernel7.img file to still keep the standard OS functional.

Note: The SD-Card should be flashed and have running Raspbian OS on it, for the default configuration and bin files.

# CAN BUS Module
The project requires CAN BUS modules, for instance we used MCP2515 CAN controller and TJA1050 CAN Transceiver. 

Refer to this repository to see the changes required if you choose a 5V module: [CANBus-MCP2515-Modification](https://github.com/tolgakarakurt/CANBus-MCP2515-Raspi/tree/master?tab=readme-ov-file)