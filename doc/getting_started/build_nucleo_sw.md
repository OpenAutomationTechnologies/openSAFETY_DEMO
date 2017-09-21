Building the Nucleo software {#page_build_nucleo_sw}
============

This page provides information about building the Nucleo software.

[TOC]

Make sure that all required software of \ref sect_gs_requirements_sw (2) is
installed. In brief:

- Windows only:
 - MinGW
 - ST-LINK USB drivers

- generally:
 - CMake
 - ARM cross compiler
 - libusb
 - stlink tools

# Nucleo F103RB {#sect_build_nucleo_sw_f103}

This section provides information on how to build the software for the
**stm32f103rb** Nucleo board. The following steps need to be carried out
in order to do this:

1. Create a build environment with **bash** shell, the **GNU CoreUtils** and the
   **gcc-arm-none-eabi** compiler. (See \subpage page_install_software)

2. Inside the shell enter the build directory or create a new directory
   and change there. Consider \ref sect_gs_cmake_buildsw_outofsource
   when building for different targets or changing between native and
   cross-compilation.

   - Execute CMake in the build directory with the path to the source directory
     as a parameter.

     If passing other paths than the toolchain file and the source
     directory to **cmake**, make sure to use absolute paths.

     Example starting from the openSAFETY_DEMO root directory:

            > cd [YOUR_NUCLEO_BUILD_DIRECTORY]
            > cmake -G"Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-arm-cortexm3-gnueabi.cmake \
            >       -DCFG_STM32_HAL_LIB_LOC=[PATH_TO_STM32Cube_FW_F1_V1.6.0] ../

     This will generate Unix style Makefiles for the stm32f103rb platform with
     the default configuration. See the section \ref sect_gs_cmake_options and
     \ref sect_gs_nucleo_buildsw_options_f103 for all available options which
     can be passed to CMake. Simply add `-D[YOUR_OPTION]` to the build
     command above.

   - Alternatively, execute `cmake-gui` to do the configuration via the
     graphical interface.

3. After CMake has finished generating the Makefiles it is possible to compile
   the software for your project. This can be done with the following command.

       > make all

4. After the build is finished it is possible to write the executable to the
   local flash memory. (e.g: DEMO_NAME=demo-sn-gpio-upm)
   Use `make help` to print all available make targets.

       > make download-[DEMO_NAME]

   > **Notes for downloading the software:**
   >
   > * The boards have to be powered up **before** they are connected
   >  via USB to the computer.
   >
   > * When downloading software to the boards, it must to be ensured that
   >   only one Nucleo board is connected to the computer during that time.
   >
   > * "WARN src/stlink-common.c: unknown chip id! 0xe0042000"
   >   This indicates that an unknown (or unpowered) micro-controller board is
   >   connected to the host-computer.
   >
   > * "SRAM size: 0x18000 bytes (96 KiB), Flash: 0 bytes (0 KiB) in pages
   >   of 16384 bytes" This indicates that the micro-controller board has been
   >   connected to the host-computer first before it got powered up
   >   by the externalpower-source.
   >
   > * When the application gets stuck on "Restore SOD" the whole
   >   flash memory of the microcontroller should be erased, and the download of
   >   the software must be started again.
   >   Erasing the flash can be done with STM32 ST-LINK Utility:
   >   `Target -> Erase chip` or with the button `Full chip erase`.

## Target specific configuration option (stm32f103rb) {#sect_gs_nucleo_buildsw_options_f103}

See \ref sect_gs_cmake_options for the general configuration options.

Variable Name                    | Description              | Default value
---------------------------------|--------------------------|------------------
CFG_STM32_HAL_LIB_LOC | path to the stm32f103 HAL library | app/target/stm32f1xx
CFG_ARM_BOARD_TYPE | kind of Cortex-M3 board type | stm32f103rb
CFG_PROG_FLASH_ENABLE | enable flashing of the firmware | OFF
CFG_DUAL_CHANNEL | selection between single and dual channelled application | single-channel


- **CFG_STM32_HAL_LIB_LOC**

 Specify the path to the stm32f103 HAL library (**STM32CubeF1 V 1.6.0**).
 Get it from: http://www.st.com/en/embedded-software/stm32cubef1.html

 Value: Path to the HAL library

- **CFG_ARM_BOARD_TYPE**

  Enables to specify the kind of Cortex-M3 board type (e.g: **stm32f103rb**)

  Value: stm32f103rb

- **CFG_PROG_FLASH_ENABLE**

  Enables a custom target to write the firmware to the local flash by using the
  tool **st-flash**.

  Values: ON, OFF

- **CFG_DUAL_CHANNEL**

  Select between single and dual channelled application as well as just the
  up-master and up-slave of the dual channelled application.

  Values: single-channel, dual-channel, up-master, up-slave

# Nucleo F401RE {#sect_build_nucleo_sw_f1401}

This section provides information on how to build the software for the
**stm32f401re** Nucleo board. The following steps need to be carried out
in order to do this:

1. Create a build environment with **bash** shell, the **GNU CoreUtils** and the
   **gcc-arm-none-eabi** compiler. (See \subpage page_install_software)

2. Inside the shell enter the build directory or create a new directory and
   change there.

   - Execute CMake in the build directory with the path to the source directory
     as a parameter.

     If passing other paths than the toolchain file and the source
     directory to **cmake**, make sure to use absolute paths.

     Example starting from the openSAFETY_DEMO root directory:

            > cd [YOUR_NUCLEO_BUILD_DIRECTORY]
            > cmake -G"Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-arm-cortexm4-gnueabi.cmake \
            >       -DCFG_STM32_HAL_LIB_LOC=[PATH_TO_STM32Cube_FW_F4_V1.21.0] ../

     This will generate Unix style Makefiles for the stm32f401re platform with
     the default configuration. See the section \ref sect_gs_cmake_options and
     \ref sect_gs_nucleo_buildsw_options_f401 for all available options which
     can be passed to CMake. Simply add `-D[YOUR_OPTION]` to the build
     command above.

   - Alternatively, execute `cmake-gui` to do the configuration via the
     graphical interface.

3. After CMake has finished generating the Makefiles it is possible to compile
   the software for your project. This can be done with the following command.

       > make all

4. After the build is finished it is possible to write the executable to the
   local flash memory. (e.g: DEMO_NAME=demo-sn-gpio-upm)
   Use `make help` to print all available make targets.

       > make download-[DEMO_NAME]

   > **Notes for downloading the software:**
   >
   > * The boards have to be powered up **before** they are connected
   >  via USB to the computer.
   >
   > * When downloading software to the boards, it must to be ensured that
   >   only one Nucleo board is connected to the computer during that time.
   >
   > * "WARN src/stlink-common.c: unknown chip id! 0xe0042000"
   >   This indicates that an unknown (or unpowered) micro-controller board is
   >   connected to the host-computer.
   >
   > * "SRAM size: 0x18000 bytes (96 KiB), Flash: 0 bytes (0 KiB) in pages
   >   of 16384 bytes" This indicates that the micro-controller board has been
   >   connected to the host-computer first before it got powered up
   >   by the externalpower-source.
   >
   > * When the application gets stuck on "Restore SOD" the whole
   >   flash memory of the microcontroller should be erased, and the download of
   >   the software must be started again.
   >   Erasing the flash can be done with STM32 ST-LINK Utility:
   >   `Target -> Erase chip` or with the button `Full chip erase`.

## Target specific configuration options (stm32f401re) {#sect_gs_nucleo_buildsw_options_f401}

See \ref sect_gs_cmake_options for the general configuration options.

Variable Name                    | Description              | Default value
---------------------------------|--------------------------|------------------
CFG_STM32_HAL_LIB_LOC | path to the stm32f401 HAL library | app/target/stm32f4xx
CFG_ARM_BOARD_TYPE | kind of Cortex-M4 board type | stm32f401re
CFG_PROG_FLASH_ENABLE | enable flashing of the firmware | OFF
CFG_DUAL_CHANNEL | selection between single and dual channelled application | single-channel


- **CFG_STM32_HAL_LIB_LOC**

 Specify the path to the stm32f401 HAL library (**STM32Cube_FW_F4_V1.21.0**).
 Get it from: http://www.st.com/en/embedded-software/stm32cubef4.html

 Value: Path to the HAL library

- **CFG_ARM_BOARD_TYPE**

  Enables to specify the kind of Cortex-M4 board type (e.g: **stm32f401re**)

  Value: stm32f401re

- **CFG_PROG_FLASH_ENABLE**

  Enables a custom target to write the firmware to the local flash by using the
  tool **st-flash**.

  Values: ON, OFF

- **CFG_DUAL_CHANNEL**

  Select between single and dual channelled application as well as just the
  up-master and up-slave of the dual channelled application.

  Values: single-channel, dual-channel, up-master, up-slave

# Debugging the software on the Nucleo board   {#sect_gs_nucleo_buildsw_debug}
It is possible to debug the software using arm-none-eabi-gdb, the st-link tools
and the **Eclipse IDE**. With this you should be able to comfortably step
through the code and set breakpoints.

1. Set the CMake generator to: **Eclipse CDT4 - Unix Makefiles**
   - This will generate an additional Eclipse project with the standard
     Unix Makefiles.

2. Get Eclipse from https://eclipse.org
   - The Eclipse IDE needs to be started from a shell where the path is set
     to all tools needed to compile build and run the demos on an stm32.

3. Import generated CMake Eclipse project into the Eclipse IDE with:
   - File -> Import -> Existing Projects Into Workspace.
   - Set 'Select root directory' to the CMake build folder.
   - Select the found project and hit 'Finish'

4. Open a second shell start the tool **st-util**. This tool is a gdb server
   which enables you to connect with the gdb client (e.g: arm-none-eabi-gdb).
   The tool st-util listens on port 4242 with not set differently.

5. Define a new 'Debug Configuration' in Eclipse:
   - Run -> Debug Configuration -> C/C++ Remote Application
   - In the 'Main' tab select the **elf** file as an application.
   - In the 'Debugger' tab set the following:
     * GDB debugger = arm-none-eabi-gdb
     * Connection / Host = localhost
     * Connection / Port = 4242

6. \b -> Step through the code and find the bug!

> **Note:** If debugging isn't possible in the described way with
> C/C++ Remote Application Debug Configuration go to
> `Help -> Install New Software`.
> In the new window select the entry that matches your version of Eclipse
> e.g. "[VERSION] - https://download.eclipse.org/releases/[VERSION]" from the
> `Work with` drop down menu.
> Enter "hardware" in the filter text box and install
> `C/C++ GDB Hardware Debugging` from the section
> `Mobile and Device Development`.
> Now a new Debug Configuration can be added (GDB Hardware Debugging):
> In the 'Main' tab select your .elf file you want to debug.
> In the 'Debugger' tab make the following settings:
> - GDB Command: arm-none-eabi-gdb
> - check use remote target checkbox
> - Host name or IP address: localhost
> - Port number: 4242

# Viewing the debug output of the Nucleo board {#sect_gs_nucleo_buildsw_debug_virtual_com}

When the software was built in `Debug` configuration, it is possible to view
the status / debug output of the openSAFETY_DEMO application processors:

- Connect the Nucleo board via USB to the computer
- Open a terminal program and open the virtual COM port related to the
  Nucleo board

The following settings are needed for the COM port:
- Port: Virtual COM port of the Nucleo board
- Baud rate: 115200
- Data: 8bit
- Paraity: none
- Stop: 1 bit
- Flow control: none

Depending on the used terminal program, it may be necessary to set the new-line
recognition to `\n` or `line feed` for correct view of the output.