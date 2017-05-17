Setting up the Hardware {#page_setup_hardware}
============

[TOC]

This page provides detailed information on how to setup the hardware of the
demo boards.



# Setting up the FPGA board {#sect_gs_fpgaboard_setup}

## TERASIC Industrial Networking Kit (INK) (terasic-de2-115) {#sect_gs_targxalt_terasicink}

The **terasic-de2-115** needs a couple of pre-settings which are shown in the
provided figure.

1. Set the jumpers **JP1**, **JP2**, **JP3**, **JP6** and **JP7**.
2. Set switch **SW19** to **Run**.
3. Connect the USB-Blaster to your PC via the USB-cable.
4. Connect one Ethernet port of the INK-board to a POWERLINK MN with the
   delivered cable.
5. Set the POWERLINK Node ID to **2**: Push **SW11** to upper position.
6. The board can be switched on with the Power button (**SW18**).

![Terasic INK - Peripherals](terasic-de2-115.png)
@image latex terasic-de2-115.eps "Terasic INK - Peripherals"

The INK PHYs are not configured for auto-crossing the Ethernet connections.
Thus, a link will only be established if one of the following conditions is met.

1. Patch-cable connected to MDI-X (Media Dependent Interface Crossed), e.g.
   industrial hub.
2. Crossover cable connected to MDI (Media Dependent Interface) e.g.
   POWERLINK MN X20CP1485-1 IF 3.
3. Either cable type connected to auto-crossover interface.

> **Note:** In field applications CAT 5e cables are required.

### Status / Error LEDs {#sect_gs_fpgaboard_leds}

LEDR0 and LEDG7 indicate the status of the board / the application runnning
on the board:

LEDG7            | State
:----------------|:--------
LED off          | Off, Initialising, Not Active
LED flickering   | Basic Ethernet Mode (no POWERLINK frame received)
LED single flash | pre operational state 1
LED double flash | pre operational state 2
LED on           | operational, POWERLINK communication is working
LED blinking     | Stopped


LEDR0            | State
:----------------|:--------
LED off          | No error
LED on           | error, POWERLINK connection lost


## PCP <-> Application - Interconnect  {#sect_gs_altsetuphw_interconnect}

In order to interconnect the POWERLINK processor with the application the GPIO
bank JP5 on the **terasic-de2-115** is used. The following figure shows the
pinout of this IO bank in detail.
In order to power-up external application boards the
**3.3V/5V** and **GND** pins may be used.

![Terasic INK - JP5 Pinout](terasic-de2-115_jp5.png)
@image latex terasic-de2-115_jp5.eps "Terasic INK - JP5 Pinout" width=0.75\textwidth


# Setting up the Nucleo boards {#sect_gs_nucleoboard_setup}

This section provides information on how to setup the Nucleo boards.
This applies to both boards, the NUCLEO-F103RB as well as the
NUCLEO-F401RE.

## Board setup - Single channelled    {#sect_gs_nucleo_setuphw_single}
In order to setup the board for the **demo-sn-gpio** in
single channelled configuration, carry out the following steps:
1. Connect the jumper **JP5** to pin **1-2**. (Power supply over E5V)
2. Connect the stm32 **E5V** power supply pins with the terasic-de2 **JP5**
  (5V, GND) pins.
3. Connect the **PCP** <-> **uP** interconnect the following way:
    Name   | terasic-de2 FPGA pin (board connector JP5 pin) | stm32f401 (board connector CN5 pin)
    :------|:------------|:---------
    sync   | AC15 (2)    | PC7  (2)
    SS_n   | Y17 (4)     | PB6 (3)
    MOSI   | Y16 (6)     | PA7 (4)
    MISO   | AE16 (8)    | PA6 (5)
    SCK    | AE15 (10)   | PA5 (6)
    GND    | (12)        | (7)

4. For programming and debugging connect the USB connector with your PC.
 > **Note:** Take care to always connect the USB connector after the board is
 > powered up. Otherwise it is not possible to program the board and the LED **LD1**
 > glows orange.

![Setup NUCLEO-F103RB / NUCLEO-F401RE (Single channelled)](stm32_nucleo_single.png)
@image latex stm32_nucleo_single.eps "Setup NUCLEO-F103RB / NUCLEO-F401RE (Single channelled)" width=0.6\textwidth


## Board setup - Dual channelled    {#sect_gs_nucleo_setuphw_dual}
For a setup with two redundant **demo-sn-gpio** processors a special wiring is
needed. The following figure provides an overview of the wiring of the two boards.

> **Note:** You can use the bottom end of the E5V pins of uP-Master to forward
> the connection to uP-Slave. In addition most of the pins are wired to the
> **Morpho** and **Arduino** connector.

![Setup NUCLEO-F103RB / NUCLEO-F401RE (Dual channelled)](stm32_nucleo_dual.png)
@image latex stm32_nucleo_dual.eps "Setup NUCLEO-F103RB / NUCLEO-F401RE (Dual channelled)" width=1\textwidth

Carry out the following steps in order to wire a dual channelled demo:
1. Setup one board **exactly** as described in \ref sect_gs_nucleo_setuphw_single
2. Also connect the jumper **JP5** to pin **1-2** of **uP-Slave**.
3. Connect the uP-Slave **E5V** power supply pins with the ones from **uP-Master**.
   (You can use the bottom side end of the uP-Master pins!)
4. Wire the SPI bus **uP-Master** <-> **uP-Slave** in the following way:
    Name    | uP-Master | uP-Slave | Name
    :-------|:----------|:---------|:---------
    SCK     | PA5       | PA5      | SCK
    MISO    | PA6       | PA7      | MOSI
    SS_n    | PB6       | PA4      | SS_n
    sync    | PC7       | PC7      | sync

5. Wire the UART from **uP-Master** <-> **uP-Slave** bus the following way:
    Name    | uP-Master | uP-Slave | Name
    :-------|:----------|:---------|:---------
    UART_RX | PA9       | PA10     | UART_TX
    UART_TX | PA10      | PA9      | UART_RX



# Connecting the boards {#sect_gs_board_connection}

The overall connection of the openSAFETY_DEMO hardware for a **dual-channelled**
configuration is depicted below. For a **single-channelled** configuration,
the uP-Slave and related wiring may be omitted.
 > **Note:** On the Nucleo boards, the pins of CN5 are be connected with the
 > odd pins of CN10. There is no connection between even pins of CN10 and
 > the odd pins of CN10.
 > The pins of CN8 are connected with the even numbered pins of CN7. There
 > is no connection between even and odd numbered pins of CN7.
 > For further information refer to the schematics of the Nucleo boards.



![Board Connections] (connection_pcp_app.png)
@image latex connection_pcp_app.png "Board Connections" width=1\textwidth
