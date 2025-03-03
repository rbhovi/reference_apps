/*******************************************************************************
  Dynamic Host Configuration Protocol (DHCP) Server

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    - Provides automatic IP address, subnet mask, gateway address, 
      DNS server address, and other configuration parameters on DHCP 
      enabled networks.
    - Reference: RFC 2131, 2132
*******************************************************************************/

/*
Copyright (C) 2012-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/

#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_DHCP_SERVER

#include "tcpip/src/tcpip_private.h"
#include "tcpip/src/dhcps_private.h"

#if defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_DHCP_SERVER)

//ICMP Request and Reply global variables

TCPIP_DHCPS_ICMP_PROCESS_TYPE dhcpsEchoRequestState=TCPIP_DHCPS_ECHO_REQUEST_IDLE;
static uint16_t             dhcpsicmpSequenceNo;         // current sequence number
static uint16_t             dhcpsicmpIdentifier;         // current ID number

static int                  dhcpsicmpReqNo;              // number of requests to send
static int                  dhcpsicmpReqCount;           // current request counter
static int                  dhcpsicmpAckRecv;            // number of acks
static int                  dhcpsicmpReqDelay;
static DHCPS_ICMP_PROCESS   dhcpsicmpProcessSteps = DHCPS_ICMP_IDLE;
uint32_t                    dhcpsicmpStartTick;
static TCPIP_NET_HANDLE     dhcpsicmpNetH = 0;
static void TCPIP_DHCPS_EchoICMPRequestTask(void);
static void                 DHCPSPingHandler(const  TCPIP_ICMP_ECHO_REQUEST* pEchoReq, TCPIP_ICMP_REQUEST_HANDLE iHandle, TCPIP_ICMP_ECHO_REQUEST_RESULT result, const void* param);

static IPV4_ADDR            dhcpsicmpTargetAddr;         // current target address
static uint8_t              dhcpsicmpPingBuff[TCPIP_DHCPS_ICMP_ECHO_REQUEST_BUFF_SIZE];
static int                  dhcpsicmpPingSize = TCPIP_DHCPS_ICMP_ECHO_REQUEST_DATA_SIZE;
static TCPIP_ICMP_REQUEST_HANDLE dhcpsicmpReqHandle;     // current transaction handle
static DHCPS_RESULT _DHCPS_VerifyEchoRequestedAddress(void);
static DHCPS_RESULT _DHCPS_EchoRequestInit(TCPIP_NET_IF *pNetIf,IPV4_ADDR *requestedIpAddr);
//ICMP
uint32_t initTime ;

// DHCP server Magic bytes "DHCP"
#define TCPIP_DHCPS_MAGIC_COOKIE 0x63538263ul

#define TCPIP_DHCPS_MAX_RECEIVED_BUFFER_SIZE 480
static DHCPS_HASH_DCPT gPdhcpsHashDcpt = { 0,0};
static TCPIP_NET_IF  *gpDhcpsNetH = 0;
static DHCPS_MOD    dhcps_mod;
static BOOTP_HEADER	gBOOTPHeader;
// DHCP is running on all interfaces
static DHCP_SRVR_DCPT      *gPdhcpSDcpt=NULL;
static const void*          dhcpSMemH = 0;        // memory handle

static int                  dhcpSInitCount = 0;     // initialization count

static void _DHCPSUpdateEntry(DHCPS_HASH_ENTRY* dhcpsHE);
static bool _DHCPS_GetOptionLen(TCPIP_DHCPS_DATA *inputBuf,uint8_t *optionVal,uint8_t *optionLen);
static void DHCPReplyToDiscovery(TCPIP_NET_IF* pNetIf,BOOTP_HEADER *Header,DHCP_SRVR_DCPT * pDhcpsDcpt,DHCPS_HASH_DCPT *pdhcpsHashDcpt,TCPIP_DHCPS_DATA *getBuf);
static void DHCPReplyToRequest(TCPIP_NET_IF* pNetIf,BOOTP_HEADER *boot_header, bool bAccept, bool bRenew,DHCPS_HASH_DCPT *pdhcpsHashDcpt,TCPIP_DHCPS_DATA *getBuf,DHCP_SRVR_DCPT * pDhcpsDcpt);
static DHCPS_RESULT _DHCPS_FindValidAddressFromPool(BOOTP_HEADER *Header,DHCP_SRVR_DCPT * pDhcpsDcpt,DHCPS_HASH_DCPT *pdhcpsHashDcpt,IPV4_ADDR *reqIPAddress);
#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void _DHCPServerCleanup(void);
#else
#define _DHCPServerCleanup()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)
static DHCPS_RESULT preAssignToDHCPClient(TCPIP_NET_IF* pNetIf,BOOTP_HEADER *Header,DHCP_SRVR_DCPT * pDhcpsDcpt,DHCPS_HASH_DCPT *pdhcpsHashDcpt);
static bool isMacAddrEffective(const TCPIP_MAC_ADDR *macAddr);
static void DHCPSReplyToInform(TCPIP_NET_IF* pNetIf,BOOTP_HEADER *boot_header, DHCP_SRVR_DCPT* pDhcpsDcpt,DHCPS_HASH_DCPT *pdhcpsHashDcpt,bool bAccept,TCPIP_DHCPS_DATA *getBuf);
static void _DHCPSrvClose(TCPIP_NET_IF* pNetIf, bool disable);
static size_t DHCPSgetFreeHashIndex(OA_HASH_DCPT* pOH,void* key,IPV4_ADDR *requestedAddr,IPV4_ADDR *ipAddrExists);
static DHCPS_RESULT DHCPSLocateRequestedIpAddress(IPV4_ADDR *requestedAddr);
static  DHCPS_RESULT DHCPSRemoveHashEntry(TCPIP_MAC_ADDR* hwAdd, const uint8_t* pIPAddr);
static int TCPIP_DHCPS_CopyDataArrayToProcessBuff(uint8_t *val ,TCPIP_DHCPS_DATA *putbuf,int len);
static void TCPIP_DHCPS_DataCopyToProcessBuffer(uint8_t val ,TCPIP_DHCPS_DATA *putbuf);
static bool _DHCPSAddCompleteEntry(int intfIdx, const uint8_t* pIPAddr, TCPIP_MAC_ADDR* hwAdd, DHCPS_ENTRY_FLAGS entryFlag);
static bool _DHCPSDescriptorGetFromIntf(TCPIP_NET_IF *pNetIf,uint32_t *dcptIdx);
static bool _DCHPS_FindRequestIPAddress(TCPIP_DHCPS_DATA *inputBuf,uint8_t *reqAddr);
static bool _DHCPS_ProcessGetPktandSendResponse(void);
static bool _DHCPS_Enable(TCPIP_NET_HANDLE hNet,bool checkIfUp);
static bool _DHCPS_StartOperation(TCPIP_NET_IF* pNetIf,DHCP_SRVR_DCPT* pDhcpsDcpt);

static void TCPIP_DHCPS_TaskForLeaseTime(void);
static void TCPIP_DHCPS_Process(void);
static void TCPIP_DHCPSSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param);

#if ((TCPIP_DHCPS_DEBUG_LEVEL & TCPIP_DHCPS_DEBUG_MASK_BASIC) != 0)
// a Basic debug Print and Message

static void _DHCPSDbgCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("DHCP Server Cond: %s, in line: %d, \r\n", message, lineNo);
    }
}

#else
#define _DHCPSDbgCond(cond, message, lineNo)
#endif  // (TCPIP_DHCPS_DEBUG_LEVEL & TCPIP_DHCPS_DEBUG_MASK_BASIC)

#if ((TCPIP_DHCPS_DEBUG_LEVEL & TCPIP_DHCPS_DEBUG_MASK_ICMP) != 0)
static void _DhcpsICMPDebugPrint(uint16_t requestCount,char* icmpMessage)
{
    if(icmpMessage == NULL)
    {
        SYS_CONSOLE_PRINT("ICMP : Request count:: %d \r\n", requestCount);
    }
    else
    {
        SYS_CONSOLE_PRINT("ICMP : %s :: %d \r\n", icmpMessage,requestCount);
    }
}
static void _DhcpsICMPDebugMessage(char* icmpMessage)
{  
    SYS_CONSOLE_PRINT("ICMP : %s ", icmpMessage);
}
#else
#define _DhcpsICMPDebugPrint(requestCount,icmpMessage)
#define _DhcpsICMPDebugMessage(icmpMessage)
#endif  // (TCPIP_DHCPS_DEBUG_LEVEL & TCPIP_DHCPS_DEBUG_MASK_ICMP)

/*static __inline__*/static  void /*__attribute__((always_inline))*/ _DHCPSSetHashEntry(DHCPS_HASH_ENTRY* dhcpsHE, DHCPS_ENTRY_FLAGS newFlags, TCPIP_MAC_ADDR* hwAdd, const uint8_t* pIPAddr)
{
    dhcpsHE->hEntry.flags.value &= ~DHCPS_FLAG_ENTRY_VALID_MASK;
    dhcpsHE->hEntry.flags.value |= newFlags;
    
    if(hwAdd)
    {
        dhcpsHE->hwAdd = *hwAdd;		
        memcpy(dhcpsHE->ipAddress.v, pIPAddr, sizeof(dhcpsHE->ipAddress));
    }
    
    dhcpsHE->Client_Lease_Time = SYS_TMR_TickCountGet();
    dhcpsHE->pendingTime = SYS_TMR_TickCountGet();
}

/*static __inline__*/static  void /*__attribute__((always_inline))*/ _DHCPSRemoveCacheEntries(DHCPS_HASH_DCPT* pDHCPSHashDcpt)
{
    if(pDHCPSHashDcpt->hashDcpt)
    {
        TCPIP_OAHASH_EntriesRemoveAll(pDHCPSHashDcpt->hashDcpt);
    }
}

static __inline__ void __attribute__((always_inline)) _DHCPSStateSet(TCPIP_DHCPS_STATE_STATUS newState)
{
    dhcps_mod.smState = newState;
}

static  DHCPS_RESULT DHCPSRemoveHashEntry(TCPIP_MAC_ADDR* hwAdd, const uint8_t* pIPAddr)
{
    DHCPS_HASH_DCPT *pDhcpsHashDcpt;
    OA_HASH_ENTRY*	hE;

    pDhcpsHashDcpt = &gPdhcpsHashDcpt;
    if(hwAdd)
    {
    	hE = TCPIP_OAHASH_EntryLookup(pDhcpsHashDcpt->hashDcpt,hwAdd);
        if(hE != 0)
        {
            if(TCPIP_DHCPS_HashIPKeyCompare(pDhcpsHashDcpt->hashDcpt, hE, pIPAddr)== 0)
            {
                TCPIP_OAHASH_EntryRemove(pDhcpsHashDcpt->hashDcpt, hE);
                return DHCPS_RES_OK;
            }
        }
    }

    return DHCPS_RES_NO_ENTRY;
    
}

static void _DHCPSUpdateEntry(DHCPS_HASH_ENTRY* dhcpsHE)
{    
     dhcpsHE->Client_Lease_Time = SYS_TMR_TickCountGet();
     dhcpsHE->pendingTime = 0;

     dhcpsHE->hEntry.flags.value &= ~DHCPS_FLAG_ENTRY_VALID_MASK;
     dhcpsHE->hEntry.flags.value |= DHCPS_FLAG_ENTRY_COMPLETE;
}

// validate the IP address pool from the DHCP server configuration and poolCnt returns the valid pool numbers
static void dhcpServPoolAddressValidation(const TCPIP_DHCPS_MODULE_CONFIG* pDhcpsConfig,int nIfx,int *poolCnt)
{
    int ix=0;
    int tempPoolCnt=0;    
    const char *tempStr = {"0.0.0.0"};
    IPV4_ADDR	serverIPAddress;
    IPV4_ADDR	startIPv4PoolAddress;
    IPV4_ADDR   netMask;
    TCPIP_DHCPS_ADDRESS_CONFIG *pServer;

    *poolCnt =tempPoolCnt;
    
    for(ix=0;ix<pDhcpsConfig->dhcpServerCnt;ix++)
    {
        if(pDhcpsConfig->dhcpServer == NULL) 
        {
            break;
        }
        
        pServer = pDhcpsConfig->dhcpServer+ix;
        if(pServer == NULL)
        {
            continue;
        }
        pDhcpsConfig->dhcpServer[ix].poolEnabled = false;
        // if the interface index of dhcp configuration is greater than the 
        //number of interface supported, then that DHCP  lease address pool configuration 
        // should not be considered

        if(pDhcpsConfig->dhcpServer[ix].interfaceIndex >= nIfx)
        {
            continue;
        }
        if((pDhcpsConfig->dhcpServer[ix].serverIPAddress == NULL)||
            (pDhcpsConfig->dhcpServer[ix].startIPAddRange == NULL) ||
              (pDhcpsConfig->dhcpServer[ix].ipMaskAddress == NULL) )
        {
            continue;
        }

         // if the server address or netmask or ip address range is zero, 
        //then that pool should not be considered
        if((strcmp(pDhcpsConfig->dhcpServer[ix].serverIPAddress,tempStr)==0)||
           (strcmp(pDhcpsConfig->dhcpServer[ix].startIPAddRange,tempStr)==0) ||
           (strcmp(pDhcpsConfig->dhcpServer[ix].ipMaskAddress,tempStr)==0))
        {
            continue;
        }

        // if start IP address range and serverIp address are not in the same 
        // subnet , then don't consider that pool
//Server Ip address
        TCPIP_Helper_StringToIPAddress((char*)pDhcpsConfig->dhcpServer[ix].serverIPAddress,&serverIPAddress);
//start IP address range
        TCPIP_Helper_StringToIPAddress((char*)pDhcpsConfig->dhcpServer[ix].startIPAddRange,&startIPv4PoolAddress);
//NetMask
        TCPIP_Helper_StringToIPAddress((char*)pDhcpsConfig->dhcpServer[ix].ipMaskAddress,&netMask);        
        if((serverIPAddress.Val & netMask.Val) != (startIPv4PoolAddress.Val & netMask.Val))
        {
            continue;
        }
        if(startIPv4PoolAddress.Val>serverIPAddress.Val)
        {
            tempPoolCnt++;
            pDhcpsConfig->dhcpServer[ix].poolEnabled = true;
        }
    }
    // if there is no pool, then pool count make it as 1 and use the default values for the server configuration.
    if(tempPoolCnt==0)  
    {
        _DHCPSDbgCond(false, " Debug Message: No pool and Default Server configuration is use. This condition should not come.", __LINE__);
        tempPoolCnt=1;
    }
    *poolCnt = tempPoolCnt;
}

