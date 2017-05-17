Fieldbus Protocols {#page_protocols}
============

[TOC]

# POWERLINK    {#sect_prot_powerlink}
POWERLINK is an Industrial Real-Time Ethernet protocol based on the Fast Ethernet
Standard IEEE 802.3. A Managing Node (MN), acting as the master in the POWERLINK
network, polls the controlled nodes (CN) cyclically. This process takes place in
the isochronous phase of the POWERLINK cycle. Immediately after the isochronous
phase an asynchronous phase for communication follows which is not time-critical,
e.g. TCP/IP communication. The isochronous phase starts with the Start of Cyclic
(SoC) frame on which all nodes are synchronized. This schedule design avoids
collisions which are usually present on standard Ethernet, and ensures the
determinism of the hard real-time communication. It is implemented in the POWERLINK
data link layer. The POWERLINK network can be connected via gateways to non
real-time networks.
POWERLINK supports up to 254 nodes. Node numbers 1 to 239 are used for standard
Controlled Nodes (CN). Node number 240 (F0h) is used for the Managing Node (MN).
Each CN publishes up to 1488 bytes of cyclic data in PRes frames, which can be
subscribed by all other CN. The MN can provide up to 1488 bytes of dedicated
cyclic data for each CN in PReq frames. In every POWERLINK cycle there is an
asynchronous slot, which can be used to transport configuration
and other acyclic data and standard Ethernet frames. All data on the POWERLINK
network is coded in LSB first (little endian, Intel format).

The communication profile of Ethernet POWERLINK is adapted from CANopen. Thus,
design principles such as process data object (PDO) for the exchange of process
variables and service data object (SDO) for the configuration of remote object
dictionaries are reused. All PDOs are exchanged within the isochronous phase,
similar to the synchronous PDOs of CANopen. This is because event-triggered PDOs
would interfere with hard real-time requirements.

To be conforming to IEEE 802.3, each POWERLINK device has a unique MAC address.
Additionally, each device is assigned a logical node ID. Mostly, this node ID can
be configured via node switches on the device. If a particular POWERLINK device
implements a TCP/IP stack, it gets a private IP address from class C within the
network 192.168.100.0 where the host part equals the POWERLINK node ID.

For more information about the protocol see the following pages:
- http://www.ethernet-powerlink.org
- https://sourceforge.net/projects/openpowerlink
- https://sourceforge.net/projects/openconf

# openSAFETY   {#sect_prot_opensafety}
openSAFETY is defined as a bus-independent, autonomous frame, which can in
principle also be inserted into standard protocols other than POWERLINK.
openSAFETY has been designed so that standard data and safety data transfer is
possible within the same network.

openSAFETY uses a variety of safety mechanisms. In addition to guarding data
content via CRC Codes, timing of the data that is being transferred is also
monitored independent of the actual (non-safe) transport protocol that is being
used. This allows openSAFETY to provide secure transfer of data across non-secure
networks.

Different transfer protocols and media such as CAN can also be used with openSAFETY.
openSAFETY only uses the non-secure transfer layer for exchanging the secured
openSAFETY Frames, whose contents are not interpreted by the transfer layer. Of
course, POWERLINK is the preferred transfer layer because of the close relationship
between the communication mechanisms in POWERLINK and openSAFETY, which enables an
ideal level of performance.

An openSAFETY network can consist of up to 1023 openSAFETY nodes (so called Safety
Nodes), and up to 1023 openSAFETY networks may exist in parallel on the same
fieldbus network. The openSAFETY domain can be spread across several, even
inhomogeneous networks. Special openSAFETY Domain Gateways enable communication
between openSAFETY domains.

For more information about the protocol see the following pages:
- http://www.open-safety.org/
- https://sourceforge.net/projects/opensafety
