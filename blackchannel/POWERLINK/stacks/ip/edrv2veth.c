/**
********************************************************************************
\file   edrv2veth.c

\brief  Wrapper module to connect uIP stack to Virtual Ethernet driver

This module converts the driver interface of the IP stack to the interface
provided from the Virtual Ethernet driver NoOs of openPOWERLINK stack.

\ingroup module_ip
*******************************************************************************/

/*------------------------------------------------------------------------------
* License Agreement
*
* Copyright (c) 2013, B&R Industrial Automation GmbH
* Copyright (c) 2016, Kalycito Infotech Pvt. Ltd
* All rights reserved.
*
* Redistribution and use in source and binary forms,
* with or without modification,
* are permitted provided that the following conditions are met:
*
*   * Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer
*     in the documentation and/or other materials provided with the
*     distribution.
*   * Neither the name of the B&R nor the names of its contributors
*     may be used to endorse or promote products derived from this software
*     without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <oplk/oplk.h>
#include <ip.h>
#include <socketwrapper.h>

#include "socketwrapper-int.h"
#include "edrv2veth.h"

#ifdef __NIOS2__
#include <sys/alt_alarm.h>
#endif

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------


//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

#define GET_TYPE_BASE(typ, element, ptr)    \
    ((typ*)( ((size_t)ptr) - (size_t)&((typ*)0)->element ))

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

/**
\brief  Receive buffer descriptor

This structure defines the receive buffer descriptor.
*/
typedef struct
{
    BOOL            fIpStackOwner;      ///< TRUE if the IP stack owns the buffer
    ULONG           length;             ///< Payload length
    UINT8           aBuffer[IP_MTU];    ///< Receive buffer
} tEdrv2VethRxDesc;

/**
 * \brief Instance structure of the socket wrapper module
 */
typedef struct
{
    IP_STACK_H              pIpStack;                   ///< Pointer to the IP stack handle.
    tEdrv2VethRxDesc        aRxBuffer[IP_RX_BUF_CNT];   ///< Edrv2Veth Receive Buffer descriptor.
    ipState_enum            ipState;                    ///< Current state of the IP Stack.
    tNmtState               nmtState;                   ///< Current state of the POWERLINK CN.
} tEdrv2VethInstance;

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

static tEdrv2VethInstance  edrv2vethInstance_l;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void  freePacket(ip_packet_typ* pPacket_p);
static ULONG ipEthSendCb(void* hEth_p, ip_packet_typ* pPacket_p,
                         IP_BUF_FREE_FCT* pfnFctFree_p);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief Initialize wrapper module

\param  pEthMac_p[out]   Pointer to the address where the MAC should be stored.

\return tOplkError
\retval kErrorOk         Success

\ingroup module_ip
*/
//------------------------------------------------------------------------------
tOplkError edrv2veth_init (eth_addr* pEthMac_p)
{
    tOplkError      ret = kErrorOk;
    UINT8           aMacAddr[6];
    struct in_addr  ipAddr;

    memset(&edrv2vethInstance_l, 0, sizeof(tEdrv2VethInstance));

    // Copy default MAC address
    ret = oplk_getEthMacAddr(aMacAddr);
    if (ret != kErrorOk)
        return ret;

    memcpy(pEthMac_p, aMacAddr, sizeof(eth_addr));

    ipAddr.S_un.S_addr = 0; // Set invalid address, correct address is set later!

    edrv2vethInstance_l.pIpStack = ipInit(pEthMac_p, &ipAddr, ipEthSendCb, NULL);
    if (edrv2vethInstance_l.pIpStack == NULL)
    {
        PRINTF("%s(Err/Warn): Error when initializing IP stack\n", __func__);
        ret = kErrorNoResource;
        return ret;
    }

    edrv2vethInstance_l.ipState = IP_STATE_INIT_ARP;

    ret = socketwrapper_setIpStackHandle(edrv2vethInstance_l.pIpStack);
    if (ret != kErrorOk)
    {
        PRINTF("%s(Err/Warn): Error when setting IP stack handle to socketwrapper\n",
               __func__);
    }
    return ret;
}

//------------------------------------------------------------------------------
/**
\brief Exit wrapper module

\ingroup module_ip
*/
//------------------------------------------------------------------------------
void edrv2veth_exit (void)
{
    if (edrv2vethInstance_l.pIpStack != NULL)
        ipDestroy(edrv2vethInstance_l.pIpStack);

    edrv2vethInstance_l.pIpStack = NULL;
}

//------------------------------------------------------------------------------
/**
//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

//------------------------------------------------------------------------------
\brief Change IP address, Subnet mask and MTU of the IP stack

\param  ipAddr_p       The IP address of the node to set
\param  subNetMask_p   The Subnet mask of the node to set
\param  mtu_p          The MTU of t he node to set

\ingroup module_ip
*/
//------------------------------------------------------------------------------
void edrv2veth_changeAddress(UINT32 ipAddr_p, UINT32 subNetMask_p, UINT16 mtu_p)
{
    struct in_addr ipAddr;

    UNUSED_PARAMETER(mtu_p);

    ipAddr.S_un.S_addr = htonl(ipAddr_p);

    ipChangeAddress(edrv2vethInstance_l.pIpStack, &ipAddr);

    ipAddr.S_un.S_addr = htonl(subNetMask_p);

    ipSetNetmask(edrv2vethInstance_l.pIpStack, &ipAddr);
}

