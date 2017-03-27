SPI Bridge {#page_spibridge}
============

[TOC]

This four wire serial interface provides a lightweight communication channel from
the user application to the internal triple buffers. The SPI component consists
of two components where the first one handles the SPI slave core functionality and
the second one implements the SPI protocol.

The core subcomponent synchronizes the input signals to the local clock, reads/writes
data to the SPI shift register and forwards the data to the protocol component.
The protocol component implements a full duplex streamed access to and from
the external buffers where no additional commands need to be transferred.

On streaming access a basic communication cycle starts at the low address of its
first consuming buffer and forwards the data as a stream to the core component.
At the same time it reads arriving data from the core component and writes it to
the first producing buffer. Communication is therefore always full duplex and
should not be interrupted until both streams are transferred over the interface.

> In case of a reassertion of the SS signal both producing and consuming transfers
> are restarted. Therefore the SS signal needs to be active until the whole transfer
> is carried out.

![Internals of the SPI bridge component](spi_bride_core.png)
@image latex spi_bride_core.eps "Internals of the SPI bridge component" width=0.75\textwidth

# SPI bridge limits    {#sect_spibridge_limits}
The amount of data which can be transferred over the interface is limited by the
streaming protocol. The size of data depends on the maximum SPI clock frequency
the master and slave are able to handle. Currently the SPI bridge IP-Core is
able to handle a maximum frequency of **25Mhz**. In addition it is important that
the time between two SPI accesses is as short as possible. Therefore it is
recommended to use an SPI master with a Direct Memory Access (DMA). This enables
fast reading and writing to the local memory and reduces the gap between two bytes
during data transfers.

# IP-Core GUI {#sect_spibridge_gui}
The SPI bridge core provides a graphical user interface which enables easy adaptation
to the used SPI master. The following IP-Core configuration parameters are available:
- **Clock polarity** (CPHA)
- **Clock phase** (CPOL)
- **Shift direction:** MSB or LSB first
- **Register size:**
  * 8 bits
  * 16 bits (This option is experimental)
  * 32 bits (This option is experimental)
- **Enable continuous memory map:**: Enable this option if the SPI bridge IP-Core
  is used with the triple buffer core. This enables to read the data from the
  buffers in a continuous stream.
- **Read buffers base address:** Sets the base address of the input buffer
  (In case of a continuous stream this option is not available
- **Read buffers size:** This parameter needs to be adapted according to the
  triple buffer IP-Core settings.
- **Write buffers base address:** Sets the base address of the output buffer
  (In case of a continuous stream this option is not available)
- **Write buffers size:** This parameter needs to be adapted according to the
  triple buffer IP-Core settings.
