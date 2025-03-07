/*******************************************************************************
  BM71 Bluetooth Driver implementation

  Company:
    Microchip Technology Inc.

  File Name:
    custom_bm71.c

  Summary:
   BM71 Bluetooth Driver main source file

  Description:
    This file is the implementation of the external (public) API of the
    implementation of the BM71 driver (plus some local functions).

    The BM71 is a Bluetooth 4.2 Module that supports BLE (Bluetooth Low Energy).

    It uses a UART to receive commands from the host microcontroller (e.g. PIC32)
    and and send events back.

*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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
// DOM-IGNORE-END

/*************************************************************
 * Include files.
 ************************************************************/
#include "definitions.h"
#include "bm71_common.h"
#include "configuration.h"
#include "custom_bm71.h"
#include "custom_bm71_local.h"
#include "custom_bm71_command_send.h"
#include "custom_bm71_command_decode.h"
#include "custom_bm71_ble.h"
#include "custom_bm71_uart.h"
#include "custom_bm71_gpio.h"
#include "system/time/sys_time.h"

//#define SYS_DEBUG(x,y)    printf(y)      // comment out printf if not defined

// all #defines, enums and non-static functions and variables prefixed by

BM71_HANDLE CUSTOM_BM71_tmrHandle;

static uint16_t _timer1ms;

static void _initSPP( void );
static void _BM71ControlTasks( void );

#define CUSTOM_BM71_SPP_RxFifoSize 200                       //receive buffer max. size
uint8_t CUSTOM_BM71_SPP_RxFifo[CUSTOM_BM71_SPP_RxFifoSize];          //receive buffer
uint32_t CUSTOM_BM71_SPP_RxFifoHead, CUSTOM_BM71_SPP_RxFifoTail, CUSTOM_BM71_SPP_RxCounter;

static void _clientCallBack(CUSTOM_BM71_EVENT event, uint32_t param);

//*****************************************************************************
/* Driver Hardware instance objects.

  Summary:
    Defines the hardware instances objects for the BM71 Bluetooth module

  Description:
    This data type defines the hardware instance objects that are available for
    BM71 Bluetooth module, so as to capture the hardware state of the instance.
 */

CUSTOM_BM71_OBJ    gBm71Obj;


// *****************************************************************************
/* Driver Client instance objects.

  Summary:
    Defines the client instances objects

  Description:
    This data type defines the client instance objects that are available for
    BM71 Bluetooth module, so as to capture the client state of the instance.
    It uses the configuration of maximum number of clients which can get
    registered per hardware instance.
 */

CUSTOM_BM71_CLIENT_OBJ gBm71ObjClientObj[CUSTOM_BM71_CLIENTS_NUMBER];


// *****************************************************************************
/* Driver common data object

  Summary:
    Defines the common data object

  Description:
    This object maintains data that is required by all BM71 Bluetooth
   driver instances

  Remarks:
    None
 */

CUSTOM_BM71_COMMON_DATA_OBJ gBm71ObjCommonDataObj;

// *****************************************************************************
// *****************************************************************************
// Section: BM71 Bluetooth Driver System Routine Implementations
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function CUSTOM_BM71_Initialize:

        void CUSTOM_BM71_Initialize( void );

  Summary:
    Initializes hardware and data for the instance of the BM71 Bluetooth module

  Description:
    This routine initializes the BM71 driver instance for the specified driver
    index, making it ready for clients to open and use it. The initialization
    data is specified by the init parameter. The initialization may fail if the
    number of driver objects allocated are insufficient or if the specified
    driver instance is already initialized.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    This routine must be called before any other BM71 driver routine is called.
    This routine should only be called once during system initialization.
    This routine will never block for hardware access.

 */

