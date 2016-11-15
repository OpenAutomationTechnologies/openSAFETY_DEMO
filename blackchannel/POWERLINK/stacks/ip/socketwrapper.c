/**
********************************************************************************
\file   socketwrapper.c

\brief  Socket wrapper module to connect the IP stack with SDO/UDP module.

This module connects the IP stack with the SDO/UDP module.

\ingroup module_ip
*******************************************************************************/

/*------------------------------------------------------------------------------
* License Agreement
*
* Copyright 2013 BERNECKER + RAINER, AUSTRIA, 5142 EGGELSBERG, B&R STRASSE 1
* Copyright (c) 2016, Kalycito Infotech Pvt. Ltd.
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
#include <socketwrapper.h>

#include "ip.h"
#include "edrv2veth.h"
#include "socketwrapper-int.h"

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

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

typedef struct
{
    BOOL                    fInitialized;      ///< True if socketwrapper instance shall be created.
    IP_STACK_H              pIpStackHandle;    ///< Pointer to IP stack handle.
    tSocketWrapperAddress   socketAddress;     ///< Address of Socket Wrapper.
    tSocketWrapperReceiveCb socketReceiveCb;   ///< Receive callback function for the socket wrapper.
} tSocketWrapInstance;

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static tSocketWrapInstance  instance_l;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void       receiveFromSocket(void* pArg_p, ip_udp_info* pInfo_p);
static tOplkError updateIpStack(tSocketWrapInstance* pInstance_p);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief  Create socket wrapper instance

The function creates a socket wrapper instance.

\param  pfnReceiveCb_p      Socket receive callback

\return The function returns the created socket wrapper instance.
\retval NULL    Error, no socket wrapper instance was created!

\ingroup module_socketwrapper
*/
//------------------------------------------------------------------------------
tSocketWrapper socketwrapper_create(tSocketWrapperReceiveCb pfnReceiveCb_p)
{
     if (pfnReceiveCb_p == NULL)
         return NULL;

    memset(&instance_l, 0, sizeof(instance_l));

    instance_l.fInitialized = TRUE;
    instance_l.socketReceiveCb = pfnReceiveCb_p;

    return (tSocketWrapper)&instance_l;
}

//------------------------------------------------------------------------------
/**
\brief  Bind to socket wrapper instance

The function binds to a certain IP address and port.

\param  pSocketWrapper_p    Socket wrapper instance.
\param  pSocketAddress_p    Socket address structure.

\return The function returns a tOplkError error code.

\ingroup module_socketwrapper
*/
//------------------------------------------------------------------------------
tOplkError socketwrapper_bind(tSocketWrapper pSocketWrapper_p,
                              const tSocketWrapperAddress* pSocketAddress_p)
{
    tOplkError              ret = kErrorOk;
    tSocketWrapInstance*    pInstance = (tSocketWrapInstance*)pSocketWrapper_p;

    UNUSED_PARAMETER(pSocketWrapper_p);

    if (pInstance == NULL)
        return kErrorSdoUdpInvalidHdl;

    if (!pInstance->fInitialized)
        return kErrorSdoUdpSocketError;

    pInstance->socketAddress = *pSocketAddress_p;

    if (pInstance->pIpStackHandle != NULL)
    {
        ret = updateIpStack(pInstance);
        if (ret != kErrorOk)
            return ret;
    }

    return ret;
}

//------------------------------------------------------------------------------
/**
\brief  Close socket wrapper instance

The function closes the given socket wrapper instance.

\param  pSocketWrapper_p    Socket wrapper instance.
\param  pSocketAddress_p    Socket address structure.

\ingroup module_socketwrapper
*/
//------------------------------------------------------------------------------
void socketwrapper_close(tSocketWrapper pSocketWrapper_p)
{
    tSocketWrapInstance*    pInstance = (tSocketWrapInstance*)pSocketWrapper_p;

    if (pInstance == NULL)
        return;

    pInstance->fInitialized = FALSE;
}

//------------------------------------------------------------------------------
/**
\brief  Send to socket wrapper instance

The function sends the given data to the remote address.

\param  pSocketWrapper_p    Socket wrapper instance.
\param  pRemote_p           Pointer to remote socketwrapper address.
\param  pData_p             Pointer to payload data.
\param  dataSize_p          Size of payload data.

\return The function returns a tOplkError error code.

\ingroup module_socketwrapper
*/
//------------------------------------------------------------------------------
tOplkError socketwrapper_send(tSocketWrapper pSocketWrapper_p,
                              const tSocketWrapperAddress* pRemote_p,
                              const UINT8* pData_p, UINT dataSize_p)
{
    tSocketWrapInstance*    pInstance = (tSocketWrapInstance*)pSocketWrapper_p;
    INT                     error;
    ip_udp_info             udpInfo;

    if (pInstance == NULL)
        return kErrorSdoUdpInvalidHdl;

    if (!pInstance->fInitialized)
        return kErrorSdoUdpSocketError;

    udpInfo.pData = pData_p;
    udpInfo.len = dataSize_p;
    udpInfo.localPort = instance_l.socketAddress.port;
    udpInfo.localHost.S_un.S_addr = instance_l.socketAddress.ipAddress;
    udpInfo.remotePort = htons(pRemote_p->port);
    udpInfo.remoteHost.S_un.S_addr = pRemote_p->ipAddress;

    error = ipUdpSend(pInstance->pIpStackHandle, &udpInfo);
    if (error != (INT)dataSize_p)
        return kErrorSdoUdpSendError;

    return kErrorOk;
}