// DHCP server descriptor update has been done at the init  time only.
static void _DHCPS_AddressPoolDescConfiguration(const TCPIP_DHCPS_MODULE_CONFIG* pDhcpsConfig)
{
    int poolCount=0;
    TCPIP_DHCPS_ADDRESS_CONFIG *pPoolServer=NULL;
    DHCP_SRVR_DCPT *pServerDcpt=NULL;
    int ix=0,localPoolIndex=0;


    poolCount = dhcps_mod.poolCount;    
    if(gPdhcpSDcpt == NULL)
    {
        return;
    }
    
    for(ix=0;ix<poolCount;ix++)
    {
        if(pDhcpsConfig->dhcpServer == NULL)
        {
            break;
        }
        
        pPoolServer = pDhcpsConfig->dhcpServer+ix;
        if(pPoolServer == NULL)
        {
            continue;
        }
        if(pPoolServer->poolEnabled == false)
        {
            continue;
        }

        pServerDcpt = gPdhcpSDcpt+ix;
        if(pServerDcpt == NULL)
        {
            continue;
        }
// Server IPv4 address
        TCPIP_Helper_StringToIPAddress((char*)pPoolServer->serverIPAddress,&pServerDcpt->intfAddrsConf.serverIPAddress);
    //NET Mask
        TCPIP_Helper_StringToIPAddress((char*)pPoolServer->ipMaskAddress,&pServerDcpt->intfAddrsConf.serverMask);
// Server start of IPv4 address pool
        TCPIP_Helper_StringToIPAddress((char*)pPoolServer->startIPAddRange,&pServerDcpt->intfAddrsConf.startIPAddress);
#if defined(TCPIP_STACK_USE_DNS)
    // primary DNS server
        TCPIP_Helper_StringToIPAddress((char*)pPoolServer->priDNS,&pServerDcpt->intfAddrsConf.serverDNS);
    // Secondary DNS server
        TCPIP_Helper_StringToIPAddress((char*)pPoolServer->secondDNS,&pServerDcpt->intfAddrsConf.serverDNS2);
#endif
        pServerDcpt->intfAddrsConf.poolIndex = localPoolIndex;
        pServerDcpt->netIx = pPoolServer->interfaceIndex;

        localPoolIndex++;
       
    }
    
    // Get the default pool configuration
    if(localPoolIndex == 0)
    {
        pServerDcpt = gPdhcpSDcpt+localPoolIndex;
        memset(&pServerDcpt->intfAddrsConf,0,sizeof(pServerDcpt->intfAddrsConf));
        pServerDcpt->netIx = TCPIP_STACK_NetIndexGet(TCPIP_STACK_NetDefaultGet());
    }
}

bool TCPIP_DHCPS_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_DHCPS_MODULE_CONFIG* pDhcpsConfig)
{    
    size_t hashMemSize=0;
    OA_HASH_DCPT*   hashDcpt;
    int poolCnt=0;

    if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        if(stackCtrl->pNetIf->Flags.bIsDHCPSrvEnabled != 0)
        {
            TCPIP_DHCPS_Enable(stackCtrl->pNetIf);
        }
        dhcpSMemH = stackCtrl->memH;
        return true;
    }
	
    
    // stack init
    if(dhcpSInitCount == 0)
    {
        if(pDhcpsConfig == 0)
        {
            return false;
        }
        // first time we're run
        // store the memory allocation handle
        dhcpSMemH = stackCtrl->memH;
        dhcpServPoolAddressValidation(pDhcpsConfig,stackCtrl->nIfs,&poolCnt);
        if(poolCnt > 0)
        {
            gPdhcpSDcpt = (DHCP_SRVR_DCPT*)TCPIP_HEAP_Calloc(dhcpSMemH, poolCnt, sizeof(DHCP_SRVR_DCPT));
            if(gPdhcpSDcpt == 0)
            {   // failed
                return false;
            }
        }
        else
        {
            SYS_ERROR(SYS_ERROR_WARNING, "DHCPS: Initialization failed! \r\n");
            return false;
        }

        hashMemSize = sizeof(OA_HASH_DCPT) + pDhcpsConfig->leaseEntries * sizeof(DHCPS_HASH_ENTRY);

        hashDcpt = (OA_HASH_DCPT*)TCPIP_HEAP_Malloc(dhcpSMemH, hashMemSize);

        if(hashDcpt == 0)
        {	// failed
        // Remove all the DHCPS lease entries.
            // Free DHCP server Pool entries
            TCPIP_HEAP_Free(dhcpSMemH,gPdhcpSDcpt);
            return false;
        }
        // populate the entries
        hashDcpt->memBlk = hashDcpt+1;
        hashDcpt->hParam = &gPdhcpsHashDcpt;
        hashDcpt->hEntrySize = sizeof(DHCPS_HASH_ENTRY);
        hashDcpt->hEntries = pDhcpsConfig->leaseEntries;
        hashDcpt->probeStep = DHCPS_HASH_PROBE_STEP;

        hashDcpt->hashF= TCPIP_DHCPS_MACHashKeyHash;
#if defined(OA_DOUBLE_HASH_PROBING)
        hashDcpt->probeHash = TCPIP_DHCPS_HashProbeHash;
#endif  // defined(OA_DOUBLE_HASH_PROBING)
        hashDcpt->cpyF = TCPIP_DHCPS_HashMACKeyCopy;
        hashDcpt->delF = TCPIP_DHCPS_HashDeleteEntry;
        hashDcpt->cmpF = TCPIP_DHCPS_HashMACKeyCompare;
        TCPIP_OAHASH_Initialize(hashDcpt);
        gPdhcpsHashDcpt.hashDcpt = hashDcpt;
        gPdhcpsHashDcpt.leaseDuartion = pDhcpsConfig->entrySolvedTmo;

        if(pDhcpsConfig->deleteOldLease)
        {	// remove the old entries, if there
            _DHCPSRemoveCacheEntries(&gPdhcpsHashDcpt);
        }
		
        dhcps_mod.signalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_DHCPS_Task, TCPIP_DHCPS_TASK_PROCESS_RATE);
        if(dhcps_mod.signalHandle == 0)
        {
            _DHCPServerCleanup();
            return false;
        }
        dhcps_mod.smServer = DHCP_SERVER_IDLE;
        dhcps_mod.uSkt = INVALID_UDP_SOCKET;
        dhcps_mod.poolCount = poolCnt;
        dhcps_mod.dhcpNextLease.Val = 0;
        dhcps_mod.smState = TCPIP_DHCPS_STATE_IDLE;
        memset(&gBOOTPHeader,0,sizeof(gBOOTPHeader));

        // expected that max number of pool entry is similar to the interface index
        // copy the valid interface details to the global dhcps descriptor table
       _DHCPS_AddressPoolDescConfiguration(pDhcpsConfig);
    }
	
    if(stackCtrl->pNetIf->Flags.bIsDHCPSrvEnabled != 0)
    {   // override the pDhcpsConfig->dhcpEnable passed with the what the stack manager says
        _DHCPS_Enable(stackCtrl->pNetIf,false);
    }
       
    // Reset state machine and flags to default values

    dhcpSInitCount++;

    return true;
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_DHCPS_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    if(dhcpSInitCount > 0)
    {   // we're up and running
        // this interface is going down no matter what
        _DHCPSrvClose(stackCtrl->pNetIf,true);
        if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--dhcpSInitCount == 0)
            {   // all closed
                // release resources
                _DHCPServerCleanup();
                dhcpSMemH = 0;
            }
        }
    }
}