void CUSTOM_BM71_Initialize()
{
    /* Populate the driver object with the required data */
    gBm71Obj.inUse                           = true;
    gBm71Obj.status                          = SYS_STATUS_UNINITIALIZED;
    gBm71Obj.numClients                      = 0;
    gBm71Obj.isInInterruptContext            = false;

    gBm71Obj.linkIndex = 0;
    CUSTOM_BM71_UART_Initialize();
    gBm71Obj.state = CUSTOM_BM71_STATE_INITIALIZE_START;

    CUSTOM_BM71_CommandDecodeInit();
    CUSTOM_BM71_CommandSendInit();

    _initSPP();

    /* Create the hardware instance mutex. */
     if(OSAL_MUTEX_Create(&(gBm71Obj.mutexDriverInstance)) != OSAL_RESULT_TRUE)
     {
        return;
     }

    /* Check if the global mutexes have been created. If not
       then create these. */
     if(!gBm71ObjCommonDataObj.membersAreInitialized)
     {
         /* This means that mutexes where not created. Create them. */
         if((OSAL_MUTEX_Create(&(gBm71ObjCommonDataObj.mutexClientObjects)) != OSAL_RESULT_TRUE))
         {
            return;
         }
         /* Set this flag so that global mutexes get allocated only once */
         gBm71ObjCommonDataObj.membersAreInitialized = true;
     }

    gBm71Obj.status = SYS_STATUS_READY;      // okay to open driver
} /* CUSTOM_BM71_Initialize */

// *****************************************************************************
/* Function CUSTOM_BM71_Tasks:

        void  CUSTOM_BM71_Tasks( void );

  Summary:
    Maintains the driver's control and data interface state machine.

  Description:
    This routine is used to maintain the driver's internal control and data
    interface state machine and implement its control and data interface
    implementations.

    This function should be called from the SYS_Tasks() function.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    This routine is not normally called directly by an application.  Instead it
    is called by the system's Tasks routine (SYS_Tasks).
*/

void CUSTOM_BM71_Tasks( void )
{
    if((false == gBm71Obj.inUse))
    {
        /* This instance of the driver is not initialized. Don't do anything */
        return;
    }

    _BM71ControlTasks();
}

// *****************************************************************************
/* Function CUSTOM_BM71_Status:

        SYS_STATUS CUSTOM_BM71_Status( void );

  Summary:
    Gets the current status of the BM71 Bluetooth driver module.

  Description:
    This routine provides the current status of the BM71 Bluetooth driver module,
    passed back as type SYS_STATUS.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    Driver status, encoded as type SYS_STATUS enum:

    SYS_STATUS_DEINITIALIZED  - Indicates that the driver has been
                                deinitialized
    SYS_STATUS_READY          - Indicates that any previous module operation
                                for the specified module has completed
    SYS_STATUS_BUSY           - Indicates that a previous module operation for
                                the specified module has not yet completed
    SYS_STATUS_ERROR          - Indicates that the specified module is in an
                                error state

  Remarks:
    A driver can opened only when its status is SYS_STATUS_READY.
*/

SYS_STATUS CUSTOM_BM71_Status( void )
{
    /* Return the status of the driver object */
    return gBm71Obj.status;
} /* CUSTOM_BM71_Status */

// *****************************************************************************
/* Function CUSTOM_BM71_GetPowerStatus:

        CUSTOM_BM71_LIB_STATUS CUSTOM_BM71_GetPowerStatus( void );

  Summary:
    Gets the current status of the BM71 Bluetooth driver module (BM71-specific).

  Description:
    This routine provides the current status (power on/off/ready) of the BM71
    Bluetooth driver module passed back as type CUSTOM_BM71_LIB_STATUS enum.

  Precondition:
    CUSTOM_BM71_Initialize must have been called to initialize the driver instance.

  Parameters:
    None.

  Returns:
    Driver status, encoded as type CUSTOM_BM71_LIB_STATUS enum.

  Remarks:
    None.
*/

