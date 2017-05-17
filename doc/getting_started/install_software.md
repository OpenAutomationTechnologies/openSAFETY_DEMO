Software Installation {#page_install_software}
============

This page provides installation information for the used software.
The given list represents the full set of software needed for using
the openSAFETY_DEMO and all of it's features.


[TOC]

# General Tools {#sect_gs_install_software_generaltools}

## CMake {#sect_gs_install_software_cmake}

Depending on the used operating system, install CMake via the system's
package manager or download and install the program from the provided
download link. If not already added, CMake's **bin** folder may be added to the
path variable.
Information on how to use CMake can be found at \ref sect_cmake.

## Doxygen {#sect_gs_install_software_doxygen}

Depending on the used operating system, install Doxygen via the system's
package manager or download and install the program from the provided
download link. If not already added, Doxygen's **bin** folder may be added to
the path variable.

## Graphviz {#sect_gs_install_software_graphviz}

Depending on the used operating system, install Graphviz via the system's
package manager or download and install the program from the provided
download link. If not already added, Graphviz' **bin** folder may be added to
the path variable.

## Windows only: MinGW + MSYS {#sect_gs_install_software_msys}

- Execute the installer
- Mark the following packages:
  * mingw32-base
  * mingw-developer-toolkit
  * msys-base
  * mingw32-gcc-g++ (To compile stlink)
- From the menu, select `Installation -> Apply Changes` to install the packages
- Open the file: **C:\\MinGW\\msys\\1.0\\etc\\fstab** and
  uncomment: `c:/mingw /mingw`
  If the file does not exist, copy the file fstab.sample, rename it to fstab
  and do the described change.
- If desired, create a shortcut to **C:\\MinGW\\msys\\1.0\\msys.bat** for easier
  access to the MinGW/MSYS shell.

## Python + needed packages {#sect_gs_install_software_python}

Execute the installer for installing Python or install Python from the package
manager.

There are different ways to download an install Python packages, two of them
are described here:

### Manual download and installation:

 - Download the needed Python packages
 - Extract the tar.gz archives to a desired location.
   To extract tar.gz archives, open a shell and type

        > tar -zxvf FILENAME

 - Change to the directory containing the extracted package data
 - To install the package, type

        > python setup.py install

 - Repeat this steps for all needed Python packages.

### Download and installation via pip :

 - If the Python package `pip` is already installed on the system,
   type

        > pip install PACKAGENAME

   or

        > python-pip install PACKAGENAME

   for downloading and installing each required package,
   where `PACKAGENAME` is the name of the Python package to install.


# openSAFETY Demo related Software {#sect_gs_install_software_demorelated}

If openSAFETY distribution and openPOWERLINK stack were downloaded as archives,
follow the instructions below.

## openPOWERLINK stack {#sect_gs_install_software_openPOWERLINK_stack}

  Extract the content of the downloaded archive to the
  `blackchannel/POWERLINK/stacks/openPOWERLINK` subdirectory located
  in the openSAFETY_DEMO folder.


## openSAFETY stack / distribution {#sect_gs_install_software_openSAFETY_stack}

  Extract the content of the downloaded archive to the
  `openSAFETY` subdirectory located in the openSAFETY_DEMO folder.

# Nucleo related Tools {#sect_gs_install_software_nucleo_tools}

## Windows only : ST-LINK/V2-1 USB Drivers {#sect_gs_install_software_stlink_usbdriver}

- Extract the content of the downloaded archive and execute the
  batch file `stlink_winusb_install.bat`

## libusb {#sect_gs_install_software_libusb}

### Install libusb on Linux via package manager

Install the library `libusb-dev` via the system's package manager.

The example below shows the installation of the library using `apt-get`.
Root access may be required.

    > apt-get install libusb-dev

### Build libusb on Linux

- To extract the downloaded tarball, open a shell, change into the directory
  where the libusb tarball is located and type

        > tar -jxvf libusb-1.0.20.tar.bz2

- Change into the extracted libusb directory and type

        > ./configure

  If configure prints an error
  `configure: error: "udev support requested but libudev not installed"`
  install the library `libudev-dev` via the package manager.

- To compile and install the library, type

        > make
        > make install

### Install precompiled libusb on Windows

 - Download the precompiled Windows binaries from https://sourceforge.net/projects/libusb/files/libusb-1.0/libusb-1.0.20/libusb-1.0.20.7z/download
 - Copy the contents of the archive to the location stated
   in the file `README.txt`

### Build libusb on Windows
 - Make sure MinGW and the required packages are installed.
 - Add the following line to `C:\MinGW\msys\1.0\etc\fstab`

        c:/mingw        /mingw

 - To extract the downloaded tarball, open a MinGW shell,
   change into the directory
   where the libusb tarball is located and type

        > tar -jxvf libusb-1.0.20.tar.bz2

 - Change to the libusb source directory and type

        > ./configure --prefix=/mingw
        > make
        > make install

   > **libusb build issues for Windows:**
   > On some systems, while trying to
   > build libusb, ./configure may cause wrong results while checking for
   > "struct timespec". To solve this issue, please remove the line
   > `#define HAVE_STRUCT_TIMESPEC 1` in the file config.h.
   >
   > To fix errors like *"In function 'get_windows_version':
   > os/windows_usb.c:850:4: error: implicit declaration of function
   > 'VerSetConditionMask' [-Werror=implicit-function-declaration] major_equal
   > = VerSetConditionMask(0, VER_MAJORVERSION, VER_EQUAL);"*:
   > add the following defines with the appropriate value to
   > the file **config.h** and build libusb again.
   >
   > ~~~~~~~{.c}
   > #define WINVER 0x0601
   > #define _WIN32_WINNT 0x0601
   > ~~~~~~~
   >
   > - Windows 7: 0x0601
   > - Windows 8: 0x0602
   > - Windows 8.1: 0x0603
   > - for other versions see https://msdn.microsoft.com/en-us/library/aa383745.aspx


## stlink tools {#sect_gs_install_software_stlinktools}

### Build stlink tools on Linux

- Make sure the following software is installed, if something is missing,
  the packages can be installed from the system's package manager:

   - libusb
   - automake
   - autoconf
   - pkg-config

- Get the stlink sources by downloading and extracting the source archive or
  obtain stlink via git:

       > git clone https://github.com/texane/stlink.git stlink-utility

- Change into the stlink source directory and compile stlink:

       > cd STLINK_DIR
       > ./autogen.sh
       > ./configure
       > make
       > make install

#### Access Rights
- To enable the access to the Nucleo USB device via stlink tools as
  non-privileged user, add a `udev` rule
  `49-stlinkv2-1.rules` to `etc/udev/rules.d` with the content shown below.
  Root access may be rquired.

        SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374b", \
            MODE:="0666", \
            SYMLINK+="stlinkv2-1_%n"

- Set the rights to `644`:

        > chmod 644 49-stlinkv2-1.rules

- Reload the udev rules by executing

        > udevadm control --reload-rules

- Unplug and reconnect the USB cable again

### Install precompiled stlink tools on Windows

See https://github.com/texane/stlink/wiki for pre-built Windows binaries and
install   the package to **C:\\MinGW** (or set your environment variables
accordingly).

### Build stlink tools on Windows

This section gives details on how to compile the **stlink** tools on
Windows with **MinGw**:

- Download the following packages and extract the archive contents to `C:\MinGW`

  * http://win32builder.gnome.org/packages/3.6/glib_2.34.3-1_win32.zip
  * http://win32builder.gnome.org/packages/3.6/pkg-config_0.28-1_win32.zip
  * http://win32builder.gnome.org/packages/3.6/pkg-config-dev_0.28-1_win32.zip

- Get the stlink project by downloading and extracting the source archive or
   get it from github:

        > $ git clone https://github.com/texane/stlink.git stlink-utility

- Investigate the file INSTALL.mingw and execute each step.
   Basically the following steps need to be carried out:

        > cd STLINK_DIR
        > ./autogen.sh
        > ./configure --prefix=/mingw
        > make
        > make install

   * If `./configure` fails with
     *PKG_CHECK_MODULES USB unexpected token!*
     Add the line: *ACLOCAL_AMFLAGS = -I /mingw/share/aclocal* to the
     file **Makefile.am**.

> **Note:** The stlink tools rely on libusb to access the ST-LINK/V2.
> Older versions of libusb have trouble accessing ST-LINK/V2 debug adaptors
> when they are connected to an USB 3.0 port (even with an USB 2.0 hub in
> between). In this case the error message "Couldn't find any ST-Link/V2
> devices" will be shown.
> To solve this issue, a newer version of libusb has to be used.
> Version 1.0.19 has been tested to be successful.
>
> See files INSTALL in libusb sources and INSTALL.mingw in the stlink
> sources for building libusb.

> **stlink build issues for Windows:**
> To fix errors like *"In file included from gdbserver/gdb-remote.c:13:0:
> ./mingw/mingw.h:68:20: error: conflicting types for 'sleep'
> static inline void sleep(unsigned ms) { Sleep(ms); }"*:
> Comment out the line `static inline void sleep(unsigned ms) { Sleep(ms); }`
> in the file **stlink_PATH/mingw/mingw.h** and
> build stlink tools again.


## HAL Library {#sect_gs_install_software_hallib}

Download the library suitable for the microcontroller you are using
and extract the content of the archive to any desired location.
This location has to be selected afterwards, when building the firmware for
the microcontroller.

 > **IMPORTANT**: There is a bug in Version 1.2.0 of the STM32CubeF1 software.
 > Open the file
 > `[STM32CubeDirectory]/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi.c`
 > and comment the following lines in function `HAL_SPI_Receive_DMA()`:
 >
 > ~~~~~~~~~~~~~~~~~~~~~{.c}
 >   /*hspi->hdmatx->XferHalfCpltCallback = 0;*/
 >   /*hspi->hdmatx->XferCpltCallback     = 0;*/
 >   /*hspi->hdmatx->XferErrorCallback    = 0;*/
 > ~~~~~~~~~~~~~~~~~~~~~

## GNU ARM Compiler {#sect_gs_install_software_gnuarm}

### Windows:
- Download the GCC ARM Embedded toolchain (Windows installer or Windows zip package) from:
     https://launchpad.net/gcc-arm-embedded/+download
- Install the package to **C:\\MinGW** (or set your environment variables
  accordingly).

### Linux:
- Download the GCC ARM Embedded toolchain (Linux installation tarball) from:
     https://launchpad.net/gcc-arm-embedded/+download
- Unpack the tarball to tge desired install directory. See the readme.txt file
  at the download link for further information.


## Windows only: STM32 ST-LINK Utility {#sect_gs_install_software_stlinkutility}

Get the STM32 ST-LINK Utility from http://www.st.com/en/embedded-software/stsw-link004.html
and install the software.


# Altera Tools {#sect_gs_install_software_alteratools}

## Quartus 13.0 SP1 {#sect_gs_install_software_quartus}

Make sure the Quartus installer as well as the Cyclone IV package are downloaded
and located in the same folder. Execute the installer and when prompted,
select the the Cyclone IV hardware package and continue installation.

![Quartus Installation, Device Support](quartus_device_support.png)
@image latex quartus_device_support.png "Quartus Installation, Device Support" width=0.75\textwidth

## Quartus Programmer {#sect_gs_install_software_quartus_programmer}

If using the Quartus 13.0 SP1 Toolchain, the Quartus Programmer is already
part of the installation. Therefore a separate installation is not necessary.

If Quartus 13.0 SP1 Toolchain is not used / installed, the Quartus Programmer
may be installed for downloading / flashing  the FPGA bitstream and software.

## Altera USB-Blaster {#sect_gs_install_software_quartus_driver}

### Windows Driver Installation

The Windows driver for the USB-Blaster can be installed manually, if it wasn't
installed via the installation of Quartus or the Quartus Programmer.

Carry out the following steps to install the USB-Blaster driver:

- power up the board and connect it to the computer via USB
- open the computer's device manager
- find the Altera USB-Blaster device, right-click and select `Update Driver Software`
- select to browse the computer for driver software
- depending on your software installation:
  * Quartus is installed: select the directory `QUARTUS_INSTALL_DIR\quartus\drivers`
  * Quartus Programmer is installed: select the directory `QUARTUS_INSTALL_DIR\qprogrammer\drivers`
- carry on with the driver installation

### Linux Setup for Altera USB-Blaster

#### Access Rights

- To enable the access to the Terasic USB-Blaster as non-privileged user,
  add a `udev` rule `51-usbblaster.rules` to `etc/udev/rules.d` with the
  content shown below. Root access may be required.

        # USB-Blaster

        SUBSYSTEM=="usb", ATTR{idVendor}=="09fb", ATTR{idProduct}=="6001", MODE="0666"

- Set the rights to `644`:

        > chmod 644 51-usbblaster.rules

- Reload the udev rules by executing

        > udevadm control --reload-rules

- Unplug and reconnect the USB cable again

# System Integration {#sect_gs_install_software_systemintegration}

Automation Studio and SafeDESIGNER are for Windows operating systems only.

To install B&R Automation Studio and SafeDESIGNER, execute the `Install.exe`
from the DVD or from the extracted download.
Once Automation Studio is installed, SafeDESIGNER can be installed from the
subfolder `/Setups` of the DVD or download.