static void _DHCPServerCleanup(void)
{
    // Free HASH descriptor 
    if(gPdhcpsHashDcpt.hashDcpt != NULL)
    {
        // Remove all the HASH entries
        _DHCPSRemoveCacheEntries(&gPdhcpsHashDcpt);
        TCPIP_HEAP_Free(dhcpSMemH,gPdhcpsHashDcpt.hashDcpt);
        gPdhcpsHashDcpt.hashDcpt = 0;
    }
    // Free Pool entry DHCP server descriptor
    if(gPdhcpSDcpt != NULL)
    {
        TCPIP_HEAP_Free(dhcpSMemH,gPdhcpSDcpt);
    }
    // Free timer handler
    if(dhcps_mod.signalHandle)
    {
        _TCPIPStackSignalHandlerDeregister(dhcps_mod.signalHandle);
        dhcps_mod.signalHandle = 0;
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static void _DHCPSrvClose(TCPIP_NET_IF* pNetIf, bool disable)
{
    int netIx=0;
    TCPIP_NET_IF* pIf=NULL;
    bool serverIsEnabled=false;
    if(disable)
    {
        pNetIf->Flags.bIsDHCPSrvEnabled = false;
    }
    for(netIx = 0;netIx < TCPIP_STACK_NumberOfNetworksGet(); netIx++)
    {
        pIf = (TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(netIx);
        if(pIf->Flags.bIsDHCPSrvEnabled)
        {
            serverIsEnabled=true;
            break;
        }
    }
    // it means that  there is no server is enabled and so close the DHCP server socket
    if(serverIsEnabled == false)
    {
        if( dhcps_mod.uSkt != INVALID_UDP_SOCKET)
        {
            TCPIP_UDP_Close(dhcps_mod.uSkt);
            dhcps_mod.uSkt = INVALID_UDP_SOCKET;
            dhcps_mod.smServer = DHCP_SERVER_IDLE;
            _DHCPSStateSet(TCPIP_DHCPS_STATE_IDLE);
        }
    }
    else
    {
        dhcps_mod.smServer = DHCP_SERVER_LISTEN;
    }
    
}

bool _DHCPS_ValidatePktReceivedIntf(TCPIP_NET_IF *pNetIfFromDcpt)
{
    if(_TCPIPStackHandleToNetLinked(pNetIfFromDcpt) == 0)
    {
        return false;
    }
    if(_TCPIPStackNetAddress(pNetIfFromDcpt) == 0)
    {
        return false;
    }
#if defined(TCPIP_STACK_USE_DHCP_CLIENT)
    // Make sure we don't clobber anyone else's DHCP server
    if(TCPIP_DHCP_IsServerDetected(pNetIfFromDcpt)|| TCPIP_DHCP_IsEnabled(pNetIfFromDcpt))
    {
        return false;
    }
#endif
    if(!TCPIP_DHCPS_IsEnabled(pNetIfFromDcpt))
    {
        return false;
    }
    return true;
}

static bool TCPIP_DHCPS_GetDataFromUDPBuff(TCPIP_DHCPS_DATA *getbuf,uint8_t *val)
{
    int nBytes = getbuf->endPtr - getbuf->wrPtr;
    if(nBytes == 0)
    {
        return false;
    }
    *val = getbuf->wrPtr[0];
    getbuf->wrPtr = getbuf->wrPtr+1;    
    return true;
}

static int TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(TCPIP_DHCPS_DATA *getbuf,uint8_t *buf,int bytes)
{
    int nBytes = getbuf->endPtr - getbuf->wrPtr;
    if(nBytes == 0)
        return 0;

    if(bytes < nBytes)
    {
        nBytes =  bytes;
    }
    if(buf == NULL)
    {
        getbuf->wrPtr = getbuf->wrPtr+nBytes;
        return nBytes;
    }
    memcpy(buf,getbuf->wrPtr,nBytes);
    getbuf->wrPtr = getbuf->wrPtr+nBytes;
    return nBytes;
}

void TCPIP_DHCPS_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;
    DHCPS_RESULT res;
    
    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(dhcps_mod.uSkt == INVALID_UDP_SOCKET) 
    {
        return;
    }

    if((sigPend & TCPIP_MODULE_SIGNAL_RX_PENDING) != 0)
    { //  RX signal occurred
        TCPIP_DHCPS_Process();
    }
    
    if(dhcps_mod.smServer == DHCP_SERVER_ICMP_PROCESS)
    {
        // run ICMP ECHO request task
        TCPIP_DHCPS_EchoICMPRequestTask();
        res = _DHCPS_VerifyEchoRequestedAddress();
        if(res == DHCPS_RES_ECHO_IN_PROCESS)
        {
            return;
        }
        else if(res == DHCPS_RES_ENTRY_EXIST)
        {
            //update global variable that can be used for @HS
            dhcps_mod.smState = TCPIP_DHCPS_GET_NEW_ADDRESS;
            dhcps_mod.smServer = DHCP_SERVER_LISTEN;
            return;
        }
        else if(res == DHCPS_RES_NO_ENTRY)
        {
            //update global variable that can be used for @HS
            dhcps_mod.smState = TCPIP_DHCPS_SEND_OFFER;
            dhcps_mod.smServer = DHCP_SERVER_LISTEN;
        }
    }
        
    if((sigPend & TCPIP_MODULE_SIGNAL_TMO) != 0)
    { // regular TMO occurred
        TCPIP_DHCPS_TaskForLeaseTime();
    }

}

// send a signal to the DHCPS module that data is available
// no manager alert needed since this normally results as a higher layer (UDP) signal
static void TCPIP_DHCPSSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_UDP_SIGNAL_RX_DATA)
    {
        _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}


static void TCPIP_DHCPS_Process(void)
{
    _DHCPS_ProcessGetPktandSendResponse();   
}

static bool _DHCPS_ProcessGetPktandSendResponse(void)
{
    uint32_t            buffsize=0;
    uint8_t 		i;
    uint8_t		Option, Len;    
    uint32_t		dw;
    bool		bAccept=false, bRenew=false;
    UDP_SOCKET          s;
    OA_HASH_ENTRY   	*hE;
    DHCPS_HASH_DCPT  	*pdhcpsHashDcpt;
    DHCP_SRVR_DCPT	*pDhcpsDcpt=NULL;
    UDP_SOCKET_INFO     udpSockInfo;
    TCPIP_NET_IF        *pNetIfFromDcpt=NULL;
    uint32_t            ix=0;
    uint8_t             getBuffer[TCPIP_DHCPS_MAX_RECEIVED_BUFFER_SIZE];
    TCPIP_DHCPS_DATA    udpGetBufferData;
    IPV4_ADDR           ClientIP;
    TCPIP_DHCPS_STATE_STATUS dhcpsSmSate;
    BOOTP_HEADER	   BOOTPHeader;
    
    s = dhcps_mod.uSkt;
    dhcpsSmSate = dhcps_mod.smState;
    if(gPdhcpSDcpt == NULL)
    {
        return false;
    }
      
    pdhcpsHashDcpt = &gPdhcpsHashDcpt;

    while(true)
    {
        switch(dhcpsSmSate)
        {
            case TCPIP_DHCPS_STATE_IDLE:
                break;
            case TCPIP_DHCPS_START_RECV_NEW_PACKET:
                
            // Check to see if a valid DHCP packet has arrived
                buffsize = TCPIP_UDP_GetIsReady(s);
                if(buffsize == 0) 
                {
                    return false;
                }
                if(buffsize < TCPIP_DHCPS_MIN_DISCOVERY_PKT_SIZE)
                {
                    TCPIP_UDP_Discard(s);
                    continue;
                }
                if(dhcps_mod.smServer == DHCP_SERVER_ICMP_PROCESS)
                {
                    TCPIP_UDP_Discard(s);
                    continue;
                }
                memset(getBuffer,0,sizeof(getBuffer));
                dhcpsSmSate = TCPIP_DHCPS_DETECT_VALID_INTF;
                // break free
                
            case TCPIP_DHCPS_DETECT_VALID_INTF:

                TCPIP_UDP_SocketInfoGet(s, &udpSockInfo);
                pNetIfFromDcpt = (TCPIP_NET_IF*)udpSockInfo.hNet;
                // check if DHCP server is enabled or Not for this incoming packet interface
                if(!_DHCPS_ValidatePktReceivedIntf(pNetIfFromDcpt))
                {
                    TCPIP_UDP_Discard(s);
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    continue;
                }
                gpDhcpsNetH = pNetIfFromDcpt;
                dhcpsSmSate = TCPIP_DHCPS_PARSE_RECVED_PACKET;
                //break free
                
            case TCPIP_DHCPS_PARSE_RECVED_PACKET:
                if(buffsize > sizeof(getBuffer))
                {
                    buffsize = sizeof(getBuffer);
                }
                udpGetBufferData.head = getBuffer;
                udpGetBufferData.wrPtr = udpGetBufferData.head;
                udpGetBufferData.endPtr = udpGetBufferData.head+buffsize;
                // Get Complete Array of DHCP server Reply bytes
                TCPIP_UDP_ArrayGet(s,udpGetBufferData.head,buffsize);
               // discard the current packet and point to the next queued packet
                TCPIP_UDP_Discard(s);

                // Retrieve the BOOTP header
                if(!TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(&udpGetBufferData, (uint8_t*)&BOOTPHeader, sizeof(BOOTPHeader)))
                {
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    continue;
                }
                
                //copy the local BOOTP_Header to gBOOTPHeader 
                memcpy(&gBOOTPHeader,&BOOTPHeader,sizeof(BOOTPHeader));
                hE = TCPIP_OAHASH_EntryLookup(pdhcpsHashDcpt->hashDcpt, &gBOOTPHeader.ClientMAC);
                if(hE != 0)
                {
                    ClientIP = gBOOTPHeader.ClientIP;
                    if(TCPIP_DHCPS_HashIPKeyCompare(pdhcpsHashDcpt->hashDcpt, hE, ClientIP.v) == 0)
                    {
                        bRenew= true;
                        bAccept = true;
                    }
                    else
                    {
                        bRenew = false;
                        bAccept = false;
                    }
                }
                else if(gBOOTPHeader.ClientIP.Val == 0x00000000u)
                {
                    bRenew = false;
                    bAccept = true;
                }
                else
                {
                    bRenew = false;
                    bAccept = false;
                }

                // Validate first three fields
                if((gBOOTPHeader.MessageType != 1u) || (gBOOTPHeader.HardwareType != 1u) || (gBOOTPHeader.HardwareLen != 6u))
                {
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    continue;
                }

                // Throw away 10 unused bytes of hardware address,
                // server host name, and boot file name -- unsupported/not needed.
                if(!TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(&udpGetBufferData,0,DHCPS_UNUSED_BYTES_FOR_TX))
                {
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    continue;
                }
      
                // Obtain Magic Cookie and verify DHCP
                if(!TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(&udpGetBufferData,(uint8_t*)&dw, sizeof(uint32_t)))
                {
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    continue;
                }

                if(dw != TCPIP_DHCPS_MAGIC_COOKIE)
                {
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    continue;
                }
                dhcpsSmSate = TCPIP_DHCPS_FIND_DESCRIPTOR;
                //break Free
             case TCPIP_DHCPS_FIND_DESCRIPTOR:
                ix=0;
                // get the Descriptor index from the interface
                if(_DHCPSDescriptorGetFromIntf(pNetIfFromDcpt,&ix) == false)
                {
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    // the interface is not configured for DHCP Server
                    return false;
                }
                // assign the dhcpDescriptor value
                pDhcpsDcpt = gPdhcpSDcpt+ix;
                dhcpsSmSate = TCPIP_DHCPS_MESSAGE_TYPE;
                // break free
            case   TCPIP_DHCPS_MESSAGE_TYPE:  
                // Obtain options //while(true)
                if(_DHCPS_GetOptionLen(&udpGetBufferData,&Option,&Len)!= true)
                {
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    continue;
                }
                switch(Option)
                {
                    case DHCP_MESSAGE_TYPE:
                    {
                        if(!TCPIP_DHCPS_GetDataFromUDPBuff(&udpGetBufferData,&i))
                        {
                            dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                            continue;
                        }
                        if(i == DHCP_DISCOVER_MESSAGE)
                        {
                            if(_DCHPS_FindRequestIPAddress(&udpGetBufferData,ClientIP.v)!= true)
                            {
                                dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                                continue;
                            }
                            else
                            { // 
                                 /* Validating if the requested IP address should not be part of any Hash Entry */
                                if(DHCPSLocateRequestedIpAddress((IPV4_ADDR *)&ClientIP) != DHCPS_RES_OK)
                                { // use the alternate address
                                    dhcps_mod.dhcpNextLease.Val = 0;
                                }
                                else
                                {// address is present
                                    dhcps_mod.dhcpNextLease.Val = ClientIP.Val;
                                }
                                // Find the new address
                                if(_DHCPS_FindValidAddressFromPool(&gBOOTPHeader,pDhcpsDcpt,pdhcpsHashDcpt,(IPV4_ADDR*)NULL) != DHCPS_RES_OK)
                                {
                                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                                    continue;
                                }
                                if(dhcps_mod.dhcpNextLease.Val != 0)
                                {
                                    dhcpsSmSate = TCPIP_DHCPS_START_ICMP_PROCESS;
                                }
                                else
                                {
                                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                                    continue;
                                }
                            }                        
                        }
                        else if(i==DHCP_REQUEST_MESSAGE)
                        {
                            DHCPReplyToRequest(pNetIfFromDcpt,&gBOOTPHeader, bAccept,bRenew,pdhcpsHashDcpt,&udpGetBufferData,pDhcpsDcpt);
                            dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                            memset(&gBOOTPHeader,0,sizeof(gBOOTPHeader));
                            break;
                        }
                         // Need to handle these if supporting more than one DHCP lease
                        else if((i== DHCP_RELEASE_MESSAGE) ||(i==DHCP_DECLINE_MESSAGE))
                        {
                            ClientIP = gBOOTPHeader.ClientIP;
                            DHCPSRemoveHashEntry(&gBOOTPHeader.ClientMAC, ClientIP.v);
                            dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                            memset(&gBOOTPHeader,0,sizeof(gBOOTPHeader));
                            break;
                        }
                        else if(i==DHCP_INFORM_MESSAGE)
                        {
                            DHCPSReplyToInform(pNetIfFromDcpt,&gBOOTPHeader,pDhcpsDcpt,pdhcpsHashDcpt,bAccept,&udpGetBufferData);
                            dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                            memset(&gBOOTPHeader,0,sizeof(gBOOTPHeader));
                            break;
                        }
                    }
                    case DHCP_END_OPTION:
                        break;                        
                }
               
                // Remove any unprocessed bytes that we don't care about
                if(!TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(&udpGetBufferData,0,Len))
                {
                    break;
                }
                Len = 0;
                _DHCPSStateSet(dhcpsSmSate);
                break;
            case   TCPIP_DHCPS_START_ICMP_PROCESS:
                if(_DHCPS_EchoRequestInit((TCPIP_NET_IF *)gpDhcpsNetH,(IPV4_ADDR *)&dhcps_mod.dhcpNextLease) == DHCPS_RES_ECHO_IN_PROCESS)
                {
                    dhcps_mod.smServer = DHCP_SERVER_ICMP_PROCESS;
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                   _DHCPSStateSet(dhcpsSmSate);
                    return true;
                }                
            case TCPIP_DHCPS_GET_NEW_ADDRESS:
                 // Find the new address // this is the address search with previous address value which was failed 
                // during ICMP processing
                ix=0;
                // get the Descriptor index from the interface
                if(_DHCPSDescriptorGetFromIntf(gpDhcpsNetH,&ix) == false)
                {
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    _DHCPSStateSet(dhcpsSmSate);
                    // the interface is not configured for DHCP Server
                    return false;
                }

                // assign the dhcpDescriptor value
                pDhcpsDcpt = gPdhcpSDcpt+ix;
                if(_DHCPS_FindValidAddressFromPool(&gBOOTPHeader,pDhcpsDcpt,pdhcpsHashDcpt,(IPV4_ADDR*)&dhcps_mod.dhcpNextLease) != DHCPS_RES_OK)
                {
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    _DHCPSStateSet(dhcpsSmSate);
                    continue;
                }
                if(dhcps_mod.dhcpNextLease.Val != 0)
                {
                    dhcpsSmSate = TCPIP_DHCPS_START_ICMP_PROCESS;
                    _DHCPSStateSet(dhcpsSmSate);
                }
                else
                {
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    _DHCPSStateSet(dhcpsSmSate);
                    continue;
                }
                break;
            case TCPIP_DHCPS_SEND_OFFER:
                 ix=0;
                // get the Descriptor index from the interface
                if(_DHCPSDescriptorGetFromIntf(gpDhcpsNetH,&ix) == false)
                {
                    dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                    _DHCPSStateSet(dhcpsSmSate);
                    // the interface is not configured for DHCP Server
                    return false;
                }
                // assign the dhcpDescriptor value
                pDhcpsDcpt = gPdhcpSDcpt+ix;
                DHCPReplyToDiscovery((TCPIP_NET_IF*)gpDhcpsNetH,&gBOOTPHeader,pDhcpsDcpt,pdhcpsHashDcpt,&udpGetBufferData);
                dhcpsSmSate = TCPIP_DHCPS_START_RECV_NEW_PACKET;
                _DHCPSStateSet(dhcpsSmSate);
                break;
        }
    }    
}

static void TCPIP_DHCPS_DataCopyToProcessBuffer(uint8_t val ,TCPIP_DHCPS_DATA *putBuf)
{
    if(putBuf->wrPtr < putBuf->endPtr)
    {
        *putBuf->wrPtr++ = val;
    }
}

static int TCPIP_DHCPS_CopyDataArrayToProcessBuff(uint8_t *val ,TCPIP_DHCPS_DATA *putbuf,int len)
{
    int nBytes = putbuf->endPtr - putbuf->wrPtr;
    if(len < nBytes)
    {
        nBytes = len;
    }
    if(nBytes <= 0)
    {
        return 0;
    }
    if(val == NULL)
    {
        putbuf->wrPtr = putbuf->wrPtr+nBytes;
        return 0;
    }
    memcpy(putbuf->wrPtr, val,nBytes);
    putbuf->wrPtr += nBytes;
    return nBytes;
}

static bool _DHCPS_GetOptionLen(TCPIP_DHCPS_DATA *inputBuf,uint8_t *optionVal,uint8_t *optionLen)
{
     // Get option type
    if(!TCPIP_DHCPS_GetDataFromUDPBuff(inputBuf,(uint8_t *)optionVal))
    {
        return false;
    }
    if(*optionVal == DHCP_END_OPTION)
    {
        return false;
    }

    // Get option length
    if(!TCPIP_DHCPS_GetDataFromUDPBuff(inputBuf,(uint8_t *)optionLen))
    {
        return false;
    }
    
    return true;
}

static bool _DCHPS_FindRequestIPAddress(TCPIP_DHCPS_DATA *inputBuf,uint8_t *reqAddr)
{
    
    while(inputBuf->endPtr-inputBuf->wrPtr)
    {
        uint8_t Option, Len;

        if(_DHCPS_GetOptionLen(inputBuf,&Option,&Len) != true)
        {
            if(Option != 0xFF)
            {
                return false;
            }
        }

        switch(Option)
        {
            case DHCP_PARAM_REQUEST_IP_ADDRESS:
            {
                if(!TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(inputBuf,(uint8_t*)reqAddr,4))
                {
                    break;
                }
                if(Len != 4)
                {
                    break;
                }                              
            }
            break;
        }
        // Remove the unprocessed bytes that we don't care about
        if(!TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(inputBuf,NULL, Len))
        {
            break;
        }
        Len = 0;
    }
    return true;
}



//Send DHCP Discovery message
static void DHCPReplyToDiscovery(TCPIP_NET_IF *pNetIf,BOOTP_HEADER *Header,DHCP_SRVR_DCPT * pDhcpsDcpt,DHCPS_HASH_DCPT *pdhcpsHashDcpt,TCPIP_DHCPS_DATA *getBuf)
{
    uint8_t         i;
    DHCPS_RESULT	res;
    UDP_SOCKET      s;
    OA_HASH_ENTRY   *hE;
    TCPIP_DHCPS_DATA   putBuffer;
    DHCPS_HASH_ENTRY *dhcpsHE = 0;
    BOOTP_HEADER    rxHeader;
    uint32_t magicCookie = 0x63538263;
    TCPIP_DHCPS_OPTION optionType;
    uint32_t    optionTypeTotalLen=0;

    if(getBuf == NULL)
    {
        return;
    }
    // Set the correct socket to active and ensure that
    // enough space is available to generate the DHCP response
    s =dhcps_mod.uSkt;
    if(TCPIP_UDP_TxPutIsReady(s, DHCPS_MAX_REPONSE_PACKET_SIZE) < DHCPS_MAX_REPONSE_PACKET_SIZE)
    {
        return;
    }
//this will put the start pointer at the beginning of the TX buffer
    TCPIP_UDP_TxOffsetSet(s,0,false);
    
    //dhcps_mod.dhcpNextLease.Val = 0;
    memset((void*)&rxHeader,0,sizeof(BOOTP_HEADER));
 
    // find in pool
    res = preAssignToDHCPClient(pNetIf,Header,pDhcpsDcpt,pdhcpsHashDcpt);
    if( res != DHCPS_RES_OK) 
    {
        return;
    }

// Before sending OFFER, get the perfect Hash Entry
    hE = TCPIP_OAHASH_EntryLookup(pdhcpsHashDcpt->hashDcpt, &Header->ClientMAC);
    if(hE != 0)
    {
        dhcpsHE = ( DHCPS_HASH_ENTRY *) hE;
    }

//Get the write pointer:
    putBuffer.head=putBuffer.wrPtr = TCPIP_UDP_TxPointerGet(s);
    // set the write and end pointer for DHCP server data
    putBuffer.wrPtr = putBuffer.head;
    putBuffer.endPtr = putBuffer.head+DHCPS_MAX_REPONSE_PACKET_SIZE;

    // Begin putting the BOOTP Header and DHCP options
    rxHeader.MessageType = BOOT_REPLY;
    // Reply with the same Hardware Type, Hardware Address Length, Hops, and Transaction ID fields
    rxHeader.HardwareType = Header->HardwareType;
    rxHeader.HardwareLen = Header->HardwareLen;
    // BOOTP number of HOPs
    rxHeader.Hops = Header->Hops;
    // BOOTP transaction ID
    rxHeader.TransactionID = Header->TransactionID;
    // Seconds Elaplsed .. Not yet supported
    rxHeader.SecondsElapsed = 0; // NOT USED
    // BOOTP header Flag
    rxHeader.BootpFlags = Header->BootpFlags;
    // Client IP address, only filled in if client is in
    // BOUND, RENEW or REBINDING state and can respond to ARP requests.
    rxHeader.ClientIP.Val = 0; // Not yet Assigned
    // Leased IP address or Client IP address
    rxHeader.YourIP.Val = dhcpsHE->ipAddress.Val;
    // IP address of next server to use in bootstrap;
    //returned in DHCPOFFER, DHCPACK by server.
    rxHeader.NextServerIP.Val = pNetIf->netIPAddr.Val;
    // Relay agent IP address, used in booting via a relay agent.
    rxHeader.RelayAgentIP.Val = 0;
    // Client MAC address
    memcpy(&rxHeader.ClientMAC,&Header->ClientMAC,sizeof(TCPIP_MAC_ADDR));

    // copy the BOOT RX header contents to the processing Buffer
    TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t*)&rxHeader,&putBuffer,sizeof(BOOTP_HEADER));

    // Remaining 10 bytes of client hardware address, server host name: Null string (not used)
    for(i = 0; i < DHCPS_UNUSED_BYTES_FOR_TX; i++)	
    {
        TCPIP_DHCPS_DataCopyToProcessBuffer(0,&putBuffer);
    }
    TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t *)&magicCookie, &putBuffer, sizeof(magicCookie));

    // Options: DHCP Offer
    optionType.messageType.optionType = DHCP_MESSAGE_TYPE;
    optionType.messageType.optionTypeLen = 1;
    optionType.messageType.byteVal = DHCP_OFFER_MESSAGE;
    optionTypeTotalLen = sizeof(optionType.messageType);
  
    // Option: Subnet Mask
    optionType.subnetmaskType.optionType = DHCP_SUBNET_MASK;
    optionType.subnetmaskType.optionTypeLen = sizeof(IPV4_ADDR);
    optionType.subnetmaskType.intVal = pNetIf->netMask.Val;
    optionTypeTotalLen += sizeof(optionType.subnetmaskType);
   
    // Option: Server identifier
    optionType.serverIdentifierType.optionType = DHCP_SERVER_IDENTIFIER;
    optionType.serverIdentifierType.optionTypeLen = sizeof(IPV4_ADDR);
    optionType.serverIdentifierType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.serverIdentifierType);

    // Option: Router/Gateway address
    optionType.routerType.optionType = DHCP_ROUTER;
    optionType.routerType.optionTypeLen = sizeof(IPV4_ADDR);
    optionType.routerType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.routerType);
  
    // Option: DNS server address
    optionType.dnsType.optionType = DHCP_DNS;
    optionType.dnsType.optionTypeLen = sizeof(IPV4_ADDR);
    optionType.dnsType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.dnsType);
    
     // Option: Lease duration
    optionType.ipLeaseTimeType.optionType = DHCP_IP_LEASE_TIME;
    optionType.ipLeaseTimeType.optionTypeLen = 4;
    optionType.ipLeaseTimeType.intVal = pdhcpsHashDcpt->leaseDuartion;
    optionType.ipLeaseTimeType.intVal = TCPIP_Helper_htonl(optionType.ipLeaseTimeType.intVal);
    optionTypeTotalLen += sizeof(optionType.ipLeaseTimeType);
  
    // copy the DHCP Reply Option type contents to the processing Buffer
    TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t*)&optionType,&putBuffer,optionTypeTotalLen);

    // No more options, mark ending
    TCPIP_DHCPS_DataCopyToProcessBuffer(DHCP_END_OPTION,&putBuffer);

    // Add zero padding to ensure compatibility with old BOOTP relays that discard small packets (<300 UDP octets)
    while(putBuffer.wrPtr < putBuffer.endPtr)
    {
        TCPIP_DHCPS_DataCopyToProcessBuffer(0,&putBuffer);
    }

    // Force remote destination address to be the broadcast address, regardless
    // of what the node's source IP address was (to ensure we don't try to
    // unicast to 0.0.0.0).
    TCPIP_UDP_BcastIPV4AddressSet( s,UDP_BCAST_NETWORK_LIMITED,pNetIf);

    IP_MULTI_ADDRESS tmp_MultiAddr; tmp_MultiAddr.v4Add = pNetIf->netIPAddr;
    TCPIP_UDP_SourceIPAddressSet(s,IP_ADDRESS_TYPE_IPV4,&tmp_MultiAddr);

    // Once it is completed writing into the buffer, you need to update the Tx offset again,
    // because the socket flush function calculates how many bytes are in the buffer using the current write pointer:
    TCPIP_UDP_TxOffsetSet(s,(uint16_t)(putBuffer.wrPtr - putBuffer.head), false);

    // Transmit the packet
    TCPIP_UDP_Flush(s);

}

