Revision History {#page_revision}
==================

[TOC]

Revision history of the openSAFETY_DEMO.

# V1.2.1    {#sect_rev_v121}

This is the latest (intermediate) release of the V1.2 release series.
This release contains fixes and optimizations.

Following is a summary of changes in V1.2.1.
For a detailed revision history refer to the Git source code history.

* `Features and changes:`
  - Improve latency of TxPDO data
  - Use openSAFETY stack 1.5.2
  - Update documentation
  - Improve display of device info in design tool with new device description file


# V1.2.0    {#sect_rev_v120}

This is the first release of the V1.2 release series.
This release contains fixes and optimizations.

Following is a summary of changes in V1.2.
For a detailed revision history refer to the Git source code history.

* `Features and changes:`
  - Improve performance by adapting / correcting tightly coupled memory parts
  - Update device description files regarding POWERLINK conformance and
openSAFETY test vendor ID
  - Update git submodule for openPOWERLINK
  - Update documentation
  - Stability improvements
  - Update Cortex-M3 / M4 HAL libraries related code
* `Fixes:`
  - Fix build system issues
  - Fix reset guarding SCM issue
  - Fix logger messages with invalid timestamps

# V1.1.0    {#sect_rev_v110}

This is the latest release of the V1.1 release series.
This release contains fixes and optimizations.

Following is a summary of changes in V1.1.
For a detailed revision history refer to the Git source code history.

* `Features and changes:`
  - Use openPOWERLINK stack V2.5.2
  - Use openSAFETY stack V1.5
  - Use git submodules for openPOWERLINK and openSAFETY
  - Comply with EPSG DS 301 V1-3-0
  - Ease handling for safe parameters
  - Support newer versions of STM32 HAL library
  - Update documentation
  - Remove unused parts
* `Fixes:`
  - Fix blocking of asynchronous communication
  - Fix build system issues
  - Fix application issues


# V1.0.0    {#sect_rev_v100}

Initial release of the openSAFETY_DEMO.

* `Features:`
  - openPOWERLINK stack PSI driver.
  - PCP port to target board terasic-de2-115 with demo:
     * cn-pcp-spi
  - PSI user libraries:
     * libpsi
     * libpsicommon
  - PSI user library unit tests.
  - Application processor demos:
     * demo-sn-gpio
        - Single channelled
        - Dual channelled
  - Demos ported to platforms:
     * terasic-de2-115
     * stm32f103rb
     * stm32f401re
  - Full documentation in doxygen.
  - Common build system for all platforms (CMake)
* `Fixes:`
  - none
