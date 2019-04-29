Needed Software {#page_needed_software}
============

[TOC]

This page provides information on the software used in conjunction with the
openSAFETY_DEMO and where to get it. See also \ref page_install_software
for information on additional ways to get the software and it's installation.

> **Note:** The openSAFETY_DEMO has been successfully brought into operation
> with the used hardware and software, related revisions and
> versions stated in this guide.
> Depending on user needs and requirements, other hardware, software or
> related versions may be used, but this is out of the scope of the
> openSAFETY_DEMO and this documentation.

# Software Requirements   {#sect_gs_requirements_sw}

The software requirements are depending on the desired usage of the openSAFETY_DEMO:

## 0. Generating the documentation {#sect_gs_req_sw0}

  For generating the documentation the follwing software is needed:
  - Windows operating systems only: MinGW (+ MSYS)

   development environment for applications, including Windows ports of GCC,
   GNU Binutils, Bourne Shell command line interpreter and some Unix tools.

   https://sourceforge.net/projects/mingw/

  - CMake https://www.cmake.org/
  - Doxygen http://www.stack.nl/~dimitri/doxygen/download.html
  - GraphViz https://www.graphviz.org/Download.php


## 1. Usage of the pre-built binaries only {#sect_gs_req_sw1}

  For flashing the development hardware with the pre-built binaries only,
  the following software is needed:
  - Windows only: ST-Link/V2-1 USB driver for your NUCLEO board
    http://www.st.com/content/st_com/en/products/embedded-software/development-tool-software/stsw-link009.html

  - Altera Quartus Programmer v13.0sp1, for programming the FPGA board
    https://www.altera.com/downloads/download-center.html


## 2. Build the software locally {#sect_gs_req_sw2}

For building the software and the FPGA bitstream locally,
the following software is additionally needed to (1):

### General Software
- CMake 3.0.2

  extensible open-source system which allows managing build processes independent
  from operating systems and compilers

  https://www.cmake.org/

- Windows operating systems only: MinGW (+ MSYS), see also \ref sect_gs_req_sw0

### openSAFETY Demo related software

- openSAFETY distribution 1.5.2

  contains the openSAFETY stack and additional tools needed for the
  openSAFETY_DEMO

- openPOWERLINK stack 2.5.0 or higher

  contains the openPOWERLINK stack for POWERLINK communication.
  Using a stable release is recommended. The git repository version
  of the openSAFETY_DEMO is using openPOWERLINK version 2.6.2.

Download as archives

If the openSAFETY Demo was downloaded as archive, get the packages here:

- openSAFETY distribution 1.5.2 https://sourceforge.net/projects/opensafety

- openPOWERLINK stack 2.5.0 or higher
  http://openpowerlink.sourceforge.net/web/openPOWERLINK/Download.html


Using git

When the openSAFETY Demo repository was cloned without submodules,
the openPOWERLINK stack and openSAFETY distribution
can be obtained by changing into the openSAFETY_Demo directory and executing the
following commands:

        > git submodule init
        > git submodule update


### Nucleo related Tools

- GCC ARM Embedded 4.8-2014-q2-update

  cross compiler for the ARM Cortex-M3 / -M4 microcontrollers used by the
  Nucleo boards.

  https://launchpad.net/gcc-arm-embedded/4.8/4.8-2014-q2-update

- stlink tools

  open source software to program and debug the Nucleo boards

  https://github.com/texane/stlink

- libusb

  needed by the stlink tools
  > **Note:** The st-link tools rely on libusb to access the ST-LINK/V2.
  > Older versions of libusb have trouble accessing ST-LINK/V2 debug adaptors
  > when they are connected to an USB 3.0 port (even with an USB 2.0 hub in
  > between). In this case the error message "Couldn't find any ST-Link/V2
  > devices" will be shown.
  > To solve this issue, a newer version of libusb has to be used.
  > Version 1.0.19 has been tested to be successful.

  http://libusb.info/

- HAL Library for the used NUCLEO boards:

  * NUCLEO-F103RB: STM32CubeF1 V1.7.0: http://www.st.com/en/embedded-software/stm32cubef1.html

  * NUCLEO-F401RE: STM32CubeF4 V1.21.0: http://www.st.com/en/embedded-software/stm32cubef4.html

  * Both versions of Nucleo: STM32CubeMX: http://www.st.com/en/development-tools/stm32cubemx.html
  > This enables the download of different versions of STM32Cube for
  > different microcontrollers. This way the HAL libraries can be downloaded in
  > an other version than the most recent one provided by the links above.

- optional: STM32 ST-LINK Utility

  for programming / erasing the microcontroller on the Nucleo boards and
  updating the debug adapter firmware,
  this program is only available for Windows operating systems.

  http://www.st.com/en/embedded-software/stsw-link004.html

- optional: ST-LINK/V2 firmware upgrade

  for upgrading the debug adapter firmware of the Nucleo boards, this
  software is available for Windows, Linux and MacOS operating systems.

  http://www.st.com/content/st_com/en/products/embedded-software/development-tool-software/stsw-link007.html

### Altera Tools
- Altera Quartus v13.0sp1 with device support for Cyclone IV E devices

  toolchain for FPGA design, software development, device programming

  https://www.altera.com/downloads/download-center.html
  > **Note:** The Quartus Programmer is part of the Quartus Toolchain,
  > it doesn't have to be downloaded separately.
  >
  > **Note:** Both versions of Quartus Software (Web-Edition,
  > Subscription-Edition)
  > are able to generate the FPGA bitstream. With the Web-Edition it is not
  > possible to download a design to the non-volatile memory. This means, that
  > a continuous connection between FPGA board and computer / Quartus is
  > mandatory to use the generated bitstream.
  > In the case the connection gets closed, the PCP part will stop working.


## 3. Modify the openSAFETY_DEMO with user customised OSDD file {#sect_gs_req_sw3}

  For customising the demo in respect to the processing of input and output data
  as well as the used GPIO pins, the following software is additionally needed
  to (2):
  - Python (successfully tested with versions 2.7.9 and 3.4.2)
    https://www.python.org

    The following Python packages need to be installed:
    * PyXB 1.2.4 https://pypi.python.org/pypi/PyXB/1.2.4
    * cogapp 2.4 https://pypi.python.org/pypi/cogapp/2.4

    To execute the unittests of the pysodbuilder with coverage info and
    xml report, the following Python packages are needed:
    * nose 1.3.7 https://pypi.python.org/pypi/nose/1.3.7
    * coverage 3.7.1 https://pypi.python.org/pypi/coverage/3.7.1


## 4. System integration in an B&R openSAFETY system {#sect_gs_req_sw4}

For integrating the openSAFETY_DEMO Safe Node in an B&R openSAFETY system
and putting the whole system into operation the following software is needed.
Please contact your local B&R representation regarding further product
information and purchasing.

- Automation Studio 4.5.3 or higher

  integrated development environment used, among others, for planning,
  implementing and configuring B&R control systems

  https://www.br-automation.com/en/products/software/automation-studio/

- SafeDESIGNER 4.3 or higher

  development environment for developing safe logic / the safe program
  for the SafePLC and configuring the openSAFETY safe nodes

> **Note:** The software is only available for Microsoft Windows
> operating systems.


## 5. Other useful software {#sect_gs_req_sw5}

- git

  version control system, used in this demo for checking out git repositories

  https://git-scm.com/downloads

- Windows only: Tera Term

  terminal programm for viewing output of the development boards on virtual COM
  port

  https://ttssh2.osdn.jp/index.html.en

- Eclipse IDE for C/C++ Developers

  IDE for C/C++ development, used for developing software for the application
  processors

  https://www.eclipse.org/downloads/

- Wireshark

  Network protocol analyzer used for viewing / analyzing
  POWERLINK and openSAFETY frames.

  https://www.wireshark.org/