// Replies to a DHCP Inform message.
static void DHCPSReplyToInform(TCPIP_NET_IF* pNetIf,BOOTP_HEADER *boot_header, DHCP_SRVR_DCPT* pDhcpsDcpt,
                                DHCPS_HASH_DCPT *pdhcpsHashDcpt,bool bAccept,TCPIP_DHCPS_DATA *getBuf)
{
    uint8_t         i;
    UDP_SOCKET      s;
    OA_HASH_ENTRY   	*hE;
    TCPIP_DHCPS_DATA   putBuffer;
    BOOTP_HEADER    rxHeader;
    uint32_t magicCookie = 0x63538263;
    TCPIP_DHCPS_OPTION optionType;
    IPV4_ADDR           ClientIP;
    uint32_t    optionTypeTotalLen=0;

    if(getBuf == NULL)
    {
        return;
    }
    // Set the correct socket to active and ensure that
    // enough space is available to generate the DHCP response
    s = dhcps_mod.uSkt;
    if(TCPIP_UDP_TxPutIsReady(s, DHCPS_MAX_REPONSE_PACKET_SIZE) < DHCPS_MAX_REPONSE_PACKET_SIZE)
    {
        return;
    }
    //this will put the start pointer at the beginning of the TX buffer
    TCPIP_UDP_TxOffsetSet(s,0,false);
    
    // Search through all remaining options and look for the Requested IP address field
    // Obtain options

    while(getBuf->endPtr-getBuf->wrPtr)
    {
        uint8_t Option, Len;
        TCPIP_MAC_ADDR tmp_MacAddr;

        // Get option type
        if(!TCPIP_DHCPS_GetDataFromUDPBuff(getBuf,(uint8_t *)&Option))
        {
            return;
        }
        if(Option == DHCP_END_OPTION)
        {
            break;
        }

        // Get option length
        if(!TCPIP_DHCPS_GetDataFromUDPBuff(getBuf,(uint8_t *)&Len))
        {
            return;
        }
 
        if((Option == DHCP_PARAM_REQUEST_CLIENT_ID) && (Len == 7u))
        {
            // Get the requested IP address and see if it is the one we have on offer.	
            // If not, we should send back a NAK, but since there could be some other
            // DHCP server offering this address, we'll just silently ignore this request.
            if(!TCPIP_DHCPS_GetDataFromUDPBuff(getBuf,(uint8_t *)&i))
            {
                return;
            }
            if(!TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(getBuf,(uint8_t*)&tmp_MacAddr, 6))
            {
                return;
            }
            Len -= 7;
            hE = TCPIP_OAHASH_EntryLookup(pdhcpsHashDcpt->hashDcpt, &tmp_MacAddr);
            ClientIP = boot_header->ClientIP;
            if(hE !=0)
            {
                if(TCPIP_DHCPS_HashIPKeyCompare(pdhcpsHashDcpt->hashDcpt, hE, ClientIP.v) == 0)
                {
                    bAccept = true;
                }
                else
				{
                    bAccept = false;
				}
            }
            else
            {
                // do a IP address validation .check if the Ip address in the same subnet
                // as per IP address range is decided per interface.
                if((pNetIf->netIPAddr.Val & pNetIf->netMask.Val)==
                        (ClientIP.Val & pNetIf->netMask.Val))
                {
                    if(_DHCPSAddCompleteEntry(pDhcpsDcpt->netIx, ClientIP.v, &boot_header->ClientMAC,DHCPS_FLAG_ENTRY_COMPLETE) != DHCPS_RES_OK)
                    {
                        return;
                    }
                    bAccept = true;
                }
                bAccept = false;
            }
            break;
        }

        // Remove the unprocessed bytes that we don't care about
        if(!TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(getBuf,NULL, Len))
        {
            break;
        }
    }
     if(!bAccept)
     {
         return;
     }


     //Get the write pointer:
    putBuffer.head=putBuffer.wrPtr = TCPIP_UDP_TxPointerGet(s);
	// set the write and end pointer for DHCP server data
    putBuffer.wrPtr = putBuffer.head;
    putBuffer.endPtr = putBuffer.head+DHCPS_MAX_REPONSE_PACKET_SIZE;


    memset((void*)&rxHeader,0,sizeof(BOOTP_HEADER));
    // Begin putting the BOOTP Header and DHCP options
//    TCPIP_DHCPS_DataCopyToProcessBuffer(BOOT_REPLY,&putBuffer);
    rxHeader.MessageType = BOOT_REPLY;
    // Reply with the same Hardware Type, Hardware Address Length, Hops, and Transaction ID fields
    rxHeader.HardwareType = boot_header->HardwareType;
    rxHeader.HardwareLen = boot_header->HardwareLen;
    // BOOTP number of HOPs
    rxHeader.Hops = boot_header->Hops;
    // BOOTP transaction ID
    rxHeader.TransactionID = boot_header->TransactionID;
    // Seconds Elaplsed .. Not yet supported
    rxHeader.SecondsElapsed = 0; // NOT USED
    // BOOTP header Flag
    rxHeader.BootpFlags = boot_header->BootpFlags;
    // Client IP address, only filled in if client is in
    // BOUND, RENEW or REBINDING state and can respond to ARP requests.
    if(bAccept)
    {
        rxHeader.ClientIP.Val = boot_header->ClientIP.Val; // Not yet Assigned
    }
    else
    {
        rxHeader.ClientIP.Val = 0; // Not yet Assigned
    }
    // Leased IP address or Client IP address
    rxHeader.YourIP.Val = 0;
    // IP address of next server to use in bootstrap;
    //returned in DHCPOFFER, DHCPACK by server.
    if(bAccept)
    {
        rxHeader.NextServerIP.Val = pNetIf->netIPAddr.Val;
    }
    else
    {
        rxHeader.NextServerIP.Val = 0;
    }
    // Relay agent IP address, used in booting via a relay agent.
    rxHeader.RelayAgentIP.Val = 0;
    // Client MAC address
    memcpy(&rxHeader.ClientMAC,&boot_header->ClientMAC,sizeof(TCPIP_MAC_ADDR));

    // copy the BOOT RX header contents to the processing Buffer
    TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t*)&rxHeader,&putBuffer,sizeof(BOOTP_HEADER));
	
    for(i = 0; i < DHCPS_UNUSED_BYTES_FOR_TX; i++)	// Remaining 10 bytes of client hardware address, server host name: Null string (not used)
    {
        TCPIP_DHCPS_DataCopyToProcessBuffer(0,&putBuffer); // Boot filename: Null string (not used)
    }
    TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t*)&magicCookie, &putBuffer, sizeof(magicCookie));

    // Options: DHCP lease ACKnowledge
    optionType.messageType.optionType = DHCP_OPTION_ACK_MESSAGE;
    optionType.messageType.optionTypeLen = 1;
    if(bAccept)
    {
        optionType.messageType.byteVal = DHCP_ACK_MESSAGE;
    }
    optionTypeTotalLen = sizeof(optionType.messageType);

  // Option: Subnet Mask
    optionType.subnetmaskType.optionType = DHCP_SUBNET_MASK;
    optionType.subnetmaskType.optionTypeLen = sizeof(IPV4_ADDR);
    optionType.subnetmaskType.intVal = pNetIf->netMask.Val;
    optionTypeTotalLen += sizeof(optionType.subnetmaskType);

    // Option: Server identifier
    optionType.serverIdentifierType.optionType = DHCP_SERVER_IDENTIFIER;
    optionType.serverIdentifierType.optionTypeLen = sizeof(IPV4_ADDR);
    optionType.serverIdentifierType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.serverIdentifierType);
  

    // Option: Router/Gateway address
    optionType.routerType.optionType = DHCP_ROUTER;
    optionType.routerType.optionTypeLen = sizeof(IPV4_ADDR);
    optionType.routerType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.routerType);

    // Option: DNS server address
    optionType.dnsType.optionType = DHCP_DNS;
    optionType.dnsType.optionTypeLen = sizeof(IPV4_ADDR);
    optionType.dnsType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.dnsType);

    // copy the DHCP Reply Option type contents to the processing Buffer
    TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t*)&optionType,&putBuffer,optionTypeTotalLen);

    // No more options, mark ending
    TCPIP_DHCPS_DataCopyToProcessBuffer(DHCP_END_OPTION,&putBuffer);

    // Add zero padding to ensure compatibility with old BOOTP relays that discard small packets (<300 UDP octets)
    while(putBuffer.wrPtr < putBuffer.endPtr)
    {
        TCPIP_DHCPS_DataCopyToProcessBuffer(0,&putBuffer);
    }
