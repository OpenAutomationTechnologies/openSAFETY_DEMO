Hardware Design {#page_idxhardware}
============

[TOC]

This chapter discusses the general hardware requirements for POWERLINK Slaves
using the POWERLINK slim interface package to outline the usage of hardware
resources and provide a reference point for hardware development respectively.
Basically the system is organized to host an
FPGA for the PCP and one or two micro-controllers for the application processor.

- \subpage page_hw_pcp
- \subpage page_hw_app

This being said, the hardware used by the openSAFETY_DEMO perfectly fulfils
those requirements:

 - 1x Terasic DE2-115 FPGA Board as the PCP part http://www.terasic.com.tw/cgi-bin/page/archive.pl?Language=English&CategoryNo=139&No=502
 - 2x STMicroelectronics Nucleo boards as the application processors (NUCLEO-F401RE or NUCLEO-F103RB) for the dual channelled configuration of the Safe Node
  - NUCLEO-F401RE http://www.st.com/en/evaluation-tools/nucleo-f401re.html
  - NUCLEO-F103RB http://www.st.com/en/evaluation-tools/nucleo-f103rb.html
