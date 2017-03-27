Customising the openSAFETY_DEMO {#page_user_customisations}
============

[TOC]


This page provides information about the customisation possibilities of the
openSAFETY_DEMO without having to alter the original source code.
The following application related parts can be modified:

- used GPIO pins (app-gpio.c)
- processing of cyclic input / output data and processing of the GPIO pin data
  (app.c)
- openSAFETY stack configuration header file (EPLScfg.h)

- customisations done with a customised OSDD file:
  - Safe Object Dicitonary (SOD) (sod.c, sod.h)
  - SOD utility file (sodutil.h)

Prior to further details on customisation, a short introduction to the
openSAFETY Device Description file format (OSDD) and the workflow of generating
source code related to an OSDD file is given.

# OSDD - one file format for everything {#sect_osdd}

The openSAFETY Device Description file format is a standardised file format and
is used to describe an openSAFETY device, respectively it's properties.
It contains definitions, channels and settings of the device.
Please refer to the openSAFETY Device Description File Format Definition for
detailed information on on the OSDD file format.

An OSDD file can be used for system integration as well as for firmware
development and therefore ensures consistency throughout the development
process.
This consistency can be achieved by using the OSDD file for generating
customised source files. As part of the openSAFETY distribution, the
**pysodbuilder** tool enables the generation of source files based on a given
OSDD file.
The integration of the pysodbuilder tool into the workflow is  shown
schematically in section \ref sect_pysodbuilder.

# pysodbuilder {#sect_pysodbuilder}

When enabling the pysodbuilder tool with the cmake variable
**PYSODBUILDER_ENABLE**, the tool uses input template files, the specified
OSDD file, settings file and schema files to generate source files which
are related to the OSDD file and the settings made in cmake.
Section \ref sect_pysodbuilder_cmakeconfig describes the available
settings.

![pysodbuilder - principle](pysodbuilder_flow_small.png)
@image latex pysodbuilder_flow.png "pysodbuilder_flow" width=0.75\textwidth


When building the application with the tool enabled, the generated files
will be compiled into the binary instead of the original ones.
By changing the cmake settings and the used OSDD file for code generation,
customising the application takes relatively little effort and consistency of
related code with the OSDD file is ensured.

# Using the pysodbuilder tool {#sect_pysodbuilder_user_guide}

## Requirements {#sect_pysodbuilder_requirements}

This tool is written in <a href="https://www.python.org/">Python</a>
and makes use of two third party packages.
For proper execution of the tool the required Python interpreter and packages
have to be installed:

 - Python interpreter version 2.7.9 or 3.4.2 https://www.python.org/ , the tool
  was developed and tested with both versions
 - PyXB 1.2.4 https://pypi.python.org/pypi/PyXB/1.2.4
 - cogapp 2.4 https://pypi.python.org/pypi/cogapp/2.4


## CMake Configuration Options {#sect_pysodbuilder_cmakeconfig}

The following build configuration options are available to pass to **CMake** by
using the `-D` parameter or by enabling them in the graphical user interface.

The configuration options other than those in
\ref sect_pysodbuilder_cmakeconfig_general  are used to parameterize the
openSAFETY stack.
The generation of the related EPLScfg.h file uses specified cmake settings
as well as information obtained from the specified OSDD file.
By default, settings made with cmake are transferred to the file, if
they overrule (are bigger than) the information obtained from the OSDD file,
otherwise the information from the OSDD file will be taken.
This behaviour applies to numerical values, but not to boolean values.

### General Configuration Options {#sect_pysodbuilder_cmakeconfig_general}

Variable name           | Description                           | Default value
----------------------- | --------------------------------------|--------------
PYSODBUILDER_ENABLE     | Enable or disable usage of the tool   | OFF
PYSODBUILDER_INPUT_DIR  | Directory of input template files     | [ProjectDir]/app/demo-sn-gpio/pysodb_templates
PYSODBUILDER_MODULE_ID  | Module id to generate files for, if the OSDD file contains several modules | -
PYSODBUILDER_OUTPUT_DIR | Directory of the output files         | [ProjectBuildDir]/pysodbuilder_generated
PYSODBUILDER_OSDD_FILE  | OSDD file to use for pysodbuilder     | [ProjectDir]/devicedescription/openSAFETY_DEMO.xosdd
PYSODBUILDER_OSDD_SCHEMA_FILE | OSDD schema file to which the used OSDD file adheres | [ProjectDir]/openSAFETY/doc/XOSDD_R15.xsd
PYSODBUILDER_SETTINGS_FILE | Settings file to use for pysodbuilder, which is created by **CMake** | [PysodbuilderBinaryDir]/pysodbsettings.ini
USE_CUSTOMISED_FILES    | Enable or disable the usage of customised files instead of original or generated ones | ON

- **USE_CUSTOMISED_FILES**

  This option depends on PYSODBUILDER_ENABLE and the selected target, where
  it is currently available for the STM32 target only.

  When enabled, the files app.c (generated from pysodbuilder) and app-gpio.c
  will be copied to the directory **user_customisable**/[SelectedTarget],
  if they do not exist.
  If the files are already existing, they will not be overwritten.

  When building the application and this option is enabled, the files
  in the directory **user_customisable**/[SelectedTarget] will be compiled
  into the application.

  If the option is disabled, the original app-gpio.c file and the generated
  app.c file will be used for compilation.

### eplsCfg settings {#sect_pysodbuilder_cmakeconfig_eplscfg}


Variable name                 | Description                               | Default value
----------------------------- | ------------------------------------------|--------------
PYSODBUILDER_SAPL_REPORT_STATE_CHANGE | Enable or disable status change callback functions | OFF
PYSODBUILDER_MAXINSTANCES     | Maximum number of instances               | 1
PYSODBUILDER_MAXPAYLOADLENGTH | Maximum payload data length of a SSDO     | 8
PYSODBUILDER_ERRORSTRING      | Enables or disables the  XXX_GetErrorStr function (e.g. SOD_GetErrorStr) | OFF
PYSODBUILDER_ERRORSTATISTIC   | Enable or disable the extended telegram error statistic | OFF
PYSODBUILDER_APPLICATIONOBJECTS | Enable or disable the objects managed by the application | OFF


- **PYSODBUILDER_MAXINSTANCES**

  Configures the maximum number of instances.
  Allowed values : 1..255

  See also EPLS_cfg_MAX_INSTANCES in the openSAFETY stack documentation.

- **PYSODBUILDER_MAXPAYLOADLENGTH**

  Configures the maximum payload data length
  of a received SSDO Service Request or a transmitted SSDO Service Response on
  a SSDO server.

  **NOTE:**
  On a SSDO client the maximum payload data length of a SSDO server is
  specified in the SOD (Index 0xC400 sub-index 8 MaximumSsdoPayloadLen) and
  must be passed to the SSDO client by calling the SSDOC_SendWriteReq() and
  SSDOC_SendReadReq().

  Allowed values: 8..254

  **ATTENTION:** standard data type INT64, UINT64, REAL64
  is only available for EPLS_cfg_MAX_PYLD_LEN >= 12

  See also EPLS_cfg_MAX_PYLD_LEN in the openSAFETY stack documentation.

- **PYSODBUILDER_ERRORSTRING**

  Enable or disable the XXX_GetErrorStr function
  (e.g. SOD_GetErrorStr). These functions are used to print an error string
  for the error code.

  Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE

  See also EPLS_cfg_ERROR_STRING in the openSAFETY stack documentation.

- **PYSODBUILDER_ERRORSTATISTIC**

  Enable or disable the extended telegram error statistic.

  Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE

  See also EPLS_cfg_ERROR_STATISTIC in the openSAFETY stack documentation.

- **PYSODBUILDER_APPLICATIONOBJECTS**

  Enable or disable the objects managed by the application

  Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE

  See also SOD_cfg_APPLICATION_OBJ in the openSAFETY stack documentation.

### constants settings {#sect_pysodbuilder_cmakeconfig_constants}

Variable name                 | Description                               | Default value
----------------------------- | ------------------------------------------|--------------
PYSODBUILDER_CONFIGSTRING     | Represents the EPLsafety Stack configuration as a string | demo

- **PYSODBUILDER_CONFIGSTRING**

  Represents the EPLsafety Stack configuration as a string

  See also EPLS_k_CONFIGURATION_STRING in the openSAFETY stack documentation.

### spdocfg settings {#sect_pysodbuilder_cmakeconfig_spdocfg}

Variable name                 | Description                               | Default value
----------------------------- | ------------------------------------------|--------------
PYSODBUILDER_PROP_DELAY_STATISTIC | Enable variable which delivers an average over the last cycle and actual cycle propagation delay | OFF
PYSODBUILDER_40BIT_CT_SUPPORT | Enable the support for SPDOs with a 40-bit CT value | ON
PYSODBUILDER_EXTENDED_CT_BITFIELD | Extended CT bit field is to be created | OFF
PYSODBUILDER_FRAMECOPYINTERN     | Enable or disable the copy of the received SPDO frames within the SPDO_ProcessRxSpdo() | OFF
PYSODBUILDER_NUMLOOKUPTABLE |  Enable or disable the usage of the Lookup table for the SPDO number assignment | OFF
PYSODBUILDER_CONNECTIONVALIDBITFIELD | Enable or disable creation of Valid bit field | ON
PYSODBUILDER_CONNECTIONVALIDSTATISTIC | Enable or disable creation of Valid statistic counter field created | OFF
PYSODBUILDER_MAXRXSYNCEDPERTX | Maximum number of RxSPDOs to be synchronized over a TxSPDO | 1
PYSODBUILDER_MAXRXSYNCEDPERTX | Number of not answered TR (internal Time Request counter) | 100


- **PYSODBUILDER_FRAMECOPYINTERN**

  Enable or disable the copy of the received SPDO frames within the
  SPDO_ProcessRxSpdo(). If this define is enabled then the buffer of the
  received SPDO frame will be copied into a internal SPDO buffer. The buffer
  of the received SPDO frame is unchanged and the SCM UDID decoding is
  accomplished in the internal SPDO buffer. Otherwise the SCM UDID decoding is
  accomplished in the buffer of the received SPDO frame, the buffer data is
  changed.

  Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE

  See also SPDO_cfg_FRAME_CPY_INTERN in the openSAFETY stack documentation.

- **PYSODBUILDER_NUMLOOKUPTABLE**

  Define to enable or disable the using of the Lookup table for the SPDO
  number assignment. If the loop-up table is enabled then the SPDO filtering
  works faster but it needs always 2 Byte * 1024 memory. If the loop-up table
  is disabled then a linear search table is used to filter the SPDO frames and
  the filtering works slower because of linear searching. This linear search
  table needs 4 Bytes per SADR configured in the SPDO communication parameters.

  Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE

  See also SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE in the openSAFETY stack documentation.

- **PYSODBUILDER_CONNECTIONVALIDBITFIELD**

  Connection Valid bit field is to be created

  Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE

  See also SPDO_cfg_CONNECTION_VALID_BIT_FIELD in the openSAFETY stack documentation.

- **PYSODBUILDER_CONNECTIONVALIDSTATISTIC**

  Connection Valid statistic counter field is to be created

  Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE

  See also SPDO_cfg_CONNECTION_VALID_STATISTIC in the openSAFETY stack documentation.

- **PYSODBUILDER_MAXRXSYNCEDPERTX**

  Maximum number of RxSPDOs to be synchronized over a TxSPDO

  Allowed values: 1..1023

  See also SPDO_cfg_MAX_SYNC_RX_SPDO in the openSAFETY stack documentation.

- **PYSODBUILDER_NOTANSWEREDTR**

  Configures the number of not answered TR (internal Time Request
  counter). If this number is reached then a time synchronization error will
  happen. (See Time Synchronization Consumer in the EPLS specification)

  Allowed values:
   - if SPDO_cfg_MAX_NO_RX_SPDO != 0 then 1..65535
   - if SPDO_cfg_MAX_NO_RX_SPDO == 0 then EPLS_k_NOT_APPLICABLE

  See also SPDO_cfg_NO_NOT_ANSWERED_TR in the openSAFETY stack documentation.


- **PYSODBUILDER_PROP_DELAY_STATISTIC**
  Define to enable variable which delivers an average over the last cycle
  and actual cycle propagation delay.

  See also SPDO_cfg_PROP_DELAY_STATISTIC in the openSAFETY stack documentation.

- **PYSODBUILDER_40BIT_CT_SUPPORT**
  Define to enable the support for SPDOs with a 40-bit CT value.

  Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE
  See also SPDO_cfg_40_BIT_CT_SUPPORT in the openSAFETY stack documentation.

- **PYSODBUILDER_EXTENDED_CT_BITFIELD**
  Extended CT bit field is to be created

   Allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE
  See also SPDO_cfg_EXTENDED_CT_BIT_FIELD in the openSAFETY stack documentation.

### txspdocom settings {#sect_pysodbuilder_cmakeconfig_txspdocom}

Variable name                 | Description                               | Default value
----------------------------- | ------------------------------------------|--------------
PYSODBUILDER_TX_MAXMAPENTRIES | Maximum number of the Tx SPDO mapping entries | 4
PYSODBUILDER_TX_MAXPAYLOADSIZE | Maximum payload size of the Tx SPDOs in bytes | 4
PYSODBUILDER_TX_SPDOSACTIVATEDPERCALL | Number of Tx SPDOs activated per SSC_ProcessSNMTSSDOFrame() call | 1
PYSODBUILDER_TX_MAXSPDO | Maximum number of the Tx SPDOs | 1
PYSODBUILDER_TX_MAXSPDOSDG | Maximum number of the Tx SPDOs of a SDG instance | 0


- **PYSODBUILDER_TX_MAXMAPENTRIES**

  Maximum number of the Tx SPDO mapping entries

  Allowed values: 1..253

  See also SPDO_cfg_MAX_TX_SPDO_MAPP_ENTRIES in the openSAFETY stack documentation.

- **PYSODBUILDER_TX_MAXPAYLOADSIZE**

  Maximum payload size of the Tx SPDOs in byte

  Allowed values: 1..254

  0 is not allowed because this define is used to declare an array and arrays
  can not be declare with 0 element. To configure a Tx SPDO with payload length
  0, the number of entries configured in the Tx SPDO mapping parameter has to
  be set to 0.

  See also SPDO_cfg_MAX_LEN_OF_TX_SPDO in the openSAFETY stack documentation.

- **PYSODBUILDER_TX_SPDOSACTIVATEDPERCALL**

  Tx SPDOs are activated at the transition from Pre-operational to
  Operational state. To execute this transition, SNMTS_SN_set_to_op command is
  sent. This define declares how many Tx SPDOs are activated per
  SSC_ProcessSNMTSSDOFrame() call during one SNMTS_SN_set_to_op command
  processing. Decrementing this define reduces the execution time of
  SSC_ProcessSNMTSSDOFrame() but increases the number of
  SSC_ProcessSNMTSSDOFrame() calls.

  Allowed values: 1..SPDO_cfg_MAX_NO_TX_SPDO

  See also SPDO_cfg_NO_TX_SPDO_ACT_PER_CALL in the openSAFETY stack documentation.

- **PYSODBUILDER_TX_MAXSPDO**

  Maximum number of the Tx SPDOs.

  Allowed values: 1..1023

  See also SPDO_cfg_MAX_NO_TX_SPDO in the openSAFETY stack documentation.

- **PYSODBUILDER_TX_MAXSPDOSDG**

  Maximum number of the Tx SPDOs of a SDG instance.

  Allowed values: 0 (for IO's)

  See also SPDO_cfg_MAX_NO_TX_SPDO_SDG in the openSAFETY stack documentation.

### rxspdocom settings {#sect_pysodbuilder_cmakeconfig_rxspdocom}

Variable name                 | Description                               | Default value
----------------------------- | ------------------------------------------|--------------
PYSODBUILDER_RX_MAXMAPENTRIES | Maximum number of the Rx SPDO mapping entries | 4
PYSODBUILDER_RX_MAXPAYLOADSIZE | Maximum payload size of the Rx SPDOs in bytes | 4
PYSODBUILDER_RX_SPDOSACTIVATEDPERCALL | Rx SPDOs are activated per SSC_ProcessSNMTSSDOFrame() call during one SNMTS_SN_set_to_op command processing | 1
PYSODBUILDER_RX_MAXSPDO       | Maximum number of the Rx SPDOs            | 1
PYSODBUILDER_RX_MAXSPDOSDG    | Maximum number of the Rx SPDOs of a SDG instance | 0

- **PYSODBUILDER_RX_MAXMAPENTRIES**

  Maximum number of the Rx SPDO mapping entries

  Allowed values:
   - if SPDO_cfg_MAX_NO_RX_SPDO != 0 then 1..253
   - if SPDO_cfg_MAX_NO_RX_SPDO == 0 then EPLS_k_NOT_APPLICABLE

  See also SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES in the openSAFETY stack documentation.

- **PYSODBUILDER_RX_MAXPAYLOADSIZE**

  Maximum payload size of the Rx SPDOs in byte

  Allowed values:
   - if SPDO_cfg_MAX_NO_RX_SPDO != 0 then 1..254
   - if SPDO_cfg_MAX_NO_RX_SPDO == 0 then EPLS_k_NOT_APPLICABLE

  See also SPDO_cfg_MAX_LEN_OF_RX_SPDO in the openSAFETY stack documentation.

- **PYSODBUILDER_RX_SPDOSACTIVATEDPERCALL**

  Rx SPDOs are activated at the transition from Pre-operational to
  Operational state. To execute this transition, SNMTS_SN_set_to_op command is
  sent. This define declares how many Rx SPDOs are activated per
  SSC_ProcessSNMTSSDOFrame() call during one SNMTS_SN_set_to_op command
  processing. Decrementing this define reduces the execution time of
  SSC_ProcessSNMTSSDOFrame() but increases the number of
  SSC_ProcessSNMTSSDOFrame() calls.

  Allowed values:
   - if SPDO_cfg_MAX_NO_RX_SPDO != 0 then 1..SPDO_cfg_MAX_NO_RX_SPDO
   - if SPDO_cfg_MAX_NO_RX_SPDO == 0 then EPLS_k_NOT_APPLICABLE

  See also SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL in the openSAFETY stack documentation.

- **PYSODBUILDER_RX_MAXSPDO**

  Maximum number of the Rx SPDOs.
  If the value of this define is 0 then the code size is reduced and the data
  in the received SPDOs and Time Response SPDOs will not be processed. Only the
  Time Request SPDOs will be processed.

  Allowed values: 0..1023

  See also SPDO_cfg_MAX_NO_RX_SPDO in the openSAFETY stack documentation.

- **PYSODBUILDER_RX_MAXSPDOSDG**

  Maximum number of the Rx SPDOs of a SDG instance.

  Allowed values: 0 (for IO's)

  See also SPDO_cfg_MAX_NO_RX_SPDO_SDG in the openSAFETY stack documentation.



## Execution of the pysodbuilder tool {#sect_pysodbuilder_execution}

When the **CMake** variable PYSODBUILDER_ENABLE is `ON`, the tool will be
invoked automatically when building the targets for the application processor
software.

In addition, the tool can be invoked manually via executing a specific target.
When a Makefile was generated with **CMake**, the execution of `make help`
lists several pysodbuilder-targets.

`make pysodbuilder` will execute the tool and create the customised .c and
header files. If necessary, it will automatically generate an appropriate
binding file before the generation of the sources.

### Execution of the pysodbuilder tool via commandline {#sect_pysodbuilder_cli}
It is also possible to run the pysodbuilder tool from commandline with
the following possible command line switches:

  - -h, \--help            show this help message and exit

  - -d FILE, \--osdd-file FILE
                        osdd file to obtain values from

  - -m MODULE_ID, \--module-id MODULE_ID
                        generate files for given module id. this option is
                        necessary, when using osdd files which contain several
                        modules

  - -s FILE, \--settings-file FILE
                        uses the given settingsfile.

  - -i [FILE | DIRECTORY [FILE | DIRECTORY ...]], \--input [FILE | DIRECTORY [FILE | DIRECTORY ...]]
                        input files or directories containing the files used
                        for code generation, basically .h and .c files.

  - -o DIRECTORY, \--output DIRECTORY
                        output directory

  - \--overwrite-input-files
                        overwrite input files with generated files.

  - -n ENCODING, \--encoding ENCODING
                        specyfiy file encoding

  - -l, \--list-modules    list modules contained in specified osdd file

  - \--generate-binding-file FILE FILE
                        use schema file (.xsd) to generate a bindings file
                        (.py).

  - \--no-overrule-osdd    settings obtained from osdd file do not get overruled,
                        if the corresponding values in the settings file are
                        bigger.

  - \--remove-cog-comments
                        remove comments used for code generation out of the
                        generated files. this option requires -o DIRECTORY.
                        Also, the output directory must not be part of the
                        input directories to prevent unwanted overwriting of
                        files.

  - \--dry-run             prints the input files with the generated code, but
                        doesn't save the result

  - -v, \--verbose         increase verbosity of the program

  - \--log FILE            write log to file

  - \--version             print version of the program

# Customising the used I/O pins of the application processor boards {#sect_customise_gpio}

For customising the used GPIO pins of the application processor(s),
activate the CMake Option **PYSODBUILDER_ENABLE**.

After a click on `Configure` the PYSODBUILDER options and the option
**USE_CUSTOMISED_FILES** will become available in the CMake GUI.
Pleas observe the information in \ref sect_pysodbuilder_cmakeconfig_general
regarding the behaviour when **USE_CUSTOMISED_FILES** is enabled.

In the folder **user_customisable**/[SelectedTarget] are now two files.
* app-gpio.c implements the configuration of the GPIO pins of the board.
* app.c implements the application and the processing of the pins and
  cyclic data. Assignment / processing has to be implemented. See the original
  app.c file as example.

Build the application and download it to the boards.

# Customising the processing of input and output data {#sect_customise_data_processing}

The customisation of the data processing in the application is identical with
\ref sect_customise_gpio, but without modifications to the file app-gpio.c

# Parameterization of the openSAFETY stack {#sect_os_parameterization}

For parameterizing the openSAFETY stack (i.e. EPLScfg.h), set the necessary
CMake Options (see \ref sect_pysodbuilder_cmakeconfig) and generate the
Makefiles.

When building the application, the necessary source files will be generated and
compiled into the binary.

> **Note:** Every time a build is invoked, the pysodbuilder tool generates
> the source files. If **USE_CUSTOMISED_FILES** is not enabled, the default
> generated files will be compiled into the binary. The generated app.c file
> does not implement the processing of the cyclic data / GPIO pins.
> For using and implementing such a processing, enable **USE_CUSTOMISED_FILES**
> and implement the app.c located in directory
> **user_customisable**/[SelectedTarget].


# Using a custom OSDD file {#sect_customise_osdd_file}

It is possible to generate customised source files using a custom OSDD file.
Create a OSDD file which fits your needs and set **PYSODBUILDER_OSDD_FILE**
in CMake accordingly.

After generating the Makefiles, build the target `pysodbuilder` to generate
the source files to the folder set by **PYSODBUILDER_OUTPUT_DIR** or build the
related application target directly.

> **Note:** Every time a build is invoked, the pysodbuilder tool generates
> the source files. If **USE_CUSTOMISED_FILES** is not enabled, the default
> generated files will be compiled into the binary. The generated app.c file
> does not implement the processing of the cyclic data / GPIO pins.
> For using and implementing such a processing, enable **USE_CUSTOMISED_FILES**
> and implement the app.c located in directory
> **user_customisable**/[SelectedTarget].

> **Note:** For testing purposes, it is recommended to adapt the existing OSDD
> file. Ensure that the payloads of the data to transmit of the new file
> do not exceed those of the original file. In this case, there are no further
> modifications to the XDD file and the PCP part needed