CMake {#page_cmake}
============

The openSAFETY_DEMO makes extensive use of the cross-platform, open-source
build system CMake.
This page provides information about the program and it's usage.


[TOC]

# Out-of-Source Builds {#sect_gs_cmake_buildsw_outofsource}

CMake support out-of-source builds. Therefore, all generated files are located
in a separate build directory which keeps your sources clean. The
openSAFETY_DEMO project already provides the folder `build` in the root
directory.

> **IMPORTANT:** When building for different architectures or platforms,
> **separate build directories** have to be created
> (e.g. /build_fpga, /build_mc, /build_doc, ..) or the build directory has to
> be emptied.
> This also applies when changing toolchain files or changing from native
> to cross compilation.
> See \ref sect_cmake_commandline for an example.

# CMake usage {#sect_cmake}

There are different ways to generate build configurations with CMake.

## Command line {#sect_cmake_commandline}

CMake can be used directly from the command line. It is possible to select
the generator for different build systems and pass the CMake variable values.
Pass `-G "GENERATOR"` to CMake, where `GENERATOR` is one of the supported
generators of CMake.

- To list available command line options and generators, open a shell where
  the path to CMake is set and type

        > cmake --help

- To specify a value for a CMake variable pass `-D[YOUR_OPTION]=VALUE` on the
  command line. Refer to the sections **Target specific configuration option**
  throughout the documentation to view available options to pass to CMake.

- Inside the shell enter the build directory or create a new one
  and execute CMake in this directory with the path to the
  source directory as a parameter.

  To create a new directory, change to the openSAFETY_DEMO root directory and
  type the following command. [YOUR_NUCLEO_BUILD_DIRECTORY] is a freely chosen
  name, e.g. `build_nucleo`.

        > mkdir [YOUR_NUCLEO_BUILD_DIRECTORY]

  Example:

        > cd [YOUR_NUCLEO_BUILD_DIRECTORY]
        > cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-arm-cortexm4-gnueabi.cmake \
        >       -DCFG_STM32_HAL_LIB_LOC=/c/dev/STM32Cube_FW_F4_V1.21.0/ -DCFG_PROG_FLASH_ENABLE=ON \
        >       -DCFG_DUAL_CHANNEL=dual-channel -DCMAKE_BUILD_TYPE=Debug ../

  The example above will generate Unix style Makefiles for the stm32f401 platform
  in a dual channelled configuration with the selected HAL library
  and enabled software download.

- After successful generation, change back to the shell. To list the
  available targets type

        > make help

- To build the desired target type

        > make TARGETNAME

  or type

        > make all

  for building all targets.

- To build the software for an additional target (change of toolchain-file or
  changing between native and cross-compiilation), delete the content
  of the existing build directory and execute cmake or create a new directory,
  change there and execute cmake.

  Example using the additional build directory `build_fpga`,
  starting in the openSAFETY_DEMO root directory:

        > mkdir build_fpga
        > cd build_fpga
        > cmake -G"Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-altera-nios2-gnu.cmake ../
        > make all

## cmake-gui {#sect_cmake_gui}

CMake also provides a graphical user interface (GUI) for selecting the
desired generator, toolchain file and settings:

- Open a shell where the path to CMake and the tools you want to use is set.
- Open the CMake GUI by typing

        > cmake-gui &

- Select the directory in which the source files are located.
  Also, select a build directory (must not be the source directory!)

  ![CMake GUI](cmake.png)
  @image latex cmake.png "CMake GUI"

- Click the button `Generate`
- Specify the generator and if you are building for the local machine
  (`Use default native compilers`) or cross compiling
  (`Specify toolchain file for cross-compiling`).

  ![CMake Generator and Compilation Selection](cmake_generator.png)
  @image latex cmake_generator.png "CMake Generator and Compilation Selection"

- When cross-compiling, select a toolchain-file from the demo's **cmake**
  directory.
- After first configuration, edit the settings shown by CMake GUI

  ![Newly added CMake Variables](cmake_settings.png)
  @image latex cmake_settings.png "Newly added CMake Variables"

- Click `Configure again`, new variables  may show up in red. If applicable,
  set the new variables to the desired value.
- Click `Generate`

  ![CMake Generation done](cmake_generation_done.png)
  @image latex cmake_generation_done.png "CMake Generation done"

- After successful generation, change back to the shell. To list the
  available targets type

        > make help

- To build the desired target type

        > make TARGETNAME

 or type

        > make all

 for building all targets.


# Target Platforms and Build Configuration  {#sect_gs_cmake_buildsw}

The following target platforms are supported, where not
all platforms always provide a full set of features.
Follow the referenced targets for information on hardware and software setup
and the procedure for building the bitstream and software respectively.

Target platforms used for executing parts of the openSAFETY_DEMO:
- \subpage page_gs_altnios2
- \subpage page_gs_stm32f103rb
- \subpage page_gs_stm32f401re


The following platform is used only for compiling and executing the PSI
unit tests:
- \subpage page_gs_x86


## Configuration Options {#sect_gs_cmake_options}

The following build configuration options are available to pass to **CMake** by
using the `-D` parameter or by enabling them in the graphical user interface:

> **Note:** When using `cmake-gui` check the boxes `Grouped` and `Advanced`
> to make visible all available options grouped by their names.

Variable name                 | Description                               | Default value
----------------------------- | ------------------------------------------|--------------
CFG_DEMO_TYPE       | Change the type of the demo to build | sn-gpio
CMAKE_BUILD_TYPE    | Specifies the build type of the openSAFETY_DEMO project | Release
CFG_APP_DEBUG_LVL   | Change the debug level of your selected application   | 0xEC000000L
CFG_PSI_DEBUG_LVL   | Change the debug level of the PSI library     | 0xEC000000L
CFG_PSICOMMON_DEBUG_LVL | Change the debug level of the PSI common library | 0xEC000000L
PYSODBUILDER_ENABLE | Enable the generation of customised code based on the given OSDD file | False
USE_CUSTOMISED_FILES | Enable or disable the usage of customised code in /user_customisable | True

- **CFG_DEMO_TYPE**

  Change the type of the demo to build. (e.g: `sn-gpio` or `custom`)

- **CMAKE_BUILD_TYPE**

  This parameter specifies the build type of the openSAFETY_DEMO project.
  The following options
  are available:
    * `Release:` The project is built with highest optimization and no debug
      information.
    * `Debug:` The project is built with no optimization and debug information.
    * `RelWithDebInfo:` The project is built with highest optimization and
      debug information.
    * `MinSizeRel:` The project is built with size optimization and no debug
      information.

- **CFG_APP_DEBUG_LVL**

  Change the debug level of your selected application (Depends on the setting
  of `CFG_DEMO_TYPE`).

- **CFG_PSI_DEBUG_LVL**

  Change the debug level of the PSI library. (See \ref group_libpsi)

- **CFG_PSICOMMON_DEBUG_LVL**

  Change the debug level of the PSI common library. (See \ref group_libpsicommon)

- **PYSODBUILDER_ENABLE**

  Enables or disables the generation of customised code based on the given OSDD
  file.
  Refer to \ref page_user_customisations for further information.

- **USE_CUSTOMISED_FILES**

  Enables or disables the usage of customised code in /user_customisable.
  This option depends on the selected target system and `PYSODBUILDER_ENABLE`.
  Refer to \ref page_user_customisations for further information.

### SN configuration options {#sect_buildsw_options_sn}
All options from this sections are valid when `CFG_DEMO_TYPE=sn-gpio`. This means
that the application build system is set to host a safe node (SN) demo.

Variable name                 | Description                               | Default value
----------------------------- | ------------------------------------------|--------------
CFG_DUAL_CHANNEL    | Specifies if the demo has one or two SN channels | single-channel
CFG_SAPL_SN_UDID    | Specifies the UDID of your SN | {0x00U,0x60U,0x65U,0x01U,0x14U,0x9BU}
CFG_SAPL_SN_VENDORID| Specifies the vendor ID of your SN    | 0x00005678UL
CFG_SAPL_SN_PRODUCT_CODE | Specifies the product code of your SN | 0x00000001UL
CFG_SAPL_SN_REVISION_NR | Specifies the revision number of your SN  | 0x00010100UL
CFG_SAPL_SN_SERIAL_NR   | Specifies the serial number of your SN    | 0x5A5A5A5AUL
CFG_SAPL_SN_FW_CHKSUM   | Specifies the firmware checksum of your SN    | 0x5A5A5A5AUL

- **CFG_DUAL_CHANNEL**

  This parameter specifies if the demo has one or two SN channels. A second
  channel means that the application is hosted on two redundant processors.

- **CFG_SAPL_SN_UDID**

  This parameter specifies the UDID of your SN. This ID needs to be set as a
  unique number for each SN in the network.

- **CFG_SAPL_SN_VENDORID**

  This parameter specifies the vendor ID of your SN.
  (Object 0x1018/0x1 in the SOD)

- **CFG_SAPL_SN_PRODUCT_CODE**

  This parameter specifies the product code of your SN.
  (Object 0x1018/0x2 in the SOD)

- **CFG_SAPL_SN_REVISION_NR**

  This parameter specifies the revision number of your SN.
  (Object 0x1018/0x3 in the SOD)

- **CFG_SAPL_SN_SERIAL_NR**

  This parameter specifies the serial number of your SN.
  (Object 0x1018/0x4 in the SOD)

- **CFG_SAPL_SN_FW_CHKSUM**

  This parameter specifies the firmware checksum of your SN.
  (Object 0x1018/0x5 in the SOD)
