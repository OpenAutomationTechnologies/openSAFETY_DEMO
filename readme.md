Introduction {#mainpage}
============

[TOC]

# Introduction {#sect_main_intro}

The openSAFETY_DEMO is an example implementation of an openSAFETY Safe Node (SN)
with a simple GPIO application using POWERLINK as the underlying
communication network. It is intended for people interested in openSAFETY and
for evaluation purposes of openSAFETY.

\warning This demo only demonstrates the network capabilities and transport of
openSAFEY frames. Additional software and hardware components are necessary to
meet any necessary requirements demanded by notified bodies or related
standards.

The demo application consists of one part for handling the
POWERLINK communication (POWERLINK Communication Processor, PCP) which is
implemented on a FPGA platform and a second part (application processor)
executing the
the user application, such as the openSAFETY stack and the safe application.

The user application part can be used in a single and a dual channelled
configuration. The single channelled configuration uses one application
processor, the dual channelled configuration additionally uses a second
redundant application processor.

The partitioning of the system into application and network part allows a good
separation of safety related and non-safety related code as well as the exchange
of the underlying network interface.


# openSAFETY_DEMO {#sect_main_sysdesc_oS_demo}

This section provides an overview of the openSAFETY_DEMO system components
and how they interact with each other. The basic concept of the system is the
clear separation of the network protocol and the user application. This is
achieved by using a standalone processor for POWERLINK, called the
`POWERLINK Communication Processor (PCP)`, which is implemented on a **Terasic
DE2-115** FPGA board.
The PCP handles the complete processing of the network protocol and provides a
memory interface to forward the process and service data to the user
application.

The second system part executes the user
application, i.e. the openSAFETY stack and the safety application. It is
implemented in two ways, as single channelled and as dual channelled system,
using STMicroelectronics Nucleo boards: **NUCLEO-F401RE** or **NUCLEO-F103RB**.

The single channelled implementation comprises just one application processor,
the dual channelled implementation additionally uses a second redundant
application processor, which shares the SPI serial device with the other
processor. The two processors  receive the same data, exchange data among them
and verify each others produced output.

System setup details of the dual channelled implementation are
depicted in the next figure. For a single channelled application, the lower
processor board on the right would not be part of the system.

![Component overview: Control system consisting of the openSAFETY_DEMO Hardware (dual channelled) and B&R Hardware](system_overview.png)
@image latex system_overview.eps "Component overview of the openSAFETY_DEMO"

See \subpage page_gs_demo_detail for further information.


# Protocol Stacks

This software distribution uses the POWERLINK and openSAFETY protocols as a
basis for communication. More information about this protocols is provided in
\ref page_protocols.

For processing the POWERLINK protocol on the PCP the open source protocol stack
`openPOWERLINK V2.x` is used. This stack is already integrated in this
distribution. For more information about openPOWERLINK checkout the doxygen
documentation of the stack in the folder
`blackchannel/POWERLINK/stacks/openPOWERLINK/doc`.

The openSAFETY protocol stack is processed on the application processor.
This open source stack can be downloaded from
http://sourceforge.net/projects/opensafety.



# License   {#sect_main_license}

The openSAFETY_DEMO is open-source software (OSS) and is licensed under the
BSD license.
Please refer to the file's header and the file
[\"license.md\"](\ref page_licenses) for the applicable license and the
corresponding terms and conditions.

# Documentation   {#sect_main_documentation}

## Scope of the openSAFETY_DEMO Documentation {#sect_main_documentation_scope}

This documentation provides demo-specific information on the used hard-
and software, how to put the demo into operation and the demo's
customisation possibilities.
With SN development in mind, this documentation also gives generalised
information on the hardware design, software design and the software API of the
interface between the POWERLINK Communication Processor (PCP) and the user
application.


## Location and Generation {#sect_main_documentation_location_generation}

* The documentation of the openSAFETY_DEMO is located in the
  subdirectory "doc". It is written in _markdown_ format and can be read
  by opening the `.md` with a text editor.
* The software manual can be generated from the markdown
  documentation and the in source-code documentation with the tools
  [Doxygen](http://www.doxygen.org) and [CMake](http://www.cmake.org). Therefore
  Doxygen version greater or equal 1.8 is required. The software manual will be
  created in HTML format under `build/doc/html`.
  [Graphviz] (http://www.graphviz.org/) is used for generating graph
  visualisations and diagrams.

  To generate the documentation enter the following commands according to your
  host operating system.

### Build command on Windows {#sect_main_build_doc_windows}

One possibility for building the documentation is to use
[MinGW and MSYS](http://sourceforge.net/projects/mingw).
When installed, open a MSYS terminal and carry on with the steps
mentioned in \ref sect_main_build_doc_linux.

Another possibility is to use the tools of a Visual Studio installation.
Open the `Visual Studio Command Prompt` and enter the following commands:

      > cd build
      > cmake -G"NMake Makefiles" ../
      > nmake doc


### Build command on Linux {#sect_main_build_doc_linux}
Open a terminal where the path to `make`, `doxygen` and `CMake` is set and enter
the following commands:

      > cd build
      > cmake -G"Unix Makefiles" ../
      > make doc

# Support   {#sect_main_support}

Support on openSAFETY Demo is available via the online discussion forums on the
SourceForge project page:

 - [Discussion] (https://sourceforge.net/p/opensafetydemo/discussion/)

# Download   {#sect_main_download}

The openSAFETY_DEMO can be obtained from its SourceForge project site,
GitHub and via git:

 - SourceForge
   https://sourceforge.net/projects/opensafetydemo/

 - GitHub
   https://github.com/OpenAutomationTechnologies/openSAFETY_DEMO

 - git

   For cloning the openSAFETY_DEMO repository and all necessary submodules
   execute the following statements:

        > git clone --recursive https://github.com/OpenAutomationTechnologies/openSAFETY_DEMO.git openSAFETY_DEMO
        > git checkout -b V1.5.0