//------------------------------------------------------------------------------
/**
\brief Change the default gateway of the IP stack

\param  defGateway_p       The default gateway of the node to set

\ingroup module_ip
*/
//------------------------------------------------------------------------------
void edrv2veth_changeGateway( UINT32 defGateway_p )
{
    struct in_addr ipGateway;

    ipGateway.S_un.S_addr = htonl(defGateway_p);

    ipSetGateway(edrv2vethInstance_l.pIpStack, &ipGateway);
}

//------------------------------------------------------------------------------
/**
\brief Set the openPOWERLINK node NMT state

\param  nmtState_p      The node's current NMT state.

\ingroup module_ip
*/
//------------------------------------------------------------------------------
void edrv2veth_setNmtState(tNmtState nmtState_p)
{
    edrv2vethInstance_l.nmtState = nmtState_p;
}

//------------------------------------------------------------------------------
/**
\brief Receive handler of the wrapper module

Handles an incoming frame from the Virtual Ethernet driver and forwards it
to the IP stack.

\param  pFrame_p       Pointer to the incoming payload.
\param  frameSize_p    Size of the incoming payload.

\return tOplkError
\retval kErrorOk          On success

\ingroup module_ip
*/
//------------------------------------------------------------------------------
tOplkError edrv2veth_receiveHandler(UINT8* pFrame_p, UINT32 frameSize_p)
{
    tOplkError     ret = kErrorOk;
    INT            rcvStatus;
    ip_packet_typ* pPacket;
    INT            i;

    for (i=0; i<IP_RX_BUF_CNT; i++)
    {
        if (!edrv2vethInstance_l.aRxBuffer[i].fIpStackOwner)
            break;
    }

    if (i == IP_RX_BUF_CNT)
    {
        // No free buffer found => ignore frame
        PRINTF("%s (Err/Warn): No free buffer found, Frames ignored\n",
        __func__);
        return kErrorOk;
    }

    // i points to a free buffer!
    edrv2vethInstance_l.aRxBuffer[i].fIpStackOwner = TRUE;
    pPacket = (ip_packet_typ*)&edrv2vethInstance_l.aRxBuffer[i].length;
    pPacket->length = frameSize_p;
    memcpy(pPacket->data, pFrame_p, frameSize_p);

    // Forward incoming data to IP stack
    rcvStatus = ipPacketReceive(edrv2vethInstance_l.pIpStack,
                                pPacket, freePacket);
    if (rcvStatus != 0)
    {
        // Call free function now
        freePacket(pPacket);
    }

    return ret;
}

//------------------------------------------------------------------------------
/**
\brief Process function

This function forwards processing time to the IP stack.

\return tOplkError
\retval kErrorOk          On success

\ingroup module_ip
*/
//------------------------------------------------------------------------------
tOplkError edrv2veth_process(void)
{
    UINT32  timeTick;

#ifdef __NIOS2__
    timeTick = alt_nticks();
#endif

    if (edrv2vethInstance_l.nmtState > kNmtCsNotActive)
    {
        if ((edrv2vethInstance_l.ipState == IP_STATE_INIT_ARP) &&
            (edrv2vethInstance_l.nmtState != kNmtCsBasicEthernet))
        {
            ipDisableInitArp(edrv2vethInstance_l.pIpStack);
        }

        edrv2vethInstance_l.ipState = ipPeriodic(edrv2vethInstance_l.pIpStack,
                                                 timeTick);

        //TODO: Handle IP_STATE_ERROR_* ?
    }

    return kErrorOk;
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

//------------------------------------------------------------------------------
/**
\brief Free the packet buffer after transmission

\param  pPacket_p       Pointer to the packet buffer for freeing

*/
//------------------------------------------------------------------------------
static void freePacket(ip_packet_typ* pPacket_p)
{
    INT i;

    for (i = 0; i < IP_RX_BUF_CNT; i++)
    {
        if ((ip_packet_typ*)&edrv2vethInstance_l.aRxBuffer[i].length == pPacket_p)
        {
            edrv2vethInstance_l.aRxBuffer[i].fIpStackOwner = FALSE;
            break;
        }
    }

    if (i == IP_RX_BUF_CNT)
    {
        PRINTF("%s(Err/Warn): Error while freeing the Veth receive buffer\n",
               __func__);
    }

}

//------------------------------------------------------------------------------
/**
\brief Transmit to lower layer

\param  hEth_p          Ethernet driver handle
\param  pPacket_p       Packet to be transmitted
\param  pfnFctFree_p    Pointer to function freeing packet buffers

*/
//------------------------------------------------------------------------------
static ULONG ipEthSendCb(void* hEth_p, ip_packet_typ* pPacket_p, IP_BUF_FREE_FCT* pfnFctFree_p)
{
    tOplkError  ret;
    ULONG       plkLength;

    UNUSED_PARAMETER(hEth_p);

    ret = oplk_sendEthFrame((tPlkFrame*)pPacket_p->data, pPacket_p->length);
    if (ret == kErrorOk)
    {
        plkLength = pPacket_p->length;
        pfnFctFree_p(pPacket_p);
    }
    else
    {
        PRINTF("%s(Err/Warn(: Error while sending frame\n", __func__);

        plkLength = 0;
    }

    return plkLength;
}

/// \}
