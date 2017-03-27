More details on the openSAFETY_DEMO {#page_gs_demo_detail}
==================

[TOC]
# openSAFETY_DEMO {#sect_gs_demo_detail}

This page provides information on the parts of the openSAFETY_DEMO, their
interaction and the limits of the demo.

![openSAFETY_DEMO Safe Node](openSAFETY_DEMO_HW.png)
@image latex openSAFETY_DEMO_HW.png "openSAFETY_DEMO Safe Node"

## POWERLINK processor (PCP)  {#sect_gs_sysdesc_oS_demo_pcp}
In order to handle the real time requirements of POWERLINK and to provide the
lowest possible cycle times, the PCP is implemented on a FPGA where the most
time critical parts are realized as soft IP-Cores. It consists of the
openMAC IP-Core for access to the POWERLINK network and the POWERLINK
interface IP-Core for an interface to the user application.
More about these IP-Cores is given in the
section \ref page_idxipcore.

The software parts of the POWERLINK interface, like the protocol stack and the
interface drivers, are executed on a soft processor inside the FPGA and are
therefore out of the scope of an application engineer. Only basic configuration
steps need to be carried out inside the code of the PCP (See \ref sect_sw_pcp).

## POWERLINK Slim Interface Library (PSI)   {#sect_gs_sysdesc_oS_demo_libpsi}

The openSAFETY_DEMO uses a lightweight modular interface to connect the
application processor to the PCP and the POWERLINK network behind this
processor. Due to it's characteristics, it is therefore limited in it's
functionality, like only providing process and service data
(See \ref sect_gs_sysdesc_oS_demo_limits).

The PSI library is executed on the application processors and
provides an interface for the user application.
More details about the library can be found in the \ref group_libpsi and
\ref group_libpsicommon.

## Interconnect PCP <-> User Application   {#sect_gs_sysdesc_oS_demo_interconn}
The application processor uses a serial peripheral interface (**SPI**) as an
interconnect to access the PCP.
In addition to this serial device, a synchronous interrupt line
is connected to the application processor. This line is used to periodically
trigger the SPI master on the application processor which then starts a
new cyclic data transfer.

## User Application, Safe Node - Demo {#sect_gs_sysdesc_app_sndemo}
On the application processors, a safe node demo (**demo-sn-gpio**) is
implemented as the user application. It realises an openSAFETY SN which uses the
PSI interface to access the POWERLINK processor.

In dual channel configuration, the two application processors exchange data
via a UART interface. This is needed for e.g. handshakes during boot-up,
synchronisation, exchange of openSAFETY stack flow counter values or
openSAFETY frames.
See \ref group_app_sn for information on the data flow and the utilisation of
**SPI** and **UART**.

In order to enable the special requirements of openSAFETY over
POWERLINK the PSI interface needs to be configured with the following
modules:
- \ref module_psi_status

    This interface internal module is used to forward the time information of
    POWERLINK to the application. In addition it internally handles the
    asynchronous transfer of the SSDO and logbook channel.

- \ref module_psi_pdo

    The PDO module is configured to forward the SPDO input and output images
    periodically. This demo is configured to receive one RSPDO image with the
    size \ref RX_SPDO0_SIZE and one TSPDO with the size \ref TX_SPDO_SIZE.
    The TSPDO channel is mapped to the POWERLINK object **0x4000**
    subindex **0x1** which is assigned by the macro
    \ref TPDO_LINKING_LIST_INIT_VECTOR.
    The RSPDO channel is mapped to the POWERLINK object **0x4001**
    subindex **0x1** which is assigned by the macro
    \ref RPDO_LINKING_LIST_INIT_VECTOR.

- \ref module_psi_ssdo

    The SSDO module provides the asynchronous channels of the SN demo where
    for the demo application only one asynchronous channel is configured.
    The macros \ref SSDO_STUB_DATA_DOM_SIZE and \ref SSDO_STUB_OBJECT_INDEX
    define the size and object of the SSDO receive channel.
    The macros \ref TSSDO_TRANSMIT_DATA_SIZE and
    \ref SSDO_STUB_DATA_OBJECT_INDEX define the size and object of the transmit
    channel.

- \ref module_psi_log

    The logbook module implements an asynchronous channel of the SN which is
    dedicated to send asynchronous messages in a format which can be read by
    the B&R Automation Studio logbook.

## Limits of the demo application {#sect_gs_sysdesc_oS_demo_limits}
In order to reduce the code size at the application processor also the feature
set of the PSI is limited. The following list gives details about the limits
of the interface:

- The application is not able to write configuration parameters to the PCP
  during initialization.
- The number of RPDOs is limited to one.
- Sending and receiving of SDOs at the application is not possible.
  * Access to the data of unique objects can be achieved by using the
  \ref module_psi_cc.
- There is no Virtual Ethernet interface on the application.
  (It is therefore not possible to send or receive any generic Ethernet frame
   at the application)
