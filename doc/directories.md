Directory structure {#page_directories}
===================

[TOC]

This page describes the directory structure of the openSAFETY_DEMO source
directory.
It gives details about the content of each folder and also adds other useful
information.

# Main directories {#sect_directories_main}
The openSAFETY_DEMO utilises a rather flat directory structure.
The sources are located in the root folder of the the
release package. It consists of all the source code needed in order to run
the provided demo application. The following folders are given in the root
directory.

Directory                     | Description
----------------------------- | -----------------------------------------------
app                           | Demo application for the application processor (See \ref sect_directories_examples)
bin                           | Prebuilt binaries for flashing (See \ref sect_directories_bin)
blackchannel                  | Sources related to the used fieldbus (See \ref sect_directories_blackchannel)
cmake                         | Files for the CMake build tool
contrib                       | Additional files needed by a platform or a tool
devicedescription             | Device description files
openSAFETY                    | Directory where the openSAFETY distribution hast to be stored in
user_customisable             | Directory where generated code will be placed for further user customisation


## Safe node application {#sect_directories_examples}
The SN demo application demonstrates the usage of the user library.

Directory                     | Description
----------------------------- | -----------------------------------------------
demo-sn-gpio                  | A lightweight openSAFETY SN demo (See \ref sect_gs_sysdesc_app_sndemo)
target                        | Target platform specific files

### Configuration files {#sect_directories_configuration}

The application contains a directory called **config**. It contains the
object dictionaries and the triple buffer configuration headers.

Directory                     | Description
----------------------------- | -----------------------------------------------
pcp                           | POWERLINK processor configuration files, e.g. Object dictionary
sn                            | application processor configuration files
tbuf                          | Configuration headers for the triple buffers


## Pre-built binaries {#sect_directories_bin}
If the openSAFETY_DEMO was downloaded as archive,
the __bin__ directory contains pre-built binaries for flashing onto the
development boards.

Directory                     | Description
----------------------------- | -----------------------------------------------
fpga/boards/terasic-de2-115   | Pre-built bitstream and application
stm32f103rb                   | Pre-built binaries in single and dual channelled version for STM32F103RB microcontroller
stm32f401re                   | Pre-built binaries in single and dual channelled version for STM32F401RE microcontroller


## Black channel {#sect_directories_blackchannel}
This folder contains a subfolder **POWERLINK** which contains sources
needed for using the openSAFETY_DEMO via POWERLINK.

Directory                     | Description
----------------------------- | -----------------------------------------------
contrib                       | Additional files needed by a platform or a tool
fpga                          | FPGA specific stuff like IP cores and VHDL code
libs                          | POWERLINK interface libraries (See also \ref sect_directories_libraries)
misc                          | Miscellaneous tools, scripts and utilities
pcp                           | POWERLINK interface PCP drivers (Driver of the POWERLINK stack)
stacks                        | The protocol stacks used in this project (See \ref sect_directories_stack)
unittests                     | CUnit Unit tests and framework



### Libraries {#sect_directories_libraries}
The __libs__ directory contains libraries used by the POWERLINK interface examples.

Directory                     | Description
----------------------------- | -----------------------------------------------
psi                           | Slim interface user library
psicommon                     | Slim interface common library (Used by POWERLINK and the user application)


### Stack sources {#sect_directories_stack}

The __stacks__ directory contains the source code of the stacks which are included in the
POWERLINK interface.

Directory                     | Description
----------------------------- | -----------------------------------------------
openPOWERLINK                 | The openPOWERLINK stack sources needed by the POWERLINK processor
ip                            | A lightweight ip stack needed by the POWERLINK processor