// Put completed for DHCP packet buffer to the UDP buffer


    IP_MULTI_ADDRESS tmp_MultiAddr; tmp_MultiAddr.v4Add = pNetIf->netIPAddr;
    TCPIP_UDP_SourceIPAddressSet(s,IP_ADDRESS_TYPE_IPV4,&tmp_MultiAddr);

    // Once it is completed writing into the buffer, you need to update the Tx offset again,
    // because the socket flush function calculates how many bytes are in the buffer using the current write pointer:
    TCPIP_UDP_TxOffsetSet(s,(uint16_t)(putBuffer.wrPtr - putBuffer.head), false);
    
    // Transmit the packet
    TCPIP_UDP_Flush(s);
}

//Replies to a DHCP Request message.
static void DHCPReplyToRequest(TCPIP_NET_IF* pNetIf,BOOTP_HEADER *boot_header, bool bAccept, bool bRenew,
                        DHCPS_HASH_DCPT *pdhcpsHashDcpt,TCPIP_DHCPS_DATA *getBuf,DHCP_SRVR_DCPT * pDhcpsDcpt)
{
    uint8_t         i;
    UDP_SOCKET      s;
    IPV4_ADDR       ipAddr;
    TCPIP_DHCPS_DATA   putBuffer;
    OA_HASH_ENTRY   *hE = 0;
    BOOTP_HEADER    rxHeader;
    uint32_t magicCookie = 0x63538263;
    TCPIP_DHCPS_OPTION optionType;
    uint32_t    optionTypeTotalLen=0;


    if(getBuf == NULL)
    {
        return;
    }
    // Set the correct socket to active and ensure that
    // enough space is available to generate the DHCP response
    s =dhcps_mod.uSkt;
    if(TCPIP_UDP_TxPutIsReady(s, DHCPS_MAX_REPONSE_PACKET_SIZE) < DHCPS_MAX_REPONSE_PACKET_SIZE)
    {
         return;
    }
    //this will put the start pointer at the beginning of the TX buffer
    TCPIP_UDP_TxOffsetSet(s,0,false);
    // Search through all remaining options and look for the Requested IP address field
    // Obtain options
    while(getBuf->endPtr-getBuf->wrPtr)
    {
        uint8_t Option, Len;
        uint32_t dw;
        TCPIP_MAC_ADDR tmp_MacAddr;

        // Get option type
        if(!TCPIP_DHCPS_GetDataFromUDPBuff(getBuf,(uint8_t*)&Option))
        {
            return;
        }
        if(Option == DHCP_END_OPTION)
        {
            break;
        }

        // Get option length
        if(!TCPIP_DHCPS_GetDataFromUDPBuff(getBuf,(uint8_t*)&Len))
        {
            return;
        }

        // Process option
        if(bRenew)
        {
            if((Option == DHCP_PARAM_REQUEST_CLIENT_ID) && (Len == 7u))
            {
                // Get the requested IP address and see if it is the one we have on offer.  If not, we should send back a NAK, but since there could be some other DHCP server offering this address, we'll just silently ignore this request.
                if(!TCPIP_DHCPS_GetDataFromUDPBuff(getBuf,(uint8_t*)&i))
                {
                    return;
                }
                if(!TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(getBuf,(uint8_t*)&tmp_MacAddr, 6))
                {
                    return;
                }
                Len -= 7;
                hE = TCPIP_OAHASH_EntryLookup(pdhcpsHashDcpt->hashDcpt, &tmp_MacAddr);
                if(hE !=0)
                {
                    IPV4_ADDR ClientIP = boot_header->ClientIP;
                    if(TCPIP_DHCPS_HashIPKeyCompare(pdhcpsHashDcpt->hashDcpt, hE, ClientIP.v) == 0)
                    {
                        DHCPS_HASH_ENTRY * dhcpsHE = (DHCPS_HASH_ENTRY *)hE;
                        // update lease time;
                        _DHCPSUpdateEntry(dhcpsHE);
                    }
                    else
                    {
                        TCPIP_OAHASH_EntryRemove(pdhcpsHashDcpt->hashDcpt,hE);
                        bAccept = false;
                    }
                }
                else
                {
                    bAccept = false;
                }
                continue;
            }
        }
        else
        {
            if((Option == DHCP_PARAM_REQUEST_IP_ADDRESS) && (Len == 4u))
            {
                // Get the requested IP address and see if it is the one we have on offer.  
                // If not, we should send back a NAK, but since there could be some other
                // DHCP server offering this address, we'll just silently ignore this request.
                if(!TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(getBuf,(uint8_t*)&dw, 4))
                {
                    return;
                }

                Len -= 4;
                hE = TCPIP_OAHASH_EntryLookup(pdhcpsHashDcpt->hashDcpt, &boot_header->ClientMAC);
                if(hE != 0)
                {
                    if(TCPIP_DHCPS_HashIPKeyCompare(pdhcpsHashDcpt->hashDcpt, hE, (uint8_t*)&dw) == 0)
                    {
                        DHCPS_HASH_ENTRY * dhcpsHE = (DHCPS_HASH_ENTRY *)hE;
                        bAccept = true;
                        _DHCPSUpdateEntry(dhcpsHE);
                    }
                    else
                    {
                        bAccept = false;
                        //remove Hash entry;
                        TCPIP_OAHASH_EntryRemove(pdhcpsHashDcpt->hashDcpt,hE);
                    }
                }
                else
                {
                    if((pNetIf->netIPAddr.Val & pNetIf->netMask.Val) != (dw & pNetIf->netMask.Val))
                    {
                        bAccept = false;
                        break;
                    }
                    if(_DHCPSAddCompleteEntry(pNetIf->netIfIx, (uint8_t*)&dw, &boot_header->ClientMAC, DHCPS_FLAG_ENTRY_COMPLETE)!= DHCPS_RES_OK)
                    {
                        return ;
                    }
                }
                continue;
            }
        }
            // Remove the unprocessed bytes that we don't care about
        if(!TCPIP_DHCPS_GetArrayOfDataFromUDPBuff(getBuf,NULL, Len))
        {
            break;
        }
        Len = 0;
    }

    //Get the write pointer:
    putBuffer.head=putBuffer.wrPtr = TCPIP_UDP_TxPointerGet(s);
    // set the write and end pointer for DHCP server data
    putBuffer.wrPtr = putBuffer.head;
    putBuffer.endPtr = putBuffer.head+DHCPS_MAX_REPONSE_PACKET_SIZE;

    memset((void*)&rxHeader,0,sizeof(BOOTP_HEADER));
    
// Begin putting the BOOTP Header and DHCP options
    rxHeader.MessageType = BOOT_REPLY;
    // Reply with the same Hardware Type, Hardware Address Length, Hops, and Transaction ID fields
    rxHeader.HardwareType = boot_header->HardwareType;
    rxHeader.HardwareLen = boot_header->HardwareLen;
    // BOOTP number of HOPs
    rxHeader.Hops = boot_header->Hops;
    // BOOTP transaction ID
    rxHeader.TransactionID = boot_header->TransactionID;
    // Seconds Elaplsed .. Not yet supported
    rxHeader.SecondsElapsed = 0; // NOT USED
    // BOOTP header Flag
    rxHeader.BootpFlags = boot_header->BootpFlags;
    if(bAccept)
    {
        hE = TCPIP_OAHASH_EntryLookup(pdhcpsHashDcpt->hashDcpt, &boot_header->ClientMAC);
        if(hE != 0)
        {
            ipAddr = ((DHCPS_HASH_ENTRY*)hE)->ipAddress;
        }
        else
        {
            bAccept =  false;
            ipAddr.Val = 0;
        }
    }
    else
    {
        ipAddr.Val=0u;
    }
    // Client IP address, only filled in if client is in
    // BOUND, RENEW or REBINDING state and can respond to ARP requests.
    rxHeader.ClientIP.Val = boot_header->ClientIP.Val; // Not yet Assigned
    // Leased IP address or Client IP address
    rxHeader.YourIP.Val = ipAddr.Val;
    // IP address of next server to use in bootstrap;
    //returned in DHCPOFFER, DHCPACK by server.
    rxHeader.NextServerIP.Val = 0;
    // Relay agent IP address, used in booting via a relay agent.
    rxHeader.RelayAgentIP.Val = 0;
    // Client MAC address
    memcpy(&rxHeader.ClientMAC,&boot_header->ClientMAC,sizeof(TCPIP_MAC_ADDR));

    // copy the BOOT RX header contents to the processing Buffer
    TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t*)&rxHeader,&putBuffer,sizeof(BOOTP_HEADER));
    for(i = 0; i < DHCPS_UNUSED_BYTES_FOR_TX; i++)	// Remaining 10 bytes of client hardware address, server host name: Null string (not used)
    {
        TCPIP_DHCPS_DataCopyToProcessBuffer(0,&putBuffer); // Boot filename: Null string (not used)
    }

    TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t*)&magicCookie, &putBuffer, sizeof(magicCookie));

    // Options: DHCP lease ACKnowledge
    optionType.messageType.optionType = DHCP_OPTION_ACK_MESSAGE;
    optionType.messageType.optionTypeLen = 1;
    if(bAccept)
    {
        optionType.messageType.byteVal = DHCP_ACK_MESSAGE;
    }
    else    // Send a NACK
    {
        optionType.messageType.byteVal = DHCP_NAK_MESSAGE;
    }
     optionTypeTotalLen = sizeof(optionType.messageType);
     
    optionType.renewalTime_t1.optionType = DHCP_PARAM_RENEWAL_TIME_OPTION;
    optionType.renewalTime_t1.optionTypeLen = 4;
    optionType.renewalTime_t1.intVal = 0.5 * pdhcpsHashDcpt->leaseDuartion;
    optionType.renewalTime_t1.intVal = TCPIP_Helper_htonl(optionType.renewalTime_t1.intVal);
    optionTypeTotalLen += sizeof(optionType.renewalTime_t1);
    
    optionType.rebindTime_t2.optionType = DHCP_PARAM_REBIND_TIME_OPTION;
    optionType.rebindTime_t2.optionTypeLen = 4;
    optionType.rebindTime_t2.intVal = 0.875 * pdhcpsHashDcpt->leaseDuartion;
    optionType.rebindTime_t2.intVal = TCPIP_Helper_htonl(optionType.rebindTime_t2.intVal);
    optionTypeTotalLen += sizeof(optionType.rebindTime_t2);
    
     //    // Option: Subnet Mask
    optionType.subnetmaskType.optionType = DHCP_SUBNET_MASK;
    optionType.subnetmaskType.optionTypeLen = sizeof(IPV4_ADDR);
    optionType.subnetmaskType.intVal = pNetIf->netMask.Val;
    optionTypeTotalLen += sizeof(optionType.subnetmaskType);

     // Option: Server identifier
    optionType.serverIdentifierType.optionType = DHCP_SERVER_IDENTIFIER;
    optionType.serverIdentifierType.optionTypeLen = sizeof(IPV4_ADDR);
    optionType.serverIdentifierType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.serverIdentifierType);

    // Option: Router/Gateway address
    optionType.routerType.optionType = DHCP_ROUTER;
    optionType.routerType.optionTypeLen = sizeof(IPV4_ADDR);
    optionType.routerType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.routerType);

    // Option: DNS server address
    optionType.dnsType.optionType = DHCP_DNS;
    optionType.dnsType.optionTypeLen = sizeof(IPV4_ADDR);
    optionType.dnsType.intVal = pNetIf->netIPAddr.Val;
    optionTypeTotalLen += sizeof(optionType.dnsType);

     // Option: Lease duration
    optionType.ipLeaseTimeType.optionType = DHCP_IP_LEASE_TIME;
    optionType.ipLeaseTimeType.optionTypeLen = 4;
    optionType.ipLeaseTimeType.intVal = pdhcpsHashDcpt->leaseDuartion;
    optionType.ipLeaseTimeType.intVal = TCPIP_Helper_htonl(optionType.ipLeaseTimeType.intVal);
    optionTypeTotalLen += sizeof(optionType.ipLeaseTimeType);

     // copy the DHCP Reply Option type contents to the processing Buffer
    TCPIP_DHCPS_CopyDataArrayToProcessBuff((uint8_t*)&optionType,&putBuffer,optionTypeTotalLen);
    
    // No more options, mark ending
    TCPIP_DHCPS_DataCopyToProcessBuffer(DHCP_END_OPTION,&putBuffer);

    // Add zero padding to ensure compatibility with old BOOTP relays that discard small packets (<300 UDP octets)
     while(putBuffer.wrPtr < putBuffer.endPtr)
    {
        TCPIP_DHCPS_DataCopyToProcessBuffer(0,&putBuffer);
    }