//------------------------------------------------------------------------------
/**
\brief  Critical section

The function handles critical sections for the SDO/UDP module.

\param  fEnable_p           Specifies if a critical section shall be entered (TRUE)
                            or left (FALSE).

\ingroup module_socketwrapper
*/
//------------------------------------------------------------------------------
void socketwrapper_criticalSection(BOOL fEnable_p)
{
    (void)fEnable_p;
}

//------------------------------------------------------------------------------
/**
\brief  Set IP stack handle

The function sets the IP stack handle to the socket wrapper instance.

\param  pHandle_p           The IP stack handle

\return The function returns a tOplkError error code.

\ingroup module_socketwrapper
*/
//------------------------------------------------------------------------------
tOplkError socketwrapper_setIpStackHandle(IP_STACK_H pHandle_p)
{
    tSocketWrapInstance*    pInstance = (tSocketWrapInstance*)&instance_l;

    if (pInstance == NULL)
        return kErrorSdoUdpInvalidHdl;

    if (!pInstance->fInitialized)
        return kErrorApiNotInitialized;

    pInstance->pIpStackHandle = (IP_STACK_H)pHandle_p;

    return updateIpStack(pInstance);
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{

//------------------------------------------------------------------------------
/**
\brief  Socket receive callback

The function is called by the IP stack socket if a frame is received.

\param  pArg_p              Argument pointer holding the socket wrapper instance.
\param  pInfo_p             Pointer to UDP frame info.

*/
//------------------------------------------------------------------------------
static void receiveFromSocket(void* pArg_p, ip_udp_info* pInfo_p)
{
    tSocketWrapInstance*    pInstance = (tSocketWrapInstance*)pArg_p;
    tSocketWrapperAddress   remote;

    if (pInstance == NULL)
        return;

    if ((!pInstance->fInitialized) || (pInstance->pIpStackHandle == NULL))
        return;

    remote.ipAddress = pInfo_p->remoteHost.S_un.S_addr;
    remote.port = htons(pInfo_p->remotePort);

    pInstance->socketReceiveCb((UINT8*)pInfo_p->pData, pInfo_p->len, &remote);
}

//------------------------------------------------------------------------------
/**
\brief  Update IP stack

The function updates the IP stack with the socket wrapper settings (IP address).

\param  pInstance_p         Pointer to socket wrapper instance

\return The function returns a tOplkError error code.
*/
//------------------------------------------------------------------------------
static tOplkError updateIpStack(tSocketWrapInstance* pInstance_p)
{
    UINT32  ipAddr = htonl(pInstance_p->socketAddress.ipAddress);
    INT     error;

    ipChangeAddress(pInstance_p->pIpStackHandle, (struct in_addr*)&ipAddr);

    error = ipUdpListen(pInstance_p->pIpStackHandle, pInstance_p->socketAddress.port,
                        receiveFromSocket, pInstance_p);
    if (error < 0)
        return kErrorSdoUdpInvalidHdl;

    return kErrorOk;
}

//------------------------------------------------------------------------------
/**
\brief  The function handles the Arp Query

The function is called when a new SDO connection is initialized to handle the Arprequests
of the remote nodes.

\param  pSocketWrapper_p   Socket wrapper instance

\param  remoteIpAddress_p  Ip Address

\return The function returns a tOplkError error code.
*/
//------------------------------------------------------------------------------

tOplkError socketwrapper_arpQuery(tSocketWrapper pSocketWrapper_p,
                                       UINT32 remoteIpAddress_p)
{
    eth_addr       macAddr;
    eth_addr       macAddrZero;
    tOplkError     ret = kErrorOk;

    UNUSED_PARAMETER(pSocketWrapper_p);
    memset(&macAddrZero, 0, sizeof(eth_addr));
    memset(&macAddr, 0, sizeof(eth_addr));

    // if target node MAC is unknown return with error
    ipArpQuery(remoteIpAddress_p, &macAddr);
    if(!memcmp(&macAddr , &macAddrZero , sizeof(eth_addr)))
    {
         printf(" Inside socketwrapper_arpQuery\n");
        // send ARP request when MAC is unknown!
        ipArpRequest(remoteIpAddress_p, &macAddr);
        ret = kErrorSdoUdpArpInProgress;
    }
       return ret;

}

/// \}
