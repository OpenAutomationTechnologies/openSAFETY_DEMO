Needed Hardware {#page_overview_hardware}
============

This page introduces the hardware used for the openSAFETY_DEMO Safe Node itself
as well as the additional hardware needed for system integration.

> **Note:** The openSAFETY_DEMO has been successfully brought into operation
> with the used hardware and software, related revisions and
> versions stated in this guide.
> Depending on user needs and requirements, other hardware, software or
> related versions may be used, but this is out of the scope of the
> openSAFETY_DEMO and this documentation.

[TOC]

# Hardware used by the openSAFETY_DEMO {#sect_used_ardware}
The openSAFETY_DEMO Safe Node consists of the following hardware:

- 1x [Terasic DE2-115 Development and Education Board] (http://www.terasic.com.tw/cgi-bin/page/archive.pl?Language=English&CategoryNo=139&No=502) or [Terasic DE2-115 Industrial Networking Kit (INK)] (http://www.terasic.com.tw/cgi-bin/page/archive.pl?Language=English&CategoryNo=&No=511)
- 2x STMicroelectronics Nucleo boards
 - [NUCLEO-F401RE] (http://www.st.com/web/catalog/tools/FM116/SC959/SS1532/LN1847/PF260000) or [NUCLEO-F103RB] (http://www.st.com/web/en/catalog/tools/FM116/SC959/SS1532/LN1847/PF259875)
 > **Note:** It is highly recommended to use HW revision C-03 of the Nucleo
 > boards due to the connected external clock signal (derived from debug adapter
 > crystal), which is used as system clock. This signal is not connected on
 > boards with earlier HW revisions. Nevertheless, such boards can be modified
 > with some soldering effort.
 > The HW revision is indicated by a sticker on the bottom side of the
 > Nucleo board.
 > See the [Nucleo User Manual](http://www.st.com/st-web-ui/static/active/en/resource/technical/document/user_manual/DM00105823.pdf)
 > for further information.

![Hardware of the openSAFETY_DEMO Safe Node](openSAFETY_DEMO_HW.png)
@image latex openSAFETY_DEMO_HW.png "Hardware of the openSAFETY_DEMO Safe Node" width=0.75\textwidth

# Additional hardware for system integration {#sect_gs_br_hardware}

For a complete system integration i.e. putting the demo into operation in an
B&R openSAFETY system, the following hardware is needed.
Please contact your local B&R representation regarding further product
information and purchasing.

- [B&R X20CP1585 CPU](http://www.br-automation.com/en/products/control-systems/x20-system/x20-cpus/x20cp1585/) + Compact Flash card (eg. 5CFCRD.0512-06 CompactFlash 512 MB)
- [B&R X20SL8100 SafeLOGIC] (http://www.br-automation.com/en/products/safety-technology/x20-system/cpus/x20sl8100/) + [SafeKEY X20MK0211](http://www.br-automation.com/en/products/safety-technology/x20-system/accessories/x20mk0211/)
  or [SafeKEY X20MK0213](http://www.br-automation.com/en/products/safety-technology/x20-system/accessories/x20mk0213/)
  with license for 20 safe nodes
- suitable power supply, eg. [B&R 0PS1020.0 Power Supply](http://www.br-automation.com/en/products/power-supplies/single-phase-power-supplies/0ps10200/)

![B&R Hardware needed for System Integration](BR_HW.png)
@image latex BR_HW.png "B&R Hardware needed for System Integration" width=0.75\textwidth

# Wiring {#sect_wiring}

- 2x USB cable type A to mini-B (male / male) for conecting the Nucleo boards to the computer
- 4x Ethernet cables for connecting the hardware via POWERLINK, configuring the PLC, (and doing Wireshark traces)
- Wires for connecting the FPGA / microcontroller boards
- Cabling for supplying power to B&R hardware