// *****************************************************************************
/* Function CUSTOM_BM71_GetPowerStatus:

        CUSTOM_BM71_LIB_STATUS CUSTOM_BM71_GetPowerStatus( void );

  Summary:
    Gets the current status of the BM64 Bluetooth driver module (BM71-specific).

  Description:
    This routine provides the current status (power on/off/ready) of the BM71
    Bluetooth driver module passed back as type CUSTOM_BM71_LIB_STATUS enum.

  Precondition:
    CUSTOM_BM71_Initialize must have been called to initialize the driver instance.

  Parameters:
    None.

  Returns:
    Driver status, encoded as type CUSTOM_BM71_LIB_STATUS enum.

  Remarks:
    None.
*/

CUSTOM_BM71_LIB_STATUS CUSTOM_BM71_GetPowerStatus( void )
{
    if (gBm71Obj.state < CUSTOM_BM71_STATE_BLE_ADV_WAIT)
        return CUSTOM_BM71_STATUS_ON;
    else
        return CUSTOM_BM71_STATUS_READY;
}

// *****************************************************************************
/* Function CUSTOM_BM71_TaskReq:

        void CUSTOM_BM71_TaskReq(CUSTOM_BM71_REQUEST request);

  Summary:
    Make a power on/power off task request.

  Description:
    Make a power on/power off task request using the CUSTOM_BM71_REQUEST enum.

  Precondition:
    CUSTOM_BM71_Initialize must have been called to initialize the driver instance.

  Parameters:
    request        - power on/off request of type CUSTOM_BM71_REQUEST

  Returns:
    None.

  Remarks:
    None.
*/

//void CUSTOM_BM71_TaskReq(CUSTOM_BM71_REQUEST request)
//{
//    gBm71Obj.request = request;
//}

// *****************************************************************************
// *****************************************************************************
// Section: BM71 Bluetooth Driver Client Routines
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
/* Function CUSTOM_BM71_Open:

        BM71_HANDLE CUSTOM_BM71_Open(const BM71_IO_INTENT ioIntent,
                          const CUSTOM_BM71_PROTOCOL protocol);

  Summary:
    Opens the specified BM71 driver instance and returns a handle to it

  Description:
    This routine opens the specified BM71 Bluetooth driver instance and provides
    a handle that must be provided to all other client-level operations to
    identify the caller and the instance of the driver. The ioIntent
    parameter defines how the client interacts with this driver instance.

    Only BM71_IO_INTENT_READ is a valid ioIntent option as the BM71 Bluetooth
    driver audio stream is read-only.

    Specifying a BM71_IO_INTENT_EXCLUSIVE will cause the driver to provide
    exclusive access to this client. The driver cannot be opened by any
    other client.

  Precondition:
    CUSTOM_BM71_Initialize must have been called to initialize the driver instance.

  Parameters:
    ioIntent        - valid handle to an opened BM71 device driver unique to client
    protocol        - specifies which protocol(s) the client intends to use
                      with this driver.  One of the various CUSTOM_BM71_PROTOCOL
                      enum values, including CUSTOM_BM71_PROTOCOL_ALL.

  Returns:
    valid handle to an opened BM71 device driver unique to client

  Remarks:
    The handle returned is valid until the CUSTOM_BM71_Close routine is called.
    This routine will never block waiting for hardware.  If the requested intent
    flags are not supported, the routine will return BM71_HANDLE_INVALID.  This
    function is thread safe in a RTOS application. It should not be called in an
    ISR.

    Currently only one client is allowed at a time.
*/

