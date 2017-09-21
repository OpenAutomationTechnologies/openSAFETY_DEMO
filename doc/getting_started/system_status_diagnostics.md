Observing the System Status / Diagnostics{#page_system_status_diagnostics}
============

This page provides information about using the system and observing its status.

[TOC]

Make sure all the used hardware is powered up and flashed with the appropriate
software.

# FPGA Board {#sect_gs_system_integration_status_fpga}

If the software of the FPGA board / PCP was built with CMAKE_BUILD_TYPE=Debug,
the debug output of the PCP can be monitored by opening a
`Nios II Command Shell` and executing

        > nios2-terminal -i 0

See \ref sect_gs_build_fpga_sw_debug_com for further information.

# Nucleo Boards {#sect_gs_system_integration_status_nucleo}

If the software of the Nucleo boards was built with CMAKE_BUILD_TYPE=Debug,
the debug output of the openSAFETY_DEMO application processors can be
monitored by opening a terminal program and opening the virtual COM port.

The following settings are needed for the COM port:
- Port: Virtual COM port of the Nucleo board
- Baud rate: 115200
- Data: 8bit
- Paraity: none
- Stop: 1 bit
- Flow control: none

See \ref sect_gs_nucleo_buildsw_debug and
\ref sect_gs_nucleo_buildsw_debug_virtual_com for further information.

# PLC {#sect_gs_system_integration_status_plc}

There are several ways to obtain information about the system:

## Target System Status

The status of the target system / PLC is shown in the bottom right corner.
Make sure, that you are connected with the target system
(see \ref sect_gs_as_connect).

![Automation Studio, Monitor mode and Target System Status](as_monitor_status.png)
@image latex as_monitor_status.png "Automation Studio, Monitor mode and Target System Status" width=0.75\textwidth

## Monitor

The Monitor mode enables viewing the status of an executed PLC program / it's
variables, the channels or I/O status of the projected devices inside
Automation Studio.
To activate the Monitor mode click the button `Monitor` in the Online toolbar.

![Automation Studio, Monitor mode, status of the openSAFETY_DEMO channels](as_monitor_activated.png)
@image latex as_monitor_activated.png "Automation Studio, Monitor mode, status of the openSAFETY_DEMO channels" width=0.75\textwidth

## Logger

The logger is a part of Automation Studio and enables the user to
view diagnostic data obtained from the PLC and other hardware connected to it.

The logger can be opened via the menu `Open -> Logger` or by pressing `CTRL+L`.

![Automation Studio, Logger](as_logger.png)
@image latex as_logger.png "Automation Studio, Logger" width=0.75\textwidth

# SafePLC {#sect_gs_system_integration_status_safeplc}

When the SafePLC is in RUN mode, the status of the variables of the
openSAFETY_DEMO device can be observed in the SafeDESIGNER by
clicking the button `Variable status`.

> **Note:** The openSAFETY_DEMO device files contain a vendor ID, which may be
> used only for test purposes or during development.
> Therefore, the SafePLC will show a continuously fast flashing FAILSAFE LED.
> This type of flashing indicates the usage of a test / pilot firmware,
> an application created with a test / pilot version of SafeDESIGNER or
> using a safety module with device files containing the test vendor ID.

![SafeDESIGNER, Variable status](as_sd_variablestatus.png)
@image latex as_sd_variablestatus.png "SafeDESIGNER, Variable status" width=0.75\textwidth

# Wireshark {#sect_gs_wireshark}

Wireshark can be used to analyse / view the POWERLINK and openSAFETY frames or
data transmitted via network. It is therefore useful for diagnosis and tracking
down errors caused by network issues as well as configuration errors.

> **Note:** When using common network cards for capturing network traces,
> timing information of the captured packets may be imprecise due to
> processing delays of the operatig system. For precise time information
> of the captured packets, dedicated hardware would be needed.

> **Note:** It is recommended to use an additional
> network card for doing the wireshark traces to avoid tracing frames
> which are sent by the operating system itself.
> On Windows operating systems, make sure to deactivate all services
> related to the network card:
> - Go to the system settings and network interfaces
> - Right click on the network interface which should be used for tracing
>   select `Properties`
> - In the new dialog, uncheck all boxes and click `OK`

## How to do a Network Trace of the System

Generally a trace including the system's boot-up phase and a part of normal
/ cyclical operation is sufficient for tracking down most common errors.

Carry out the following steps to do a trace:

- Turn off all hardware.
- Connect the second Ethernet port of the FPGA board with the (second) Ethernet
  interface using a POWERLINK cable or an Ethernet cable.
- Start Wireshark and double click on the interface used for tracing to
  start capturing the network trace.
- Power up all hardware
- After a few seconds, Wireshark will start showing captured network packets.
- When captured enough packets, stop capturing.
  Usually tracing for 60 - 100 seconds after power up will be sufficient to
  capture the boot-up and normal operation.
- When saving the trace, checking the checkbox `Compress with gzip` will greatly
  reduce the file size.

## Analysing the Trace

![Wireshark](wireshark_general.png)
@image latex wireshark_general.png "wireshark" width=0.75\textwidth

To filter the trace for packets containing openSAFETY frames,
enter `opensafety` in the filter text box and apply it.

![Wireshark, filtering for packets with openSAFETY frames](wireshark_filter.png)
@image latex wireshark_filter.png "Wireshark, filtering for packets with openSAFETY frames" width=0.75\textwidth

To filter the trace for packets containing openSAFETY frames which do not
contain process data (cyclic data) enter
`(opensafety && !(opensafety.msg.type== 0x06))` in the filter text box and
apply it.
This will list only packets with openSAFETY frames used for network management
(SNMT) and service data objects (SSDO).

![Wireshark, filtering for packets with openSAFETY frames not containing process data](wireshark_filter_acyclic.png)
@image latex wireshark_filter_acyclic.png "Wireshark, filtering for packets with openSAFETY frames not containing process data" width=0.75\textwidth

Each openSAFETY frame can be viewed in detail by double clicking on the
captured packet or expanding the tree in detail view.
Below, detailed frame information for an openSAFTEY SSDO frame and an
openSAFETY SPDO frame is shown.

![Wireshark, openSAFETY SSDO frame in detail: Safe Node responding with DeviceVendorID](wireshark_ssdo_dvi_detail.png)
@image latex wireshark_ssdo_dvi_detail.png "WWireshark, openSAFETY SSDO frame in detail: Safe Node responding with DeviceVendorID" width=0.75\textwidth


![Wireshark, openSAFETY SPDO frame in detail: Safe Node sending process data](wireshark_spdo_detail.png)
@image latex wireshark_spdo_detail.png "Wireshark, openSAFETY SPDO frame in detail: Safe Node sending process data" width=0.75\textwidth


## How a trace with successful openSAFETY start-up looks like
Generally a system boot-up in respect to openSAFETY follows this sequence:

- Assignment of Safe Address
 * Safe PLC assigns Address to Safe Node (SNMT Assign SADR)
 * Acknowledge from Safe Node (SNMT SADR Assigned)

- Assignment of SCM UDID
 * Safe PLC assigns UDID of Safe Configuration Manager
   (SNMT Service Request, SN assign UDID SCM)
 * Acknowledge from Safe Node (SNMT Service Response, SN assgined UDID SCM)

- Read Device Vendor Information
 * Safe PLC requests Device Vendor Information: VendorID, ProductCode,
   RevisionNumber, Parameter Checksum (SSD0 Service Response,
   VendorID, ProductCode, RevisionNumber, Parameter Checksum)
 * Safe Node responses to each request (SSDO Service Response)
 * If data to be accessed is a domain object (variable length),
   a segmented transfer will be done, until all data is transferred
   (SSDO Service Request, Upload Initiate Segmented, Upload Middle Segment,
   Upload End Segment)

- If the parameter checksum and related timestamp do not match with the ones
  expected by the Safe PLC, the parameter set will be downloaded to the Safe
  Node (SSDO Service Request / SSDO Slim Service Request,
  Download Initiate Segmented, Download Middle Segment, Download End Segment)

- Set the Safe Node to Operational states
 * Safe PLC requests the Safe Node to change to Operational state
  (SNMT Service Request, SN set to Operational)
 * If the Safe Node is not ready, it will respond with busy
   (SNMT Service Respond, SN Busy)
 * If the Safe Node is ready, it will respond with status Operational
   (SNMT Service Response, SN Status Operational)

- Cyclic data will be exchanged (SPDO Data only / SPDO Data with Time Request
  or Time Response)

- Time synchronisation between Safe PLC / Safe Node and Safe Node / Safe PLC
  will be done (SPDO Data with Time Request / SPDO Data with Time Response)

- When synchronisation is finished, Connection Valid bit will be set,
  the data sent via SPDO can now considered to be valid and safe

- Time synchronisation will be repeated in regular intervals

- Node Guarding will be repeated in regular intervals
 * Safe PLC sends request (SNMT Service Request, SCM guard SN)
 * Safe Node responds with current status (SNMT Service Response,
   SN status Operational)

> **Note:** The openSAFETY_DEMO bundle contains a sample Wireshark trace
> of a successful start-up. This may help to get a better understanding of
> openSAFETY and the described flow.

## Further Information on openSAFETY

For further information on the openSAFETY protocol, the frame types and services
please refer to the **openSAFETY Profile Specification** located at
https://www.ethernet-powerlink.org/en/downloads/technical-documents/.