//Put completed for DHCP packet buffer to the UDP buffer

    // Force remote destination address to be the broadcast address, regardless
    // of what the node's source IP address was (to ensure we don't try to
    // unicast to 0.0.0.0).
    if(false == bRenew)
    {
        TCPIP_UDP_BcastIPV4AddressSet( s,UDP_BCAST_NETWORK_LIMITED,pNetIf);
    }

    IP_MULTI_ADDRESS tmp_MultiAddr; tmp_MultiAddr.v4Add = pNetIf->netIPAddr;
    TCPIP_UDP_SourceIPAddressSet(s,IP_ADDRESS_TYPE_IPV4,&tmp_MultiAddr);

    // Once it is completed writing into the buffer, you need to update the Tx offset again,
    // because the socket flush function calculates how many bytes are in the buffer using the current write pointer:
    TCPIP_UDP_TxOffsetSet(s,(uint16_t)(putBuffer.wrPtr - putBuffer.head), false);

    // Transmit the packet
    TCPIP_UDP_Flush(s);
}

static bool isMacAddrEffective(const TCPIP_MAC_ADDR *macAddr)
{
    int i;
    for(i=0;i<6;i++)
    {
        if(macAddr->v[i] != 0) return true;
    }
    return false;
}

static bool _DHCPSAddCompleteEntry(int intfIdx, const uint8_t* pIPAddr, TCPIP_MAC_ADDR* hwAdd, DHCPS_ENTRY_FLAGS entryFlag)
{
    DHCPS_HASH_DCPT  *pdhcpsDcpt;
    OA_HASH_ENTRY   *hE;
    DHCPS_HASH_ENTRY* dhcpsHE;

    pdhcpsDcpt = &gPdhcpsHashDcpt;
    
    hE = TCPIP_OAHASH_EntryLookupOrInsert(pdhcpsDcpt->hashDcpt, hwAdd);
    if(hE == 0)
    {   // oops, hash full?
        return DHCPS_RES_CACHE_FULL;
    }

    // now in cache
    dhcpsHE = (DHCPS_HASH_ENTRY*)hE;
    
    if(dhcpsHE->hEntry.flags.newEntry != 0)
    {   // populate the new entry
    	dhcpsHE->intfIdx = intfIdx;
        _DHCPSSetHashEntry(dhcpsHE, entryFlag, hwAdd, pIPAddr);
    }
    else
    {   // existent entry
        _DHCPSUpdateEntry(dhcpsHE);
    }

    return DHCPS_RES_OK;
}


static void TCPIP_DHCPS_TaskForLeaseTime(void)
{
    uint32_t current_timer = SYS_TMR_TickCountGet();
    DHCPS_HASH_ENTRY* dhcpsHE;
    DHCPS_HASH_DCPT  *pdhcpsDcpt;
    OA_HASH_ENTRY	*hE;
    int bktIx=0;
    OA_HASH_DCPT	*pOH;
    TCPIP_NET_IF* pNetIf;

    pdhcpsDcpt = &gPdhcpsHashDcpt;
    pOH = pdhcpsDcpt->hashDcpt;

    if(gPdhcpSDcpt == NULL)
    {
        return;
    }
    
    pNetIf = (TCPIP_NET_IF*)TCPIP_UDP_SocketNetGet(dhcps_mod.uSkt);

// check the lease values and if there is any entry whose lease value exceeds the lease duration remove the lease entries from the HASH.

    for(bktIx = 0; bktIx < pOH->hEntries; bktIx++)
    {
        hE = TCPIP_OAHASH_EntryGet(pOH, bktIx);        
    	if((hE->flags.busy != 0) && (hE->flags.value & DHCPS_FLAG_ENTRY_COMPLETE))
    	{
            dhcpsHE = (DHCPS_HASH_ENTRY*)hE;
            if((current_timer - dhcpsHE->Client_Lease_Time) >= pdhcpsDcpt->leaseDuartion* SYS_TMR_TickCounterFrequencyGet())
            {
                dhcpsHE->Client_Lease_Time = 0;
                TCPIP_OAHASH_EntryRemove(pOH,hE);
            }
    	}// Check if there is any entry whose DHCPS flag is INCOMPLETE, 
        // i,e DHCPS server did not receive the request from the client regarding that leased address.
        else if((hE->flags.busy != 0) && (hE->flags.value & DHCPS_FLAG_ENTRY_INCOMPLETE))
    	{
            dhcpsHE = (DHCPS_HASH_ENTRY*)hE;
            if((current_timer - dhcpsHE->pendingTime) >= TCPIP_DHCPS_LEASE_REMOVED_BEFORE_ACK* SYS_TMR_TickCounterFrequencyGet())
            {
                dhcpsHE->pendingTime = 0;
                TCPIP_OAHASH_EntryRemove(pOH,hE);
            }
    	}
        // remove the entry if the link is down or Wifi Mac is not connected
        if(hE->flags.busy != 0)
        {
            dhcpsHE = (DHCPS_HASH_ENTRY*)hE;
            pNetIf = (TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(dhcpsHE->intfIdx);
            if(pNetIf && !TCPIP_STACK_NetworkIsLinked(pNetIf))
            {                
                dhcpsHE->pendingTime = 0;
                TCPIP_OAHASH_EntryRemove(pOH,hE);
            }
        }
    }
}

static DHCPS_RESULT _DHCPS_FindValidAddressFromPool(BOOTP_HEADER *Header,DHCP_SRVR_DCPT * pDhcpsDcpt,DHCPS_HASH_DCPT *pdhcpsHashDcpt,IPV4_ADDR *reqIPAddress)
{
    OA_HASH_ENTRY   	*hE;
    IPV4_ADDR		  tempIpv4Addr;
    size_t bktIx = 0;
    
    if(reqIPAddress != 0)
    {
        tempIpv4Addr.Val = reqIPAddress->Val;                
    }
    else
    {
        tempIpv4Addr.Val = 0;
    }
    
    if(false == isMacAddrEffective(&(Header->ClientMAC))) 
    {
        return DHCPS_RES_NO_ENTRY;
    }
	
    // Find in Pool, look for the same MAC addr
    hE = TCPIP_OAHASH_EntryLookup(pdhcpsHashDcpt->hashDcpt, &Header->ClientMAC);
    if(hE !=0)
    {
        if((DHCPSLocateRequestedIpAddress(&tempIpv4Addr) != DHCPS_RES_OK)&&(dhcps_mod.smState == TCPIP_DHCPS_GET_NEW_ADDRESS))
        {
            bktIx = DHCPSgetFreeHashIndex(pdhcpsHashDcpt->hashDcpt, &Header->ClientMAC,&pDhcpsDcpt->intfAddrsConf.startIPAddress,(IPV4_ADDR*)&tempIpv4Addr);
            if(bktIx == -1)
            {
                return DHCPS_RES_CACHE_FULL;
            }
            dhcps_mod.dhcpNextLease.v[0] = pDhcpsDcpt->intfAddrsConf.startIPAddress.v[0] & 0xFF;
            dhcps_mod.dhcpNextLease.v[1] = pDhcpsDcpt->intfAddrsConf.startIPAddress.v[1] & 0xFF;
            dhcps_mod.dhcpNextLease.v[2] = pDhcpsDcpt->intfAddrsConf.startIPAddress.v[2] & 0xFF;
            dhcps_mod.dhcpNextLease.v[3] = (pDhcpsDcpt->intfAddrsConf.startIPAddress.v[3] & 0xFF) + bktIx;
        }
        return DHCPS_RES_OK;
    }
    else
    {
        if((dhcps_mod.dhcpNextLease.Val == 0)||(dhcps_mod.smState == TCPIP_DHCPS_GET_NEW_ADDRESS))
        {
            bktIx = DHCPSgetFreeHashIndex(pdhcpsHashDcpt->hashDcpt, &Header->ClientMAC,&pDhcpsDcpt->intfAddrsConf.startIPAddress,(IPV4_ADDR*)&tempIpv4Addr);
            if(bktIx == -1)
            {
                return DHCPS_RES_CACHE_FULL;
            }
            dhcps_mod.dhcpNextLease.v[0] = pDhcpsDcpt->intfAddrsConf.startIPAddress.v[0] & 0xFF;
            dhcps_mod.dhcpNextLease.v[1] = pDhcpsDcpt->intfAddrsConf.startIPAddress.v[1] & 0xFF;
            dhcps_mod.dhcpNextLease.v[2] = pDhcpsDcpt->intfAddrsConf.startIPAddress.v[2] & 0xFF;
            dhcps_mod.dhcpNextLease.v[3] = (pDhcpsDcpt->intfAddrsConf.startIPAddress.v[3] & 0xFF) + bktIx;
        }        
    }

    return DHCPS_RES_OK;
}


static DHCPS_RESULT preAssignToDHCPClient(TCPIP_NET_IF  *pNetIf,BOOTP_HEADER *Header,DHCP_SRVR_DCPT * pDhcpsDcpt,DHCPS_HASH_DCPT *pdhcpsHashDcpt)
{
    OA_HASH_ENTRY   	*hE;
    IPV4_ADDR		  tempIpv4Addr;
    size_t bktIx = 0;

    if(false == isMacAddrEffective(&(Header->ClientMAC))) 
    {
        return DHCPS_RES_NO_ENTRY;
    }
	
    // Find in Pool, look for the same MAC addr
    hE = TCPIP_OAHASH_EntryLookup(pdhcpsHashDcpt->hashDcpt, &Header->ClientMAC);
    if(hE !=0)
    {
        return DHCPS_RES_OK;
    }
    else
    {
        if(dhcps_mod.dhcpNextLease.Val == 0)
        {
            bktIx = DHCPSgetFreeHashIndex(pdhcpsHashDcpt->hashDcpt, &Header->ClientMAC,&pDhcpsDcpt->intfAddrsConf.startIPAddress,(IPV4_ADDR*)NULL);
            if(bktIx == -1)
            {
                return DHCPS_RES_CACHE_FULL;
            }
            tempIpv4Addr.v[0] = pDhcpsDcpt->intfAddrsConf.startIPAddress.v[0] & 0xFF;
            tempIpv4Addr.v[1] = pDhcpsDcpt->intfAddrsConf.startIPAddress.v[1] & 0xFF;
            tempIpv4Addr.v[2] = pDhcpsDcpt->intfAddrsConf.startIPAddress.v[2] & 0xFF;
            tempIpv4Addr.v[3] = (pDhcpsDcpt->intfAddrsConf.startIPAddress.v[3] & 0xFF) + bktIx;
        }
        else
        {
            tempIpv4Addr.Val = dhcps_mod.dhcpNextLease.Val;
        }
        /* this decided entry to the HASH POOL with a DHCPS_FLAG_ENTRY_INCOMPLETE flag
        After receiving Request and before sending ACK , make this entry to DHCPS_FLAG_ENTRY_COMPLETE
        */
        if(_DHCPSAddCompleteEntry(pNetIf->netIfIx, tempIpv4Addr.v, &Header->ClientMAC, DHCPS_FLAG_ENTRY_INCOMPLETE) != DHCPS_RES_OK)
                return DHCPS_RES_CACHE_FULL;
    }

    return DHCPS_RES_OK;
}


int TCPIP_DHCPS_HashMACKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key)
{
    return memcmp((void*)&((DHCPS_HASH_ENTRY*)hEntry)->hwAdd, key, DHCPS_HASH_KEY_SIZE);
}

int TCPIP_DHCPS_HashIPKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const uint8_t* key)
{
    return memcmp(((DHCPS_HASH_ENTRY*)hEntry)->ipAddress.v, key, sizeof(((DHCPS_HASH_ENTRY*)hEntry)->ipAddress));
}

void TCPIP_DHCPS_HashIPKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key)
{
    memcpy(((DHCPS_HASH_ENTRY*)dstEntry)->ipAddress.v, key, sizeof(((DHCPS_HASH_ENTRY*)dstEntry)->ipAddress));
}

void TCPIP_DHCPS_HashMACKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key)
{
    memcpy(&(((DHCPS_HASH_ENTRY*)dstEntry)->hwAdd), key, DHCPS_HASH_KEY_SIZE);
}

OA_HASH_ENTRY* TCPIP_DHCPS_HashDeleteEntry(OA_HASH_DCPT* pOH)
{
    OA_HASH_ENTRY*  pBkt;
    size_t      bktIx;
    DHCPS_HASH_ENTRY  *pE;
    uint32_t current_timer = SYS_TMR_TickCountGet();
    
    for(bktIx = 0; bktIx < pOH->hEntries; bktIx++)
    {
        pBkt = TCPIP_OAHASH_EntryGet(pOH, bktIx);		
        if(pBkt->flags.busy != 0)
        {
            pE = (DHCPS_HASH_ENTRY*)pBkt;
            if((current_timer - pE->Client_Lease_Time) >= TCPIP_DHCPS_LEASE_DURATION* SYS_TMR_TickCounterFrequencyGet())
            {
                return pBkt;
            }
        }
    }
    return 0;
}

// to get the free index and to use it for calculating Leased IP address.
static size_t DHCPSgetFreeHashIndex(OA_HASH_DCPT* pOH,void* key,IPV4_ADDR *ipAddr,IPV4_ADDR *ipAddrExists)
{
    OA_HASH_ENTRY*  pBkt;
    size_t      bktIx=-1;
    size_t      probeStep=0;
    size_t      bkts = 0;
    TCPIP_UINT32_VAL   dw;
    TCPIP_UINT32_VAL   reqIP;

	
#if defined(OA_DOUBLE_HASH_PROBING)
    probeStep = TCPIP_DHCPS_HashProbeHash(pOH, key);
    if(probeStep == 0)
    {	// try to avoid probing the same bucket over and over again
            // when probeHash returns 0.
            probeStep = pOH->probeStep;
    }
#else
    probeStep = pOH->probeStep;
#endif  // defined(OA_DOUBLE_HASH_PROBING)
    if(ipAddrExists != NULL) 
    {// this is added for ICMP processing 
        if(ipAddrExists->Val != 0)
        {
            reqIP.Val = ipAddrExists->Val;
            bktIx = reqIP.v[3] ^ ipAddr->v[3];
        }
        else
        {
            reqIP.Val = 0;
            bktIx = -1;
        }
    }
    else
    {
        reqIP.Val = 0;
        bktIx = -1;
    }
    
    if((bktIx ==0xFFFFFFFF)||(bktIx >  pOH->hEntries))
    {
        bktIx = TCPIP_DHCPS_MACHashKeyHash(pOH,key);
    }

    while(bkts < pOH->hEntries)
    {
        pBkt = TCPIP_OAHASH_EntryGet(pOH, bktIx);
        dw.Val = ipAddr->Val;
        dw.v[3] |= bktIx;
        if((reqIP.Val == 0) || (reqIP.Val != dw.Val))
        {        
            if((pBkt->flags.busy == 0) &&(DHCPSLocateRequestedIpAddress((IPV4_ADDR*)&dw.Val)==DHCPS_RES_NO_ENTRY))
            {	// found unused entry
                return bktIx;
            }
        }
        // advance to the next hash slot
        bktIx += probeStep;
        if(bktIx >= pOH->hEntries)
        {
            bktIx -= pOH->hEntries;
        }

        bkts++;
    }
    return -1;	// cache full, not found
}