BM71_HANDLE CUSTOM_BM71_Open
(
    const BM71_IO_INTENT ioIntent,
    const CUSTOM_BM71_PROTOCOL protocol
)
{
    CUSTOM_BM71_CLIENT_OBJ *hClient;
    uint32_t iClient;

    if (gBm71Obj.status == SYS_STATUS_BUSY)
    {
        return BM71_HANDLE_INVALID;
    }

    if (gBm71Obj.status != SYS_STATUS_READY)
    {
        /* The BM71  module should be ready */
        SYS_DEBUG(0, "Was the driver initialized? \r\n");
        return BM71_HANDLE_INVALID;
    }

    if ((gBm71Obj.numClients > 0) && (true == gBm71Obj.isExclusive))
    {
        /* Driver already opened in exclusive mode. Cannot open a new client. */
        SYS_DEBUG(0, "Cannot open a new client in exclusive mode \r\n");
        return BM71_HANDLE_INVALID;
    }

    if ((gBm71Obj.numClients > 0) &&
        (BM71_IO_INTENT_EXCLUSIVE == (ioIntent & BM71_IO_INTENT_EXCLUSIVE)))
    {
        /*  A client Instance of driver is open.
            Cannot open the new client in exclusive mode */
            SYS_DEBUG(0, "Cannot open a new client in exclusive mode \r\n");
            return BM71_HANDLE_INVALID;
    }

    iClient = 0;
    hClient = (CUSTOM_BM71_CLIENT_OBJ *)&gBm71ObjClientObj[iClient];

    /* Grab client object mutex here */
    if(OSAL_MUTEX_Lock(&(gBm71ObjCommonDataObj.mutexClientObjects), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
    {
        /* Setup client operations */
        /* Find available slot in array of client objects */
        for (; iClient < CUSTOM_BM71_CLIENTS_NUMBER; iClient++)
        {
            if (false == hClient->inUse)
            {
                /* Remember which BM71 driver instance owns me */
                hClient->inUse  = true;
                hClient->pEventCallBack = NULL;
                hClient->protocol = protocol;
                gBm71Obj.numClients++;

                /* We have found a client object
                 * Release the mutex and return with
                 * the driver handle */
                /* An operation mode is needed */
                if((OSAL_MUTEX_Unlock(&(gBm71ObjCommonDataObj.mutexClientObjects))) != OSAL_RESULT_TRUE)
                {
                    SYS_DEBUG(0, "Unable to unlock open routine mutex \r\n");
                    return BM71_HANDLE_INVALID;
                }
                /* Return the client object */
                return (BM71_HANDLE) hClient;
            }
            hClient++;
        }
        /* Could not find a client object. Release the mutex and
         * return with an invalid handle. */
        if((OSAL_MUTEX_Unlock(&(gBm71ObjCommonDataObj.mutexClientObjects))) != OSAL_RESULT_TRUE)
        {
            SYS_DEBUG(0, "Unable to unlock open routine mutex \r\n");
        }
    }
    return BM71_HANDLE_INVALID;
}

// *****************************************************************************
/* Function CUSTOM_BM71_Close:

        void CUSTOM_BM71_Close(BM71_HANDLE handle);

  Summary:
    Closes an opened-instance of the BM71 Bluetooth driver

  Description:
    This routine closes an opened-instance of the BM71 driver, invalidating the
    handle. Any buffers in the driver queue that were submitted by this client
    will be removed.  After calling this routine, the handle passed in "handle"
    must not be used with any of the remaining driver routines.  A new handle must
    be obtained by calling CUSTOM_BM71_Open before the caller may use the driver
    again

  Precondition:
    CUSTOM_BM71_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle          - valid handle to an opened BM71 device driver unique to client

  Returns:
    None.

  Remarks:
    Usually there is no need for the driver client to verify that the Close
    operation has completed.  The driver will abort any ongoing operations
    when this routine is called.
*/
void CUSTOM_BM71_Close( const BM71_HANDLE handle)
{
    CUSTOM_BM71_CLIENT_OBJ *clientObj;

    if(handle == BM71_HANDLE_INVALID || (BM71_HANDLE)NULL == handle)
    {
        SYS_DEBUG(0, "Invalid Driver Handle \r\n");
        return;
    }

    clientObj = (CUSTOM_BM71_CLIENT_OBJ *) handle;
    if (false == clientObj->inUse)
    {
        SYS_DEBUG(0, "Invalid Driver Handle \r\n");
        return;
    }

    /* De-allocate the object */
    clientObj->inUse = false;
    /* Reduce the number of clients */
    gBm71Obj.numClients--;
} /* CUSTOM_BM71_Close */

// *****************************************************************************
/* Function CUSTOM_BM71_EventHandlerSet:

        void CUSTOM_BM71_EventHandlerSet(BM71_HANDLE handle,
            const CUSTOM_BM71_EVENT_HANDLER eventHandler,
            const uintptr_t contextHandle);

  Summary:
    This function allows a client to identify a event handling function
    for the driver to call back.

  Description:
    This function allows a client to identify a command event handling function
    for the driver to call back when an event has been received from the BM71.

   The context parameter contains a handle to the client context,
    provided at the time the event handling function is registered using the
    CUSTOM_BM71_BufferEventHandlerSet function.  This context handle value is
    passed back to the client as the "context" parameter.  It can be any value
    necessary to identify the client context or instance (such as a pointer to
    the client's data) instance of the client.*

    The event handler should be set before the client performs any "BM71 Bluetooth
    Specific Client Routines" operations that could generate events.
    The event handler once set, persists until the client closes the driver or
    sets another event handler (which could be a "NULL" pointer to indicate no callback).

  Precondition:
    CUSTOM_BM71_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle          - valid handle to an opened BM71 device driver unique to client
    eventHandler    - pointer to a function to be called back (prototype defined
                      by CUSTOM_BM71_EVENT_HANDLER)
    contextHandle   - handle to the client context

  Returns:
    None.

  Remarks:
    If the client does not want to be notified when an event has occurred, it
    does not need to register a callback.
*/

void CUSTOM_BM71_EventHandlerSet
(
    BM71_HANDLE handle,
    const CUSTOM_BM71_EVENT_HANDLER eventHandler,   // TEMP!!
    const uintptr_t contextHandle
)
{
    CUSTOM_BM71_CLIENT_OBJ *clientObj;

    if((BM71_HANDLE_INVALID == handle) || (0 == handle))
    {
        /* This means the handle is invalid */
        SYS_DEBUG(0, "Handle is invalid \r\n");
        return;
    }

    clientObj = (CUSTOM_BM71_CLIENT_OBJ *) handle;
    /* Assing the event handler and the context */
    if(false == clientObj->inUse)
    {
        SYS_DEBUG(0, "Invalid driver handle \r\n");
        return;
    }

    clientObj->pEventCallBack = eventHandler;
    clientObj->hClientArg = contextHandle;
}
// *****************************************************************************
/* Function CUSTOM_BM71_ClearBLEData:

        void CUSTOM_BM71_ClearBLEData( const BM71_HANDLE handle );

  Summary:
    Clear the BLE receive buffer.

  Description:
    Clears the buffer used when receiving characters via the
    CUSTOM_BM71_ReadByteFromBLE and CUSTOM_BM71_ReadDataFromBLE calls.

  Precondition:
    CUSTOM_BM71_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle      - valid handle to an opened BM71 device driver unique to client

  Returns:
    None.

  Remarks:
    None.
*/

void CUSTOM_BM71_ClearBLEData( const BM71_HANDLE handle )
{
    CUSTOM_BM71_SPPBuffClear();
}

// *****************************************************************************
/* Function CUSTOM_BM71_ReadByteFromBLE:

        bool CUSTOM_BM71_ReadByteFromBLE(const BM71_HANDLE handle, uint8_t* byte);

  Summary:
    Read a byte over BLE.

  Description:
    Read one byte over BLE using the BM71's "Transparent Service" feature.

  Precondition:
    CUSTOM_BM71_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle      - valid handle to an opened BM71 device driver unique to client
    byte        - pointer to a uint8_t to receive the data

  Returns:
    bool        - true if a byte was returned, false if receive buffer empty

  Remarks:
    None.
*/

bool CUSTOM_BM71_ReadByteFromBLE(const BM71_HANDLE handle, uint8_t* byte)
{
    if(CUSTOM_BM71_SPP_RxCounter)
    {
        *byte = CUSTOM_BM71_SPP_RxFifo[CUSTOM_BM71_SPP_RxFifoHead];
        CUSTOM_BM71_SPP_RxCounter--;
        if(CUSTOM_BM71_SPP_RxFifoHead < CUSTOM_BM71_SPP_RxFifoSize-1)
            CUSTOM_BM71_SPP_RxFifoHead++;
        else
            CUSTOM_BM71_SPP_RxFifoHead = 0;
        return true;
    }
    else
        return false;
}

// *****************************************************************************
/* Function CUSTOM_BM71_ReadDataFromBLE:

        bool CUSTOM_BM71_ReadDataFromBLE(const BM71_HANDLE handle, uint8_t* bytes,
                uint16_t size );

  Summary:
    Read data over BLE.

  Description:
    Read data over BLE using the BM71's "Transparent Service" feature.

  Precondition:
    CUSTOM_BM71_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle      - valid handle to an opened BM71 device driver unique to client
    bytes       - pointer to a uint8_t buffer at least size bytes long
    size        - length of buffer (including

  Returns:
    bool        - true if data was returned, false if receive buffer empty

  Remarks:
    No more than size bytes will be returned, even if more are available.
*/

bool CUSTOM_BM71_ReadDataFromBLE(const BM71_HANDLE handle, uint8_t* byte, uint16_t size )
{
    if(CUSTOM_BM71_SPP_RxCounter)
    {
        while (CUSTOM_BM71_SPP_RxCounter)
        {
            *byte++ = CUSTOM_BM71_SPP_RxFifo[CUSTOM_BM71_SPP_RxFifoHead];
            *byte= '\0';
            CUSTOM_BM71_SPP_RxCounter--;
            if(CUSTOM_BM71_SPP_RxFifoHead < CUSTOM_BM71_SPP_RxFifoSize-1)
                CUSTOM_BM71_SPP_RxFifoHead++;
            else
                CUSTOM_BM71_SPP_RxFifoHead = 0;
            if (--size==0)
            {
                break;      // reached end of buffer
            }
        }
        return true;
    }
    else
        return false;
}

// *****************************************************************************
/* Function CUSTOM_BM71_SendByteOverBLE:

        void CUSTOM_BM71_SendByteOverBLE(const BM71_HANDLE handle, uint8_t byte);

  Summary:
    Send a byte over BLE.

  Description:
    Send one byte over BLE using the BM71's "Transparent Service" feature.

  Precondition:
    CUSTOM_BM71_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle      - valid handle to an opened BM71 device driver unique to client
    byte        - uint8_t of data to be sent

  Returns:
    None.

  Remarks:
    None.
*/

void CUSTOM_BM71_SendByteOverBLE(const BM71_HANDLE handle, uint8_t byte)
{
    CUSTOM_BM71_SendSPPData(&byte, 1, gBm71Obj.linkIndex);
}

// *****************************************************************************
/* Function CUSTOM_BM71_SendDataOverBLE:

        void CUSTOM_BM71_SendDataOverBLE(const BM71_HANDLE handle, uint8_t* bytes,
                uint16_t size);

  Summary:
    Send data over BLE.

  Description:
    Send data over BLE using the BM71's "Transparent Service" feature.

  Precondition:
    CUSTOM_BM71_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle      - valid handle to an opened BM71 device driver unique to client
    bytes       - pointer to a uint8_t buffer at least size bytes long
    size        - length of buffer (including

  Returns:
    None.

  Remarks:
    None.
*/

void CUSTOM_BM71_SendDataOverBLE(const BM71_HANDLE handle, uint8_t* addr, uint16_t size)
{
    CUSTOM_BM71_SendSPPData(addr, size, gBm71Obj.linkIndex);
}

// *****************************************************************************
// *****************************************************************************
// local routines
// *****************************************************************************
// *****************************************************************************
/*------------------------------------------------------------*/
/*
 * _clientCallBack
*/
/*------------------------------------------------------------*/

static void _clientCallBack(CUSTOM_BM71_EVENT event, uint32_t param)
{
    CUSTOM_BM71_CLIENT_OBJ *hClient;
    uint32_t iClient;

    iClient = 0;
    hClient = (CUSTOM_BM71_CLIENT_OBJ *)&gBm71ObjClientObj[iClient];

    for (; iClient < CUSTOM_BM71_CLIENTS_NUMBER; iClient++)
    {
        if (true == hClient->inUse)
        {
            if (NULL != hClient->pEventCallBack)
            {
                hClient->pEventCallBack(event, param, hClient->hClientArg);
            }
        }
        hClient++;
    }
}

//================================================
// Init
//================================================

void CUSTOM_BM71_Timer_1ms( uintptr_t context)
{
    //drive BM71 Module Initialization
    CUSTOM_BM71_Timer1MS_event();

    //UART TX send to BM71
    CUSTOM_BM71_CommandSend1MS_event();
}

//================================================
// Task
//================================================

uint32_t oldstate = -1;

static void _BM71ControlTasks(void)
{
    CUSTOM_BM71_UART_Tasks();       // take care of UART first

    if (gBm71Obj.state != oldstate)
    {
        printf("BM71 state: %02d\r\n",gBm71Obj.state);
        oldstate = gBm71Obj.state;
    }

    switch (gBm71Obj.state) {
        case CUSTOM_BM71_STATE_INITIALIZE_START:
            /* Open the timer Driver */
            CUSTOM_BM71_tmrHandle = SYS_TIME_CallbackRegisterMS(CUSTOM_BM71_Timer_1ms,
                    (uintptr_t)0, 1/*ms*/, SYS_TIME_PERIODIC);

            if (SYS_TIME_HANDLE_INVALID == CUSTOM_BM71_tmrHandle)
            {
                printf("can't open SYS_TIME driver");
            }

            _timer1ms = 10;
            CUSTOM_BM71_RESET_SetLow();                        // Reset is active low

            gBm71Obj.state = CUSTOM_BM71_STATE_INIT_RESET_HIGH;
            break;

        case CUSTOM_BM71_STATE_INIT_RESET_HIGH:
            if (!_timer1ms)  // check 10ms times up
            {
                CUSTOM_BM71_RESET_SetHigh();
                CUSTOM_BM71_CommandDecodeInit();
                _timer1ms = 100; //wait 100ms
                gBm71Obj.state = CUSTOM_BM71_STATE_INIT_RESET_HIGH_WAIT;
            }
            break;

        case CUSTOM_BM71_STATE_INIT_RESET_HIGH_WAIT:
            if (!_timer1ms) // check 200ms times up
            {
                BM71_RX_IND_Clear();
                _timer1ms = 5; //wait 100ms
                gBm71Obj.state = CUSTOM_BM71_STATE_INIT_BLE_ADV_START;
            }
            break;

        case CUSTOM_BM71_STATE_INIT_BLE_ADV_START:
            if (!_timer1ms) // check 200ms times up
            {
                CUSTOM_BM71_BLE_WriteAdvertisingData();
                _timer1ms = 100; //wait 100ms
                gBm71Obj.state = CUSTOM_BM71_STATE_SET_BLE_ADV_PARAM;
            }
            break;

        case CUSTOM_BM71_STATE_SET_BLE_ADV_PARAM:
            if (!_timer1ms) // check 100ms times up
            {
                CUSTOM_BM71_BLE_SetAdvertisingParams();
                _timer1ms = 100; //wait 100ms
                gBm71Obj.state = CUSTOM_BM71_STATE_SET_BLE_ADV_ENABLE;
            }
            break;

        case CUSTOM_BM71_STATE_SET_BLE_ADV_ENABLE:
            if (!_timer1ms) // check 100ms times up
            {
                CUSTOM_BM71_BLE_EnabAdvertising(true);
                _timer1ms = 100; //wait 100ms
                gBm71Obj.state = CUSTOM_BM71_STATE_BLE_ADV_WAIT;
            }
            break;

        case CUSTOM_BM71_STATE_BLE_ADV_WAIT:           // wait for connect
            break;

        case CUSTOM_BM71_STATE_BLE_CONNECTED:
            if (!_timer1ms) //check 100ms times up
            {
                CUSTOM_BM71_BLE_EnabTransparentMode(true);
                _timer1ms = 100; //wait 100ms
                gBm71Obj.state = CUSTOM_BM71_STATE_BLE_TRANSPARENT_MODE;
            }
            break;

        case CUSTOM_BM71_STATE_BLE_TRANSPARENT_MODE:   // wait for disconnect
            break;

        case CUSTOM_BM71_STATE_BLE_DISCONNECTED:
            if (!_timer1ms) //check 100ms times up
            {
                CUSTOM_BM71_BLE_EnabTransparentMode(false);
                _timer1ms = 500; //wait 100ms
                gBm71Obj.state = CUSTOM_BM71_STATE_BLE_DISCONNECTED_WAIT;
            }
            break;

        case CUSTOM_BM71_STATE_BLE_DISCONNECTED_WAIT:
            if (!_timer1ms) //check 100ms times up
            {
                gBm71Obj.linkIndex = 0;
                _timer1ms = 200; //wait 100ms
                gBm71Obj.state = CUSTOM_BM71_STATE_INIT_BLE_ADV_START;
            }
            break;

        default:
            break;

    }

    if (gBm71Obj.state >= CUSTOM_BM71_STATE_INIT_BLE_ADV_START)
    {
        CUSTOM_BM71_CommandDecodeMain();
        CUSTOM_BM71_CommandSendTask();
    }
}

//================================================
//1ms Timer
//================================================
extern uint8_t CUSTOM_BM71_txByteQueued;
void CUSTOM_BM71_Timer1MS_event( void )
{
    if(_timer1ms)
        --_timer1ms;

    if ((CUSTOM_BM71_txByteQueued>0) && ((CUSTOM_BM71_txByteQueued&1)==0))
    {
        CUSTOM_BM71_txByteQueued--;       // if > 0 and even, decrement
    }
}

void CUSTOM_BM71_UpdateClientStatus(CUSTOM_BM71_LIB_STATUS status)
{
    _clientCallBack(CUSTOM_BM71_EVENT_BLE_STATUS_CHANGED, status);
}

/*-----------------------------------------------------------------------------*/
/*below functions are for SPP process*/
/*-----------------------------------------------------------------------------*/

/*------------------------------------------------------------*/
static void _initSPP(void)
    {
    CUSTOM_BM71_SPP_RxFifoHead = 0;
    CUSTOM_BM71_SPP_RxFifoTail = 0;
    CUSTOM_BM71_SPP_RxCounter = 0;
}

/*-----------------------------------------------------------------------------*/
void CUSTOM_BM71_SPPBuffClear( void )
{
    uint16_t i;
    CUSTOM_BM71_SPP_RxFifoHead = 0;
    CUSTOM_BM71_SPP_RxFifoTail = 0;
    CUSTOM_BM71_SPP_RxCounter = 0;
    for(i = 0; i< sizeof(CUSTOM_BM71_SPP_RxFifo); i++)
        CUSTOM_BM71_SPP_RxFifo[i] = 0;
}

/*-----------------------------------------------------------------------------*/
bool CUSTOM_BM71_AddBytesToSPPBuff(uint8_t* data, uint8_t size)        //TRUE: data added ok, FALSE: data added fail, buffer is overflow
{
    uint8_t i;
    for(i=0; i<size; i++)
    {
        if(CUSTOM_BM71_SPP_RxCounter < CUSTOM_BM71_SPP_RxFifoSize)
        {
            CUSTOM_BM71_SPP_RxFifo[CUSTOM_BM71_SPP_RxFifoTail] = *data++;
            CUSTOM_BM71_SPP_RxCounter++;
            if(CUSTOM_BM71_SPP_RxFifoTail < CUSTOM_BM71_SPP_RxFifoSize-1)
                CUSTOM_BM71_SPP_RxFifoTail++;
            else
                CUSTOM_BM71_SPP_RxFifoTail = 0;
        }
        else
        {
            return false;
        }
    }

    _clientCallBack(CUSTOM_BM71_EVENT_BLESPP_MSG_RECEIVED, (uint8_t)size);
    return true;
}
