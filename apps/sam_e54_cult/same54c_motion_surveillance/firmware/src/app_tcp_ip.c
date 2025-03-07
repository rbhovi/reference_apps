/*******************************************************************************
* Copyright (C) 2021 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_tcp_ip.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app_tcp_ip.h"
#include "app_ardu_cam.h"
#include "system_config.h"
#include "system_definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_TCP_IP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_TCP_IP_DATA app_tcp_ipData;

APP_TCP_IP_LED_STATE LEDstate = APP_TCP_IP_LED_STATE_OFF;
// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_TCP_IP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */
void APP_TCP_IP_Initialize ( void )
{
    /* Intialize the app state to wait for media attach. */
    app_tcp_ipData.state = APP_TCP_IP_TCPIP_WAIT_INIT;
    app_tcp_ipData.tcpIpInit = false;
}

/******************************************************************************
  Function:
    void APP_TCP_IP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */
void APP_TCP_IP_Tasks ( void )
{
    SYS_STATUS          tcpipStat;
    TCPIP_NET_HANDLE    netH;
    int                 nNets;
    static IPV4_ADDR    dwLastIP[2] = { {-1}, {-1} };
    IPV4_ADDR           ipAddr;
    int                 i;
    const char          *netName, *netBiosName;
    static uint32_t     startTick = 0;

    switch(app_tcp_ipData.state)
    {
        case APP_TCP_IP_TCPIP_WAIT_INIT:
            if (APP_ARDU_CAM_Task_GetStatus() == true)
            {
                tcpipStat = TCPIP_STACK_Status(sysObj.tcpip);
                if(tcpipStat < 0)
                {   // some error occurred
                    SYS_CONSOLE_MESSAGE("APP: TCP/IP stack initialization failed!\r\n");
                    app_tcp_ipData.state = APP_TCP_IP_TCPIP_ERROR;
                }
                else if(tcpipStat == SYS_STATUS_READY)
                {
                    // now that the stack is ready we can check the
                    // available interfaces and register
                    // a Bonjour service

                    nNets = TCPIP_STACK_NumberOfNetworksGet();

                    for(i = 0; i < nNets; i++)
                    {
                        netH = TCPIP_STACK_IndexToNet(i);
                        netName = TCPIP_STACK_NetNameGet(netH);
                        netBiosName = TCPIP_STACK_NetBIOSName(netH);

    #if defined(TCPIP_STACK_USE_NBNS)
                        SYS_CONSOLE_PRINT("    Interface %s on host %s - NBNS enabled\r\n", netName, netBiosName);
    #else
                        SYS_CONSOLE_PRINT("    Interface %s on host %s - NBNS disabled\r\n", netName, netBiosName);
    #endif
                        (void)netName;          // avoid compiler warning
                        (void)netBiosName;      // if SYS_CONSOLE_PRINT is null macro

    #if defined(TCPIP_STACK_USE_ZEROCONF_MDNS_SD)
                        // base name of the service Must not exceed 16 bytes long
                        // the last digit will be incremented by interface
                        char mDNSServiceName[] = "MyWebServiceNameX ";

                        mDNSServiceName[sizeof(mDNSServiceName) - 2] = '1' + i;
                        TCPIP_MDNS_ServiceRegister(netH
                                , mDNSServiceName                     // name of the service
                                ,"_http._tcp.local"                   // type of the service
                                ,80                                   // TCP or UDP port, at which this service is available
                                ,((const uint8_t *)"path=/index.htm") // TXT info
                                ,1                                    // auto rename the service when if needed
                                ,NULL                                 // no callback function
                                ,NULL);                               // no application context
    #endif
                    }

    #if defined(TCPIP_STACK_USE_HTTP_NET_SERVER)
                    // register the application HTTP processing
                    HTTP_APP_Initialize();
    #endif
                    app_tcp_ipData.tcpIpInit = true;
                    app_tcp_ipData.state = APP_TCP_IP_TRANSACT;
                }
            }
            break;

        case APP_TCP_IP_TRANSACT:
            if(SYS_TMR_TickCountGet() - startTick >= SYS_TMR_TickCounterFrequencyGet()/2ul)
            {
                startTick = SYS_TMR_TickCountGet();
                LEDstate ^= APP_TCP_IP_LED_STATE_ON;
                if(LEDstate == 1)
                {
                    APP_TCP_IP_LED_1StateSet();
                }
                else if(LEDstate == 0)
                {
                    APP_TCP_IP_LED_1StateClear();
                }
            }

            // if the IP address of an interface has changed
            // display the new value on the system console
            nNets = TCPIP_STACK_NumberOfNetworksGet();

            for(i = 0; i < nNets; i++)
            {
                netH = TCPIP_STACK_IndexToNet(i);
                if(!TCPIP_STACK_NetIsReady(netH))
                {
                    return;    // interface not ready yet!
                }
                ipAddr.Val = TCPIP_STACK_NetAddress(netH);
                if(dwLastIP[i].Val != ipAddr.Val)
                {
                    dwLastIP[i].Val = ipAddr.Val;

                    SYS_CONSOLE_MESSAGE(TCPIP_STACK_NetNameGet(netH));
                    SYS_CONSOLE_MESSAGE(" IP Address: ");
                    SYS_CONSOLE_PRINT("%d.%d.%d.%d \r\n", ipAddr.v[0], ipAddr.v[1], ipAddr.v[2], ipAddr.v[3]);
                }
            }
            break;

        default:
            break;
    }
}


/*******************************************************************************
 End of File
 */