static DHCPS_RESULT DHCPSLocateRequestedIpAddress(IPV4_ADDR *requestedIpAddr)
{
    DHCPS_HASH_DCPT *pdhcpsDcpt;
    OA_HASH_ENTRY	*hE;
    int 			bktIx=0;
    OA_HASH_DCPT	*pOH;

    pdhcpsDcpt = &gPdhcpsHashDcpt;
    pOH = pdhcpsDcpt->hashDcpt;

// check the Requested IP address is matching anyone of the hash entry.
    for(bktIx = 0; bktIx < pOH->hEntries; bktIx++)
    {
        hE = TCPIP_OAHASH_EntryGet(pOH, bktIx);
        if((hE->flags.busy != 0) && (hE->flags.value & DHCPS_FLAG_ENTRY_COMPLETE))
        {
            if(TCPIP_DHCPS_HashIPKeyCompare(pOH, hE, requestedIpAddr->v) == 0)
                return DHCPS_RES_OK;
        }
    }
    return DHCPS_RES_NO_ENTRY;
}

TCPIP_DHCPS_LEASE_HANDLE TCPIP_DHCPS_LeaseEntryGet(TCPIP_NET_HANDLE netH, TCPIP_DHCPS_LEASE_ENTRY* pLeaseEntry, TCPIP_DHCPS_LEASE_HANDLE leaseHandle)
{
    int                 entryIx;
    OA_HASH_DCPT*       pOH;
    DHCPS_HASH_ENTRY*   pDsEntry;
    DHCPS_HASH_DCPT*	pDSHashDcpt;
    uint32_t 		current_time = SYS_TMR_TickCountGet();
    
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);
  
    if(pNetIf == 0 || pNetIf->Flags.bIsDHCPSrvEnabled == 0)
    {
        return 0;
    }

    pDSHashDcpt = &gPdhcpsHashDcpt;

    pOH = pDSHashDcpt->hashDcpt;
    if(pOH != 0)
    {   // DHCP Server proper initialized
        for(entryIx = (int)leaseHandle; entryIx < pOH->hEntries; entryIx++)
        {
            pDsEntry = (DHCPS_HASH_ENTRY*)TCPIP_OAHASH_EntryGet(pOH, entryIx);
            
            if(pDsEntry && (pDsEntry->hEntry.flags.busy != 0) && (pDsEntry->hEntry.flags.value & DHCPS_FLAG_ENTRY_COMPLETE) )
            {                
                if(pDsEntry->intfIdx != TCPIP_STACK_NetIxGet(pNetIf))
            	{
                    continue;
            	}
                // found entry
                if(pLeaseEntry)
                {
                    memcpy(&pLeaseEntry->hwAdd, &pDsEntry->hwAdd, sizeof(pDsEntry->hwAdd));
                    pLeaseEntry->ipAddress.Val = pDsEntry->ipAddress.Val;
                    pLeaseEntry->leaseTime = pDSHashDcpt->leaseDuartion*SYS_TMR_TickCounterFrequencyGet() - (current_time - pDsEntry->Client_Lease_Time);
                }
                return (TCPIP_DHCPS_LEASE_HANDLE)(entryIx + 1);
            }
        }
    }
    // no other entry
    return 0;
}

int TCPIP_DHCPS_GetPoolEntries(TCPIP_NET_HANDLE netH, TCPIP_DHCPS_POOL_ENTRY_TYPE type)
{
    int                 entryIx;
    int                 noOfEntries=0;
    OA_HASH_DCPT*       pOH;
    DHCPS_HASH_ENTRY*   pDsEntry;
    DHCPS_HASH_DCPT*	pDSHashDcpt;
	
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);
    if(pNetIf == 0 || pNetIf->Flags.bIsDHCPSrvEnabled == 0)
    {
        return 0;
    }

    pDSHashDcpt = &gPdhcpsHashDcpt;

    pOH = pDSHashDcpt->hashDcpt;
    
    if(pOH != 0)
    {   // DHCP Server proper initialized
        for(entryIx = (int)0; entryIx < pOH->hEntries; entryIx++)
        {
            pDsEntry = (DHCPS_HASH_ENTRY*)TCPIP_OAHASH_EntryGet(pOH, entryIx);
            
            switch(type)
            {
                case DHCP_SERVER_POOL_ENTRY_ALL:
                    if(pDsEntry && (pDsEntry->hEntry.flags.busy != 0))
                    {
                        if(pDsEntry->intfIdx != TCPIP_STACK_NetIxGet(pNetIf))
                        {
                            continue;
                        }
                        noOfEntries++;
                    }
                    break;
                case DHCP_SERVER_POOL_ENTRY_IN_USE:
                    if(pDsEntry && (pDsEntry->hEntry.flags.busy != 0) && (pDsEntry->hEntry.flags.value & DHCPS_FLAG_ENTRY_COMPLETE) != 0)
                    {
                        if(pDsEntry->intfIdx != TCPIP_STACK_NetIxGet(pNetIf))
                        {
                            continue;
                        }
                        noOfEntries++;
                    }
                    break;
            }

        }
    }
    return noOfEntries;
        
}

bool TCPIP_DHCPS_LeaseEntryRemove(TCPIP_NET_HANDLE netH, TCPIP_MAC_ADDR* hwAdd)
{
    OA_HASH_DCPT*       pOH;
    OA_HASH_ENTRY*	hE;
    DHCPS_HASH_ENTRY*   pDsEntry;
    DHCPS_HASH_DCPT*	pDSHashDcpt;
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);
    if(pNetIf == 0 || pNetIf->Flags.bIsDHCPSrvEnabled == 0)
    {
        return false;
    }

    pDSHashDcpt = &gPdhcpsHashDcpt;

    pOH = pDSHashDcpt->hashDcpt;

    if(hwAdd)
    {
    	hE = TCPIP_OAHASH_EntryLookup(pOH,hwAdd);
        if(hE != 0)
        {
            pDsEntry = (DHCPS_HASH_ENTRY*)hE;
            if(pDsEntry->intfIdx == pNetIf->netIfIx)
            {
                TCPIP_OAHASH_EntryRemove(pOH,hE);
                return true;
            }
        }
    }
    return false;
}

bool TCPIP_DHCPS_RemovePoolEntries(TCPIP_NET_HANDLE netH, TCPIP_DHCPS_POOL_ENTRY_TYPE type)
{
    int                 entryIx;
    OA_HASH_DCPT*       pOH;
    DHCPS_HASH_ENTRY*   pDsEntry;
    DHCPS_HASH_DCPT*	pDSHashDcpt;
	
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(netH);
    if(pNetIf == 0 || pNetIf->Flags.bIsDHCPSrvEnabled == 0)
    {
        return false;
    }

    pDSHashDcpt = &gPdhcpsHashDcpt;

    pOH = pDSHashDcpt->hashDcpt;
    
    if(pOH != 0)
    {   // DHCP Server proper initialized
        for(entryIx = (int)0; entryIx < pOH->hEntries; entryIx++)
        {
            pDsEntry = (DHCPS_HASH_ENTRY*)TCPIP_OAHASH_EntryGet(pOH, entryIx);
            
            switch(type)
            {
                case DHCP_SERVER_POOL_ENTRY_ALL:
                    if(pDsEntry && (pDsEntry->hEntry.flags.busy != 0))
                    {
                        if(pDsEntry->intfIdx != TCPIP_STACK_NetIxGet(pNetIf))
                        {
                            continue;
                        }
                        TCPIP_OAHASH_EntryRemove(pOH,&pDsEntry->hEntry);
                    }
                    break;
                case DHCP_SERVER_POOL_ENTRY_IN_USE:
                    if(pDsEntry && (pDsEntry->hEntry.flags.busy != 0) && (pDsEntry->hEntry.flags.value & DHCPS_FLAG_ENTRY_COMPLETE) != 0)
                    {
                        if(pDsEntry->intfIdx != TCPIP_STACK_NetIxGet(pNetIf))
                        {
                            continue;
                        }
                        TCPIP_OAHASH_EntryRemove(pOH,&pDsEntry->hEntry);
                    }
                    break;
            }
        }
    }
    return true;
}

size_t TCPIP_DHCPS_MACHashKeyHash(OA_HASH_DCPT* pOH, const void* key)
{
    return fnv_32_hash(key, DHCPS_HASH_KEY_SIZE) % (pOH->hEntries);
}

#if defined(OA_DOUBLE_HASH_PROBING)
size_t TCPIP_DHCPS_HashProbeHash(OA_HASH_DCPT* pOH, const void* key)
{
    return fnv_32a_hash(key, DHCPS_HASH_KEY_SIZE) % (pOH->hEntries);
}
#endif  // defined(OA_DOUBLE_HASH_PROBING)

size_t TCPIP_DHCPS_IPAddressHashKeyHash(OA_HASH_DCPT* pOH, const void* key)
{
    return fnv_32_hash(key, 4) % (pOH->hEntries);
}

static bool _DHCPSDescriptorGetFromIntf(TCPIP_NET_IF *pNetIf,uint32_t *dcptIdx)
{
    int ix=0;
    DHCP_SRVR_DCPT *pDhcpServDcpt=NULL;


    for(ix = 0, pDhcpServDcpt = gPdhcpSDcpt; ix < dhcps_mod.poolCount; ix++, pDhcpServDcpt++)
    {
        if(pDhcpServDcpt == NULL)
        {
            return false;
        }
        if(pNetIf->netIfIx != pDhcpServDcpt->netIx)
        {
            _DHCPSDbgCond(false, " Debug Message: DHCP Server Interface Number is not matching to the Network configuration DHCP server enable status :  ", pDhcpServDcpt->netIx);
            continue;
        }
        else
        {
            *dcptIdx = pDhcpServDcpt->intfAddrsConf.poolIndex;
            return true;
        }
    }
    return false;
}


bool TCPIP_DHCPS_Disable(TCPIP_NET_HANDLE hNet)
{    
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(hNet);
 
    if(pNetIf == 0)
    {
        return false;
    }
//  Now stop DHCP server
    _DHCPSrvClose(pNetIf,true);
    TCPIP_STACK_AddressServiceEvent(pNetIf, TCPIP_STACK_ADDRESS_SERVICE_DHCPS, TCPIP_STACK_ADDRESS_SERVICE_EVENT_USER_STOP);
    TCPIP_STACK_AddressServiceDefaultSet(pNetIf);
    _TCPIPStackSetConfigAddress(pNetIf, 0, 0, 0, true);
     // Remove all the HASH entries
    _DHCPSRemoveCacheEntries(&gPdhcpsHashDcpt);
    return true;
}

static bool _DHCPS_StartOperation(TCPIP_NET_IF* pNetIf,DHCP_SRVR_DCPT* pDhcpsDcpt)
{
    uint32_t poolIndex=0;

    if(gPdhcpSDcpt == NULL)
    {
        return false;
    }

    if( dhcps_mod.uSkt == INVALID_UDP_SOCKET)
    {
        dhcps_mod.uSkt = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_IPV4, TCPIP_DHCP_SERVER_PORT,  0);
        if(dhcps_mod.uSkt == INVALID_UDP_SOCKET)
        {
            return false;
        }
        TCPIP_UDP_SignalHandlerRegister(dhcps_mod.uSkt, TCPIP_UDP_SIGNAL_RX_DATA, TCPIP_DHCPSSocketRxSignalHandler, 0);
    }
    dhcps_mod.smServer = DHCP_SERVER_LISTEN;
    _DHCPSStateSet(TCPIP_DHCPS_START_RECV_NEW_PACKET);
    pNetIf->Flags.bIsDHCPSrvEnabled = true;
    //if there is no pool for any interface , use the 0th index pool configuration details
    // or the default for the interface configuration. here the server IP address will be same for
    // more than interface, if the address pool is not configured.
    if(_DHCPSDescriptorGetFromIntf(pNetIf,&poolIndex) == false)
    {
        return false;
    }
    pDhcpsDcpt = gPdhcpSDcpt+poolIndex;
    if(pDhcpsDcpt == NULL)
    {
        return false;
    }
// Get the network interface from the network index and configure IP address,
// Netmask and gateway and DNS
    _TCPIPStackSetConfigAddress(pNetIf, &pDhcpsDcpt->intfAddrsConf.serverIPAddress, &pDhcpsDcpt->intfAddrsConf.serverMask, 0, false);
    TCPIP_STACK_GatewayAddressSet(pNetIf, &pDhcpsDcpt->intfAddrsConf.serverIPAddress);
#if defined(TCPIP_STACK_USE_DNS)
    if(pNetIf->Flags.bIsDNSServerAuto != 0)
    {
        TCPIP_STACK_PrimaryDNSAddressSet(pNetIf, &pDhcpsDcpt->intfAddrsConf.serverDNS);
        TCPIP_STACK_SecondaryDNSAddressSet(pNetIf, &pDhcpsDcpt->intfAddrsConf.serverDNS2);
    }
