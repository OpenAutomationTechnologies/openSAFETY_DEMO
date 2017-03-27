The openMAC core {#page_openmac}
============

[TOC]

The openMAC IP-Core is a standard MAC with RMII interface and a set of special
features which are needed to create a PLK CN with a high performance and a low
jitter. This additional features are listed as following:

* Sparse resources in FPGA
* Full- and half- duplex 100Mbps MAC
* RMII (reduced media independent interface)
* 3-port hub (openHUB)
* 16 RX filters with the ability to start a transmission
* First 31 bytes are filtered by the RX filters
* Auto-response feature
* Low through-put time
* 32bit free-running timer for time-stamps (RX and TX) and IRQ generation
* Distortion filter (openFILTER)

# Synchronous interrupt    {#sect_openmac_syncint}
The synchronous interrupt signal is asserted at SoC arrival. The interrupt itself
is edge triggered and is reset after a high pulse of 2us. (The length of the pulse
can be adjusted by using the SYNC_INT_PULSE_WIDTH_NS define in pcp/appif/status.c)
The user application processes the synchronous data on this interrupt and can
correct the drift of it's local clock by reading the global POWERLINK time from
the SoC frame.

If the application is not able to handle a synchronous interrupt in every cycle
the SYNC_INT_CYCLE_NUM (pcp/appif/status.c) define can be used to reduce the
number of interrupts to a multiple of the POWERLINK cycles.