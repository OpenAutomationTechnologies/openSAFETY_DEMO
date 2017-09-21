Building the FPGA software {#page_build_fpga_sw}
============

This page provides information about building the FPGA software.

[TOC]

Make sure that all required software of \ref sect_gs_requirements_sw (2) is
installed. In brief:

- generally:
 - CMake
 - Quartus Toolchain
 - Altera USB-Blaster set up

# Building the FPGA software {#sect_gs_build_fpga_sw}
After all steps described in chapter \ref page_build_fpga_hw are carried out and
the file `[toplevel].sopcinfo` exists, the software for the POWERLINK processor
and the user application example can be built. In order to do
this the following build steps need to be executed:

1. Open the `Nios II 13.0sp1 Command Shell` or a shell where the path
   to Quartus II toolchain is set.

   There are several ways to open `Nios II 13.0sp1 Command Shell`:

   - from Qsys, select `Tools -> Nios II Command Shell`

   - Linux / Unix like environments: execute the script
     `nios2_command_shell.sh` located in the directory
     `QUARTUS_INSTALL_DIR/nios2eds`

   - Windows: execute the batch file `Nios II Command Shell.bat` located in the
     directory `QUARTUS_INSTALL_DIR/nios2eds` or open the
     `Nios II 13.0sp1 Command Shell` from the Start Menu:
     `Programs -> Altera 13.0.1 -> Nios II EDS -> Nios II 13.0sp1 Command Shell`

2. Inside the shell change to the build directory or create a new directory
   and change there. Consider \ref sect_gs_cmake_buildsw_outofsource
   when building for different targets or changing between native and
   cross-compilation.

   - Execute CMake in the build directory with the path to the source
     directory as a parameter.

     If passing other paths than the toolchain file and the source directory
     to **cmake**, make sure to use absolute paths.
     On Windows, use Windows-style drive letters (e.g
     `C:/openSAFETY_DEMO/blackchannel/fpga/boards/altera/terasic-de2-115/cn-pcp-spi`).

     Starting from the openSAFETY_DEMO root directory, type:

            > cd [YOUR_FPGA_BUILD_DIRECTORY]
            > cmake -G"Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-altera-nios2-gnu.cmake ../

     This will generate Unix style Makefiles for the Altera platform with the
     default configuration. See the section \ref sect_gs_cmake_options and
     \ref sect_gs_build_fpga_sw_options of all available options which can be
     passed to CMake. Simply add `-D[YOUR_OPTION]` to the build command above.

   - Alternatively, execute `cmake-gui` to do the configuration via the
     graphical interface.

3. After CMake has finished generating the Makefiles it is possible to compile
   the software for your project. This can be done with the following command.

       > make all

   It is also possible to use the generated build system in combination with the
   tool `Nios II 13.0sp1 Software Build Tools for Eclipse`. There execute the
   following steps:
   - Select the `build` folder as the workspace
   - Click on `File -> Import` and select `General -> Existing Projects Into
     Workspace`.
   - Select the folder `build` as your root folder.
   - Import all found projects into your workspace.
   - Build the project **pcppsi** by right clicking
     on the project and selecting `Build`.

> **Note:** It is also possible to use the `Make Target` view to build the project
> by enabling it with `Window -> Show View -> Other -> Make Target`. This view
> shows all predefined make targets for each project which provide all necessary
> build steps.

# Target specific configuration option {#sect_gs_build_fpga_sw_options}

See \ref sect_gs_cmake_options for the general configuration options.

Variable Name                    | Description              | Default value
---------------------------------|--------------------------|------------------
CFG_NIOS2_QUARTUS_DIR | path to the reference hardware project | blackchannel/POWERLINK/fpga/boards/altera/terasic-de2-115/cn-pcp-spi
ALT_PCP_INSTANCE_ID | POWERLINK processor instance number | 0

- **CFG_NIOS2_QUARTUS_DIR**

 Specify the path to the reference hardware project (e.g: blackchannel/POWERLINK/fpga/boards/altera/[board_name]/[demo-name]).

- **ALT_PCP_INSTANCE_ID**

  The instance number of the POWERLINK processor.
  (Needs to be set for flash programming!)

# Viewing the debug output {#sect_gs_build_fpga_sw_debug_com}

When the software of the PCP was built in `Debug` configuration, it is possible
to view the status / debug output of the board:

- Connect the FPGA board via USB to the computer
- Open a `Nios II 13.0sp1 Command Shell` or a shell where the path
  to Quartus II toolchain  is set.
- To connect to the PCP and view the output, type

        > nios2-terminal -i [ALT_PCP_INSTANCE_ID]

  where `ALT_PCP_INSTANCE_ID` is the value set with CMake.

  By default it is `0`:

        > nios2-terminal -i 0