#endif
    
    uint16_t queueSize;
     // make sure the socket is created with enough TX space
    TCPIP_UDP_OptionsGet(dhcps_mod.uSkt, UDP_OPTION_TX_QUEUE_LIMIT, (void*)&queueSize);
    if(queueSize < TCPIP_DHCPS_QUEUE_LIMIT_SIZE)
    {
        queueSize = TCPIP_DHCPS_QUEUE_LIMIT_SIZE;
        if(!TCPIP_UDP_OptionsSet(dhcps_mod.uSkt, UDP_OPTION_TX_QUEUE_LIMIT, (void*)(uintptr_t)queueSize))
        {
            return false;
        }
    }
    
     // make sure the socket is created with enough TX space
    TCPIP_UDP_OptionsGet(dhcps_mod.uSkt, UDP_OPTION_RX_QUEUE_LIMIT, (void*)&queueSize);
    if(queueSize < TCPIP_DHCPS_QUEUE_LIMIT_SIZE)
    {
        queueSize = TCPIP_DHCPS_QUEUE_LIMIT_SIZE;
        if(!TCPIP_UDP_OptionsSet(dhcps_mod.uSkt, UDP_OPTION_RX_QUEUE_LIMIT, (void*)(uintptr_t)queueSize))
        {
            return false;
        }
    }
    return true;
}

bool TCPIP_DHCPS_Enable(TCPIP_NET_HANDLE hNet)
{
    return _DHCPS_Enable(hNet, true);
}

static bool _DHCPS_Enable(TCPIP_NET_HANDLE hNet,bool checkIfUp)
{
    TCPIP_NET_IF* pNetIf ;
    DHCP_SRVR_DCPT* pDhcpsDcpt;

    if(checkIfUp)
    {
        pNetIf = _TCPIPStackHandleToNetUp(hNet);
    }
    else
    {
        pNetIf = _TCPIPStackHandleToNet(hNet);
    }
    pDhcpsDcpt = gPdhcpSDcpt;
    if((pNetIf==0)||(pDhcpsDcpt==0))
    {
        return false;
    }
    if(dhcps_mod.poolCount == 0)
    {
        SYS_ERROR(SYS_ERROR_WARNING, "DHCPS: DHCP server needs atleast one address pool! \r\n");
        return false;
    }
    if(checkIfUp)
    {
        if(TCPIP_STACK_AddressServiceCanStart(pNetIf, TCPIP_STACK_ADDRESS_SERVICE_DHCPS))
        {
            return _DHCPS_StartOperation(pNetIf,pDhcpsDcpt);
        }
    }
    else
    {
        if((pNetIf->Flags.v & TCPIP_STACK_ADDRESS_SERVICE_MASK) == TCPIP_NETWORK_CONFIG_DHCP_SERVER_ON)
        {
            return _DHCPS_StartOperation(pNetIf,pDhcpsDcpt);
        }
    }
    SYS_ERROR(SYS_ERROR_WARNING, "DHCPS: Other Services are enabled for this interface \r\n");
    return false;
}

/*****************************************************************************
  Function:
	bool TCPIP_DHCPS_IsEnabled(CPIP_NET_HANDLE hNet)

  Summary:
	Determins if the DHCP Server is enabled on the specified interface.

  Description:
	Determins if the DHCP Server is enabled on the specified interface.

  Precondition:
	None

  Parameters:
	 hNet- Interface to query.

  Returns:
	None
***************************************************************************/
bool TCPIP_DHCPS_IsEnabled(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(hNet);
    if(pNetIf)
    {
        return pNetIf->Flags.bIsDHCPSrvEnabled != 0;
    }
    return false;
}

static DHCPS_RESULT _DHCPS_EchoRequestInit(TCPIP_NET_IF *pNetIf,IPV4_ADDR *requestedIpAddr)
{
    // Initialize all the echo request parameters
    dhcpsEchoRequestState = TCPIP_DHCPS_START_ECHO_REQUEST;
    dhcpsicmpNetH = (TCPIP_NET_HANDLE)pNetIf;
    dhcpsicmpTargetAddr = *requestedIpAddr;
    
    return DHCPS_RES_ECHO_IN_PROCESS;
}


static DHCPS_RESULT _DHCPS_VerifyEchoRequestedAddress(void)
{
    switch(dhcpsicmpProcessSteps)
    {
        case DHCPS_ICMP_REQUEST_TIMEDOUT:
        case DHCPS_ICMP_REQUEST_CANCEL:
        case DHCPS_ICMP_NO_RESPONSE_RECEIVED:
            return DHCPS_RES_NO_ENTRY;
        case DHCPS_ICMP_REQUEST_SENT:
        case DHCPS_ICMP_WAIT_REPLY:
        case DHCPS_ICMP_WRONG_RESPONSE_RECEIVED:
            return DHCPS_RES_ECHO_IN_PROCESS;
        case DHCPS_ICMP_RESPONSE_RECEIVED:
            return DHCPS_RES_ENTRY_EXIST;
        case DHCPS_ICMP_IDLE:
            return DHCPS_RES_OK;
        default:
            return DHCPS_RES_OK;            
    }
}

static void TCPIP_DHCPS_EchoICMPRequestTask(void)
{
    ICMP_ECHO_RESULT echoRes;
    TCPIP_ICMP_ECHO_REQUEST echoRequest;
    bool cancelReq, newReq;
    bool killIcmp = false;
    char debugBuf[128];
    uint32_t    currentTime=0;
       
    switch(dhcpsEchoRequestState)
    {
        case TCPIP_DHCPS_START_ECHO_REQUEST:
            dhcpsicmpStartTick = 0;  // try to start as quickly as possible
            dhcpsicmpSequenceNo = SYS_RANDOM_PseudoGet();
            dhcpsicmpIdentifier = SYS_RANDOM_PseudoGet();
            dhcpsEchoRequestState = TCPIP_DHCPS_DO_ECHO_REQUEST;
            if(dhcpsicmpReqNo == 0)
            {
                dhcpsicmpReqNo = TCPIP_DHCPS_ICMP_ECHO_REQUESTS;
            }
            if(dhcpsicmpReqDelay == 0)
            {
                dhcpsicmpReqDelay = TCPIP_DHCPS_ICMP_ECHO_REQUEST_DELAY;
            }

            // no break needed here!

        case TCPIP_DHCPS_DO_ECHO_REQUEST:
            if(dhcpsicmpReqCount == dhcpsicmpReqNo)
            {   // no more requests to send
                killIcmp = true;
                break;
            }

            // check if time for another request
            cancelReq = newReq = false;
            currentTime = SYS_TMR_TickCountGet();
            if(currentTime - dhcpsicmpStartTick > (SYS_TMR_TickCounterFrequencyGet() * dhcpsicmpReqDelay) / 1000)
            {
                cancelReq = dhcpsicmpReqCount != dhcpsicmpAckRecv && dhcpsicmpReqHandle != 0;    // cancel if there is another one ongoing
                newReq = true;
            }
            else if(dhcpsicmpReqCount != dhcpsicmpAckRecv)
            {   // no reply received to the last ping 
                if(currentTime - dhcpsicmpStartTick > (SYS_TMR_TickCounterFrequencyGet() * TCPIP_DHCPS_ICMP_ECHO_TIMEOUT) / 1000)
                {   // timeout
                    _DhcpsICMPDebugMessage("request timeout.\r\n");
                    cancelReq = dhcpsicmpReqHandle != 0;    // cancel if there is another one ongoing
                    newReq = true;
                    dhcpsicmpProcessSteps = DHCPS_ICMP_REQUEST_TIMEDOUT;
                }
                // else wait some more
            }

            if(cancelReq)
            {
                if(TCPIP_ICMP_EchoRequestCancel(dhcpsicmpReqHandle) != ICMP_ECHO_OK)
                {   // this should NOT happen!
                    _DhcpsICMPDebugMessage("Cancel failed!!!\r\n");
                }
                else
                {
                    _DhcpsICMPDebugMessage("Request aborted tmo!\r\n");
                }
            }

            if(!newReq)
            {   // nothing else to do
                break;
            }

            // send another request
            echoRequest.netH = dhcpsicmpNetH;
            echoRequest.targetAddr = dhcpsicmpTargetAddr;
            echoRequest.sequenceNumber = ++dhcpsicmpSequenceNo;
            echoRequest.identifier = dhcpsicmpIdentifier;
            echoRequest.pData = dhcpsicmpPingBuff;
            echoRequest.dataSize = dhcpsicmpPingSize;
            echoRequest.callback = DHCPSPingHandler;

            {
                int ix;
                uint8_t* pBuff = dhcpsicmpPingBuff;
                for(ix = 0; ix < dhcpsicmpPingSize; ix++)
                {
                    *pBuff++ = SYS_RANDOM_PseudoGet();
                }
            }

            echoRes = TCPIP_ICMP_EchoRequest (&echoRequest, &dhcpsicmpReqHandle);

            if(echoRes >= 0 )
            {
                dhcpsicmpStartTick = SYS_TMR_TickCountGet();
                dhcpsicmpReqCount++;
                sprintf(debugBuf,"Ping: sent request %d to: [%x]\r\n", dhcpsicmpReqCount, (unsigned int)dhcpsicmpTargetAddr.Val);
                _DhcpsICMPDebugMessage(debugBuf);
                dhcpsicmpProcessSteps = DHCPS_ICMP_REQUEST_SENT;
            }
            else
            {
                sprintf(debugBuf,"Ping: failed to send request %d to: %x, error %d\r\n", dhcpsicmpReqCount, (unsigned int)dhcpsicmpTargetAddr.Val, echoRes);
                _DhcpsICMPDebugMessage(debugBuf);
                killIcmp = true;
                dhcpsicmpProcessSteps = DHCPS_ICMP_REQUEST_CANCEL;
            }
            if(killIcmp)
            {
                // Stop PING echo request 
                dhcpsEchoRequestState = TCPIP_DHCPS_STOP_ECHO_REQUEST;
            }
            break;
            case TCPIP_DHCPS_STOP_ECHO_REQUEST:
                 if(TCPIP_ICMP_EchoRequestCancel(dhcpsicmpReqHandle) == ICMP_ECHO_OK)
                 {// this should NOT happen!
                     dhcpsicmpProcessSteps = DHCPS_ICMP_REQUEST_CANCEL;
                 }
                dhcpsicmpReqHandle = 0;
                dhcpsicmpReqCount = 0;
                dhcpsEchoRequestState = TCPIP_DHCPS_ECHO_REQUEST_IDLE;
                //Stop echo request
                break;
            case TCPIP_DHCPS_ECHO_REQUEST_IDLE:
                
                break;

            default:
                killIcmp = true;
                break;
    }
    if(killIcmp)
    {
        // Stop PING echo request 
        dhcpsEchoRequestState = TCPIP_DHCPS_STOP_ECHO_REQUEST;
    }
   
}

static void DHCPSPingHandler(const  TCPIP_ICMP_ECHO_REQUEST* pEchoReq, TCPIP_ICMP_REQUEST_HANDLE iHandle, TCPIP_ICMP_ECHO_REQUEST_RESULT result, const void* param)
{
    char debugBuf[128];
    
    if(result == TCPIP_ICMP_ECHO_REQUEST_RES_OK)
    {   // reply has been received
        uint32_t errorMask = 0;     // error mask:
        // 0x1: wrong id
        // 0x2: wrong seq
        // 0x4: wrong target
        // 0x8: wrong size
        // 0x10: wrong data
        //
        if(pEchoReq->identifier != dhcpsicmpIdentifier)
        {
            errorMask |= 0x1;
        }

        if(pEchoReq->sequenceNumber != dhcpsicmpSequenceNo)
        {
            errorMask |= 0x2;
        }

        if(pEchoReq->targetAddr.Val != dhcpsicmpTargetAddr.Val)
        {
            errorMask |= 0x4;
        }

        if(pEchoReq->dataSize != dhcpsicmpPingSize)
        {
            errorMask |= 0x8;
        }

        // check the data
        int ix;
        int checkSize = pEchoReq->dataSize < dhcpsicmpPingSize ? pEchoReq->dataSize : dhcpsicmpPingSize;
        uint8_t* pSrc = dhcpsicmpPingBuff;
        uint8_t* pDst = pEchoReq->pData;
        for(ix = 0; ix < checkSize; ix++)
        {
            if(*pSrc++ != *pDst++)
            {
                errorMask |= 0x10;
                break;
            }
        }

        if(errorMask != 0)
        {   // some errors
            sprintf(debugBuf,"Ping: wrong reply received. Mask: 0x%2x\r\n", (unsigned int)errorMask);
            _DhcpsICMPDebugMessage(debugBuf);
            dhcpsicmpProcessSteps = DHCPS_ICMP_WRONG_RESPONSE_RECEIVED;
        }
        else
        {   // good reply
            sprintf(debugBuf,"Ping: Good reply received: 0x%2x\r\n", ++dhcpsicmpAckRecv);
            _DhcpsICMPDebugMessage(debugBuf);
            dhcpsicmpProcessSteps = DHCPS_ICMP_RESPONSE_RECEIVED;
        }
    }
    else
    {
        if(dhcpsicmpReqCount < dhcpsicmpReqNo)
        { // wait till all the request attempts
            sprintf(debugBuf,"Ping: Number of request attempted %d\r\n", dhcpsicmpReqCount);
            _DhcpsICMPDebugMessage(debugBuf);
            dhcpsicmpProcessSteps = DHCPS_ICMP_WAIT_REPLY;
        }
        else
        {
            sprintf(debugBuf,"Ping: request aborted by ICMP with result %d\r\n", result);
            _DhcpsICMPDebugMessage(debugBuf);
            dhcpsicmpProcessSteps = DHCPS_ICMP_NO_RESPONSE_RECEIVED;
        }
    }
    // one way or the other, request is done
    dhcpsicmpReqHandle = 0;
}
#else
bool TCPIP_DHCPS_Disable(TCPIP_NET_HANDLE hNet){return false;}
bool TCPIP_DHCPS_Enable(TCPIP_NET_HANDLE hNet){return false;}
bool TCPIP_DHCPS_IsEnabled(TCPIP_NET_HANDLE hNet){return false;}
#endif //#if defined(TCPIP_STACK_USE_DHCP_SERVER)
#endif // defined(TCPIP_STACK_USE_IPV4)

