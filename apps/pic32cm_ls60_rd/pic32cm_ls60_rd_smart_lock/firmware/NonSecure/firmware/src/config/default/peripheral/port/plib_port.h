/*******************************************************************************
  PORT PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_port.h

  Summary:
    PORT PLIB Header File

  Description:
    This file provides an interface to control and interact with PORT-I/O
    Pin controller module.

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef PLIB_PORT_H
#define PLIB_PORT_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "device.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
    extern "C" {
#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data types and constants
// *****************************************************************************
// *****************************************************************************

/*** Macros for TOUCH_ENABLE pin ***/
#define TOUCH_ENABLE_Set()               (PORT_REGS->GROUP[0].PORT_OUTSET = 1 << 0)
#define TOUCH_ENABLE_Clear()             (PORT_REGS->GROUP[0].PORT_OUTCLR = 1 << 0)
#define TOUCH_ENABLE_Toggle()            (PORT_REGS->GROUP[0].PORT_OUTTGL = 1 << 0)
#define TOUCH_ENABLE_OutputEnable()      (PORT_REGS->GROUP[0].PORT_DIRSET = 1 << 0)
#define TOUCH_ENABLE_InputEnable()       (PORT_REGS->GROUP[0].PORT_DIRCLR = 1 << 0)
#define TOUCH_ENABLE_Get()               (((PORT_REGS->GROUP[0].PORT_IN >> 0)) & 0x01)
#define TOUCH_ENABLE_PIN                  PORT_PIN_PA00

/*** Macros for ENABLE_6V pin ***/
#define ENABLE_6V_Set()               (PORT_REGS->GROUP[0].PORT_OUTSET = 1 << 1)
#define ENABLE_6V_Clear()             (PORT_REGS->GROUP[0].PORT_OUTCLR = 1 << 1)
#define ENABLE_6V_Toggle()            (PORT_REGS->GROUP[0].PORT_OUTTGL = 1 << 1)
#define ENABLE_6V_OutputEnable()      (PORT_REGS->GROUP[0].PORT_DIRSET = 1 << 1)
#define ENABLE_6V_InputEnable()       (PORT_REGS->GROUP[0].PORT_DIRCLR = 1 << 1)
#define ENABLE_6V_Get()               (((PORT_REGS->GROUP[0].PORT_IN >> 1)) & 0x01)
#define ENABLE_6V_PIN                  PORT_PIN_PA01

/*** Macros for  QT_LED_COL2 pin ***/
#define  QT_LED_COL2_Set()               (PORT_REGS->GROUP[0].PORT_OUTSET = 1 << 4)
#define  QT_LED_COL2_Clear()             (PORT_REGS->GROUP[0].PORT_OUTCLR = 1 << 4)
#define  QT_LED_COL2_Toggle()            (PORT_REGS->GROUP[0].PORT_OUTTGL = 1 << 4)
#define  QT_LED_COL2_OutputEnable()      (PORT_REGS->GROUP[0].PORT_DIRSET = 1 << 4)
#define  QT_LED_COL2_InputEnable()       (PORT_REGS->GROUP[0].PORT_DIRCLR = 1 << 4)
#define  QT_LED_COL2_Get()               (((PORT_REGS->GROUP[0].PORT_IN >> 4)) & 0x01)
#define  QT_LED_COL2_PIN                  PORT_PIN_PA04

/*** Macros for  QT_LED_COL0 pin ***/
#define  QT_LED_COL0_Set()               (PORT_REGS->GROUP[0].PORT_OUTSET = 1 << 5)
#define  QT_LED_COL0_Clear()             (PORT_REGS->GROUP[0].PORT_OUTCLR = 1 << 5)
#define  QT_LED_COL0_Toggle()            (PORT_REGS->GROUP[0].PORT_OUTTGL = 1 << 5)
#define  QT_LED_COL0_OutputEnable()      (PORT_REGS->GROUP[0].PORT_DIRSET = 1 << 5)
#define  QT_LED_COL0_InputEnable()       (PORT_REGS->GROUP[0].PORT_DIRCLR = 1 << 5)
#define  QT_LED_COL0_Get()               (((PORT_REGS->GROUP[0].PORT_IN >> 5)) & 0x01)
#define  QT_LED_COL0_PIN                  PORT_PIN_PA05

/*** Macros for  QT_LED_COL1 pin ***/
#define  QT_LED_COL1_Set()               (PORT_REGS->GROUP[0].PORT_OUTSET = 1 << 6)
#define  QT_LED_COL1_Clear()             (PORT_REGS->GROUP[0].PORT_OUTCLR = 1 << 6)
#define  QT_LED_COL1_Toggle()            (PORT_REGS->GROUP[0].PORT_OUTTGL = 1 << 6)
#define  QT_LED_COL1_OutputEnable()      (PORT_REGS->GROUP[0].PORT_DIRSET = 1 << 6)
#define  QT_LED_COL1_InputEnable()       (PORT_REGS->GROUP[0].PORT_DIRCLR = 1 << 6)
#define  QT_LED_COL1_Get()               (((PORT_REGS->GROUP[0].PORT_IN >> 6)) & 0x01)
#define  QT_LED_COL1_PIN                  PORT_PIN_PA06

/*** Macros for  QT_LED_COL3 pin ***/
#define  QT_LED_COL3_Set()               (PORT_REGS->GROUP[0].PORT_OUTSET = 1 << 7)
#define  QT_LED_COL3_Clear()             (PORT_REGS->GROUP[0].PORT_OUTCLR = 1 << 7)
#define  QT_LED_COL3_Toggle()            (PORT_REGS->GROUP[0].PORT_OUTTGL = 1 << 7)
#define  QT_LED_COL3_OutputEnable()      (PORT_REGS->GROUP[0].PORT_DIRSET = 1 << 7)
#define  QT_LED_COL3_InputEnable()       (PORT_REGS->GROUP[0].PORT_DIRCLR = 1 << 7)
#define  QT_LED_COL3_Get()               (((PORT_REGS->GROUP[0].PORT_IN >> 7)) & 0x01)
#define  QT_LED_COL3_PIN                  PORT_PIN_PA07

/*** Macros for QT_LED_ROW0 pin ***/
#define QT_LED_ROW0_Set()               (PORT_REGS->GROUP[1].PORT_OUTSET = 1 << 12)
#define QT_LED_ROW0_Clear()             (PORT_REGS->GROUP[1].PORT_OUTCLR = 1 << 12)
#define QT_LED_ROW0_Toggle()            (PORT_REGS->GROUP[1].PORT_OUTTGL = 1 << 12)
#define QT_LED_ROW0_OutputEnable()      (PORT_REGS->GROUP[1].PORT_DIRSET = 1 << 12)
#define QT_LED_ROW0_InputEnable()       (PORT_REGS->GROUP[1].PORT_DIRCLR = 1 << 12)
#define QT_LED_ROW0_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 12)) & 0x01)
#define QT_LED_ROW0_PIN                  PORT_PIN_PB12

/*** Macros for QT_LED_ROW1 pin ***/
#define QT_LED_ROW1_Set()               (PORT_REGS->GROUP[1].PORT_OUTSET = 1 << 13)
#define QT_LED_ROW1_Clear()             (PORT_REGS->GROUP[1].PORT_OUTCLR = 1 << 13)
#define QT_LED_ROW1_Toggle()            (PORT_REGS->GROUP[1].PORT_OUTTGL = 1 << 13)
#define QT_LED_ROW1_OutputEnable()      (PORT_REGS->GROUP[1].PORT_DIRSET = 1 << 13)
#define QT_LED_ROW1_InputEnable()       (PORT_REGS->GROUP[1].PORT_DIRCLR = 1 << 13)
#define QT_LED_ROW1_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 13)) & 0x01)
#define QT_LED_ROW1_PIN                  PORT_PIN_PB13

/*** Macros for QT_LED_ROW2 pin ***/
#define QT_LED_ROW2_Set()               (PORT_REGS->GROUP[1].PORT_OUTSET = 1 << 14)
#define QT_LED_ROW2_Clear()             (PORT_REGS->GROUP[1].PORT_OUTCLR = 1 << 14)
#define QT_LED_ROW2_Toggle()            (PORT_REGS->GROUP[1].PORT_OUTTGL = 1 << 14)
#define QT_LED_ROW2_OutputEnable()      (PORT_REGS->GROUP[1].PORT_DIRSET = 1 << 14)
#define QT_LED_ROW2_InputEnable()       (PORT_REGS->GROUP[1].PORT_DIRCLR = 1 << 14)
#define QT_LED_ROW2_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 14)) & 0x01)
#define QT_LED_ROW2_PIN                  PORT_PIN_PB14

/*** Macros for QT_LED_ROW3 pin ***/
#define QT_LED_ROW3_Set()               (PORT_REGS->GROUP[1].PORT_OUTSET = 1 << 15)
#define QT_LED_ROW3_Clear()             (PORT_REGS->GROUP[1].PORT_OUTCLR = 1 << 15)
#define QT_LED_ROW3_Toggle()            (PORT_REGS->GROUP[1].PORT_OUTTGL = 1 << 15)
#define QT_LED_ROW3_OutputEnable()      (PORT_REGS->GROUP[1].PORT_DIRSET = 1 << 15)
#define QT_LED_ROW3_InputEnable()       (PORT_REGS->GROUP[1].PORT_DIRCLR = 1 << 15)
#define QT_LED_ROW3_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 15)) & 0x01)
#define QT_LED_ROW3_PIN                  PORT_PIN_PB15

/*** Macros for LIMIT_SW_SIG1_PIN pin ***/
#define LIMIT_SW_SIG1_PIN_Get()               (((PORT_REGS->GROUP[0].PORT_IN >> 14)) & 0x01)
#define LIMIT_SW_SIG1_PIN_PIN                  PORT_PIN_PA14

/*** Macros for LIMIT_SW_SIG2_PIN pin ***/
#define LIMIT_SW_SIG2_PIN_Get()               (((PORT_REGS->GROUP[0].PORT_IN >> 15)) & 0x01)
#define LIMIT_SW_SIG2_PIN_PIN                  PORT_PIN_PA15

/*** Macros for BLE_COM_TX pin ***/
#define BLE_COM_TX_Get()               (((PORT_REGS->GROUP[0].PORT_IN >> 16)) & 0x01)
#define BLE_COM_TX_PIN                  PORT_PIN_PA16

/*** Macros for RX_IND_LP pin ***/
#define RX_IND_LP_Set()               (PORT_REGS->GROUP[1].PORT_OUTSET = 1 << 0)
#define RX_IND_LP_Clear()             (PORT_REGS->GROUP[1].PORT_OUTCLR = 1 << 0)
#define RX_IND_LP_Toggle()            (PORT_REGS->GROUP[1].PORT_OUTTGL = 1 << 0)
#define RX_IND_LP_OutputEnable()      (PORT_REGS->GROUP[1].PORT_DIRSET = 1 << 0)
#define RX_IND_LP_InputEnable()       (PORT_REGS->GROUP[1].PORT_DIRCLR = 1 << 0)
#define RX_IND_LP_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 0)) & 0x01)
#define RX_IND_LP_PIN                  PORT_PIN_PB00

/*** Macros for LED pin ***/
#define LED_Set()               (PORT_REGS->GROUP[1].PORT_OUTSET = 1 << 2)
#define LED_Clear()             (PORT_REGS->GROUP[1].PORT_OUTCLR = 1 << 2)
#define LED_Toggle()            (PORT_REGS->GROUP[1].PORT_OUTTGL = 1 << 2)
#define LED_OutputEnable()      (PORT_REGS->GROUP[1].PORT_DIRSET = 1 << 2)
#define LED_InputEnable()       (PORT_REGS->GROUP[1].PORT_DIRCLR = 1 << 2)
#define LED_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 2)) & 0x01)
#define LED_PIN                  PORT_PIN_PB02

// *****************************************************************************
/* PORT Group

  Summary:
    Identifies the port groups available on the device.

  Description:
    This enumeration identifies all the ports groups that are available on this
    device.

  Remarks:
    The caller should not use the constant expressions assigned to any of
    the enumeration constants as these may vary between devices.

    Port groups shown here are the ones available on the selected device. Not
    all ports groups are implemented. Refer to the device specific datasheet
    for more details. The MHC will generate this enumeration with the port
    groups that are available on the device.
*/

typedef enum
{

    /* Group 0 */
    PORT_GROUP_0 = PORT_BASE_ADDRESS + 0 * (0x80),

    /* Group 1 */
    PORT_GROUP_1 = PORT_BASE_ADDRESS + 1 * (0x80),
} PORT_GROUP;

// *****************************************************************************
/* PORT Pins

  Summary:
    Identifies the available Ports pins.

  Description:
    This enumeration identifies all the ports pins that are available on this
    device.

  Remarks:
    The caller should not use the constant expressions assigned to any of
    the enumeration constants as these may vary between devices.

    Port pins shown here are the ones available on the selected device. Not
    all ports pins within a port group are implemented. Refer to the device
    specific datasheet for more details.
*/

typedef enum
{
    /* PA00 pin */
    PORT_PIN_PA00 = 0,
    /* PA01 pin */
    PORT_PIN_PA01 = 1,
    /* PA04 pin */
    PORT_PIN_PA04 = 4,
    /* PA05 pin */
    PORT_PIN_PA05 = 5,
    /* PA06 pin */
    PORT_PIN_PA06 = 6,
    /* PA07 pin */
    PORT_PIN_PA07 = 7,
    /* PB12 pin */
    PORT_PIN_PB12 = 44,
    /* PB13 pin */
    PORT_PIN_PB13 = 45,
    /* PB14 pin */
    PORT_PIN_PB14 = 46,
    /* PB15 pin */
    PORT_PIN_PB15 = 47,
    /* PA12 pin */
    PORT_PIN_PA12 = 12,
    /* PA13 pin */
    PORT_PIN_PA13 = 13,
    /* PA14 pin */
    PORT_PIN_PA14 = 14,
    /* PA15 pin */
    PORT_PIN_PA15 = 15,
    /* PA16 pin */
    PORT_PIN_PA16 = 16,
    /* PB30 pin */
    PORT_PIN_PB30 = 62,
    /* PB31 pin */
    PORT_PIN_PB31 = 63,
    /* PB00 pin */
    PORT_PIN_PB00 = 32,
    /* PB02 pin */
    PORT_PIN_PB02 = 34,
    /* This element should not be used in any of the PORT APIs.
     * It will be used by other modules or application to denote that none of
     * the PORT Pin is used */
    PORT_PIN_NONE = -1,

} PORT_PIN;

// *****************************************************************************
// *****************************************************************************
// Section: Generated API based on pin configurations done in Pin Manager
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: PORT APIs which operates on one pin at a time
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void PORT_PinWrite(PORT_PIN pin, bool value)

  Summary:
    Writes the specified value to the selected pin.

  Description:
    This function writes/drives the "value" on the selected I/O line/pin.

  Precondition:
    The PORT_Initialize() function should have been called once.

  Parameters:
    pin - One of the IO pins from the enum PORT_PIN.
    value - value to be written on the selected pin.
            true  = set pin to high (1).
            false = clear pin to low (0).

  Returns:
    None.

  Example:
    <code>

    bool value = true;
    PORT_PinWrite(PORT_PIN_PB3, value);

    </code>

  Remarks:
    Calling this function with an input pin with the pull-up/pull-down feature
    enabled will affect the pull-up/pull-down configuration. If the value is
    false, the pull-down will be enabled. If the value is true, the pull-up will
    be enabled.
*/

static inline void PORT_PinWrite(PORT_PIN pin, bool value);

// *****************************************************************************
/* Function:
    bool PORT_PinRead(PORT_PIN pin)

  Summary:
    Read the selected pin value.

  Description:
    This function reads the present state at the selected input pin.  The
    function can also be called to read the value of an output pin if input
    sampling on the output pin is enabled in MHC. If input synchronization on
    the pin is disabled in MHC, the function will cause a 2 PORT Clock cycles
    delay. Enabling the synchronization eliminates the delay but will increase
    power consumption.

  Precondition:
    The PORT_Initialize() function should have been called. Input buffer
    (INEN bit in the Pin Configuration register) should be enabled in MHC.

  Parameters:
    pin - the port pin whose state needs to be read.

  Returns:
    true - the state at the pin is a logic high.
    false - the state at the pin is a logic low.

  Example:
    <code>

    bool value;
    value = PORT_PinRead(PORT_PIN_PB3);

    </code>

  Remarks:
    None.
*/

static inline bool PORT_PinRead(PORT_PIN pin);

// *****************************************************************************
/* Function:
    bool PORT_PinLatchRead(PORT_PIN pin)

  Summary:
    Read the value driven on the selected pin.

  Description:
    This function reads the data driven on the selected I/O line/pin. The
    function does not sample the state of the hardware pin. It only returns the
    value that is written to output register. Refer to the PORT_PinRead()
    function if the state of the output pin needs to be read.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    pin - One of the IO pins from the enum PORT_PIN.

  Returns:
    true - the present value in the output latch is a logic high.
    false - the present value in the output latch is a logic low.

  Example:
    <code>

    bool value;
    value = PORT_PinLatchRead(PORT_PIN_PB3);

    </code>

  Remarks:
    To read actual pin value, PIN_Read API should be used.
*/

static inline bool PORT_PinLatchRead(PORT_PIN pin);

// *****************************************************************************
/* Function:
    void PORT_PinToggle(PORT_PIN pin)

  Summary:
    Toggles the selected pin.

  Description:
    This function toggles/inverts the present value on the selected I/O line/pin.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    pin - One of the IO pins from the enum PORT_PIN.

  Returns:
    None.

  Example:
    <code>

    PORT_PinToggle(PORT_PIN_PB3);

    </code>

  Remarks:
    None.
*/

static inline void PORT_PinToggle(PORT_PIN pin);

// *****************************************************************************
/* Function:
    void PORT_PinSet(PORT_PIN pin)

  Summary:
    Sets the selected pin.

  Description:
    This function drives a logic 1 on the selected I/O line/pin.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    pin - One of the IO pins from the enum PORT_PIN.

  Returns:
    None.

  Example:
    <code>

    PORT_PinSet(PORT_PIN_PB3);

    </code>

  Remarks:
    None.
*/

static inline void PORT_PinSet(PORT_PIN pin);

// *****************************************************************************
/* Function:
    void PORT_PinClear(PORT_PIN pin)

  Summary:
    Clears the selected pin.

  Description:
    This function drives a logic 0 on the selected I/O line/pin.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    pin - One of the IO pins from the enum PORT_PIN.

  Returns:
    None.

  Example:
    <code>

    PORT_PinClear(PORT_PIN_PB3);

    </code>

  Remarks:
    None.
*/

static inline void PORT_PinClear(PORT_PIN pin);

// *****************************************************************************
/* Function:
    void PORT_PinInputEnable(PORT_PIN pin)

  Summary:
    Configures the selected IO pin as input.

  Description:
    This function configures the selected IO pin as input. This function
    override the MHC input output pin settings.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    pin - One of the IO pins from the enum PORT_PIN.

  Returns:
    None.

  Example:
    <code>

    PORT_PinInputEnable(PORT_PIN_PB3);

    </code>

  Remarks:
    None.
*/

static inline void PORT_PinInputEnable(PORT_PIN pin);

// *****************************************************************************
/* Function:
    void PORT_PinOutputEnable(PORT_PIN pin)

  Summary:
    Enables selected IO pin as output.

  Description:
    This function enables selected IO pin as output. Calling this function will
    override the MHC input output pin configuration.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    pin - One of the IO pins from the enum PORT_PIN.

  Returns:
    None.

  Example:
    <code>

    PORT_PinOutputEnable(PORT_PIN_PB3);

    </code>

  Remarks:
    None.
*/

static inline void PORT_PinOutputEnable(PORT_PIN pin);

// *****************************************************************************
// *****************************************************************************
// Section: PORT APIs which operates on multiple pins of a group
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    uint32_t PORT_GroupRead(PORT_GROUP group)

  Summary:
    Read all the I/O pins in the specified port group.

  Description:
    The function reads the hardware pin state of all pins in the specified group
    and returns this as a 32 bit value. Each bit in the 32 bit value represent a
    pin. For example, bit 0 in group 0 will represent pin PA0. Bit 1 will
    represent PA1 and so on. The application should only consider the value of
    the port group pins which are implemented on the device.

  Precondition:
    The PORT_Initialize() function should have been called. Input buffer
    (INEN bit in the Pin Configuration register) should be enabled in MHC.

  Parameters:
    group - One of the IO groups from the enum PORT_GROUP.

  Returns:
    A 32-bit value representing the hardware state of of all the I/O pins in the
    selected port group.

  Example:
    <code>

    uint32_t value;
    value = PORT_Read(PORT_GROUP_C);

    </code>

  Remarks:
    None.
*/

uint32_t PORT_GroupRead(PORT_GROUP group);

// *****************************************************************************
/* Function:
    uint32_t PORT_GroupLatchRead(PORT_GROUP group)

  Summary:
    Read the data driven on all the I/O pins of the selected port group.

  Description:
    The function will return a 32-bit value representing the logic levels being
    driven on the output pins within the group. The function will not sample the
    actual hardware state of the output pin. Each bit in the 32-bit return value
    will represent one of the 32 port pins within the group. The application
    should only consider the value of the pins which are available on the
    device.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    group - One of the IO groups from the enum PORT_GROUP.

  Returns:
    A 32-bit value representing the output state of of all the I/O pins in the
    selected port group.

  Example:
    <code>

    uint32_t value;
    value = PORT_GroupLatchRead(PORT_GROUP_C);

    </code>

  Remarks:
    None.
*/

uint32_t PORT_GroupLatchRead(PORT_GROUP group);

// *****************************************************************************
/* Function:
    void PORT_GroupWrite(PORT_GROUP group, uint32_t mask, uint32_t value);

  Summary:
    Write value on the masked pins of the selected port group.

  Description:
    This function writes the value contained in the value parameter to the
    port group. Port group pins which are configured for output will be updated.
    The mask parameter provides additional control on the bits in the group to
    be affected. Setting a bit to 1 in the mask will cause the corresponding
    bit in the port group to be updated. Clearing a bit in the mask will cause
    that corresponding bit in the group to stay unaffected. For example,
    setting a mask value 0xFFFFFFFF will cause all bits in the port group
    to be updated. Setting a value 0x3 will only cause port group bit 0 and
    bit 1 to be updated.

    For port pins which are not configured for output and have the pull feature
    enabled, this function will affect pull value (pull up or pull down). A bit
    value of 1 will enable the pull up. A bit value of 0 will enable the pull
    down.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    group - One of the IO groups from the enum PORT_GROUP.

    mask  - A 32 bit value in which positions of 0s and 1s decide
             which IO pins of the selected port group will be written.
             1's - Will write to corresponding IO pins.
             0's - Will remain unchanged.

    value - Value which has to be written/driven on the I/O
             lines of the selected port for which mask bits are '1'.
             Values for the corresponding mask bit '0' will be ignored.
             Refer to the function description for effect on pins
             which are not configured for output.

  Returns:
    None.

  Example:
    <code>
    // Write binary value 0011 to the pins PC3, PC2, PC1 and PC0 respectively.
    PORT_GroupWrite(PORT_GROUP_C, 0x0F, 0xF563D453);

    </code>

  Remarks:
    None.
*/

void PORT_GroupWrite(PORT_GROUP group, uint32_t mask, uint32_t value);

// *****************************************************************************
/* Function:
    void PORT_GroupSet(PORT_GROUP group, uint32_t mask)

  Summary:
    Set the selected IO pins of a group.

  Description:
    This function sets (drives a logic high) on the selected output pins of a
    group. The mask parameter control the pins to be updated. A mask bit
    position with a value 1 will cause that corresponding port pin to be set. A
    mask bit position with a value 0 will cause the corresponding port pin to
    stay un-affected.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    group - One of the IO ports from the enum PORT_GROUP.
    mask - A 32 bit value in which a bit represent a pin in the group. If the
    value of the bit is 1, the corresponding port pin will driven to logic 1. If
    the value of the bit is 0. the corresponding port pin will stay un-affected.

  Returns:
    None.

  Example:
    <code>

    // Set PC5 and PC7 pins to 1
    PORT_GroupSet(PORT_GROUP_C, 0x00A0);

    </code>

  Remarks:
    If the port pin within the the group is not configured for output and has
    the pull feature enabled, driving a logic 1 on this pin will cause the pull
    up to be enabled.
*/

void PORT_GroupSet(PORT_GROUP group, uint32_t mask);

// *****************************************************************************
/* Function:
    void PORT_GroupClear(PORT_GROUP group, uint32_t mask)

  Summary:
    Clears the selected IO pins of a group.

  Description:
    This function clears (drives a logic 0) on the selected output pins of a
    group. The mask parameter control the pins to be updated. A mask bit
    position with a value 1 will cause that corresponding port pin to be clear.
    A mask bit position with a value 0 will cause the corresponding port pin to
    stay un-affected.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    group - One of the IO ports from the enum PORT_GROUP.
    mask - A 32 bit value in which a bit represent a pin in the group. If the
    value of the bit is 1, the corresponding port pin will driven to logic 0. If
    the value of the bit is 0. the corresponding port pin will stay un-affected.

  Returns:
    None.

  Example:
    <code>

    // Clear PC5 and PC7 pins to 1
    PORT_GroupClear(PORT_GROUP_C, 0x00A0);

    </code>

  Remarks:
    If the port pin within the the group is not configured for output and has
    the pull feature enabled, driving a logic 0 on this pin will cause the pull
    down to be enabled.
*/

void PORT_GroupClear(PORT_GROUP group, uint32_t mask);

// *****************************************************************************
/* Function:
    void PORT_GroupToggle(PORT_GROUP group, uint32_t mask)

  Summary:
    Toggles the selected IO pins of a group.

  Description:
    This function toggles the selected output pins of a group. The mask
    parameter control the pins to be updated. A mask bit position with a value 1
    will cause that corresponding port pin to be toggled.  A mask bit position
    with a value 0 will cause the corresponding port pin to stay un-affected.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    group - One of the IO ports from the enum PORT_GROUP.
    mask - A 32 bit value in which a bit represent a pin in the group. If the
    value of the bit is 1, the corresponding port pin will be toggled. If the
    value of the bit is 0. the corresponding port pin will stay un-affected.

  Returns:
    None.

  Example:
    <code>

    // Clear PC5 and PC7 pins to 1
    PORT_GroupToggle(PORT_GROUP_C, 0x00A0);

    </code>

  Remarks:
    If the port pin within the the group is not configured for output and has
    the pull feature enabled, driving a logic 0 on this pin will cause the pull
    down to be enabled. Driving a logic 1 on this pin will cause the pull up to
    be enabled.
*/

void PORT_GroupToggle(PORT_GROUP group, uint32_t mask);

// *****************************************************************************
/* Function:
    void PORT_GroupInputEnable(PORT_GROUP group, uint32_t mask)

  Summary:
    Configures the selected IO pins of a group as input.

  Description:
    This function configures the selected IO pins of a group as input. The pins
    to be configured as input are selected by setting the corresponding bits in
    the mask parameter to 1.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    group - One or more of the of the IO ports from the enum PORT_GROUP.
    mask - A 32 bit value in which a bit represents a pin in the group. If the
    value of the bit is 1, the corresponding port pin will be configured as
    input. If the value of the bit is 0. the corresponding port pin will stay
    un-affected.

  Returns:
    None.

  Example:
    <code>

    // Make PC5 and PC7 pins as input
    PORT_GroupInputEnable(PORT_GROUP_C, 0x00A0);

    </code>

  Remarks:
   None.
*/

void PORT_GroupInputEnable(PORT_GROUP group, uint32_t mask);

// *****************************************************************************
/* Function:
    void PORT_GroupOutputEnable(PORT_GROUP group, uint32_t mask)

  Summary:
    Configures the selected IO pins of a group as output.

  Description:
    This function configures the selected IO pins of a group as output. The pins
    to be configured as output are selected by setting the corresponding bits in
    the mask parameter to 1.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    group - One or more of the of the IO ports from the enum PORT_GROUP.
    mask - A 32 bit value in which a bit represents a pin in the group. If the
    value of the bit is 1, the corresponding port pin will be configured as
    output. If the value of the bit is 0. the corresponding port pin will stay
    un-affected.

  Returns:
    None.

  Example:
    <code>

    // Make PC5 and PC7 pins as output
    PORT_GroupOutputEnable(PORT_GROUP_C, 0x00A0);

    </code>

  Remarks:
    None.
*/

void PORT_GroupOutputEnable(PORT_GROUP group, uint32_t mask);

// *****************************************************************************
// *****************************************************************************
// Section: PIO Functions which operates on one pin at a time
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void PORT_PinWrite(PORT_PIN pin, bool value)

  Summary:
    Writes the specified value to the selected pin.

  Description:
    This function writes/drives the "value" on the selected I/O line/pin.

  Remarks:
    Refer plib_port.h file for more information.
*/

static inline void PORT_PinWrite(PORT_PIN pin, bool value)
{
    PORT_GroupWrite((PORT_GROUP)(PORT_BASE_ADDRESS + (0x80 * (pin>>5))), (uint32_t)(0x1) << (pin & 0x1f), (uint32_t)(value) << (pin & 0x1f));
}

// *****************************************************************************
/* Function:
    bool PORT_PinRead(PORT_PIN pin)

  Summary:
    Read the selected pin value.

  Description:
    This function reads the present state at the selected input pin.  The
    function can also be called to read the value of an output pin if input
    sampling on the output pin is enabled in MHC. If input synchronization on
    the pin is disabled in MHC, the function will cause a 2 PORT Clock cycles
    delay. Enabling the synchronization eliminates the delay but will increase
    power consumption.

  Remarks:
    Refer plib_port.h file for more information.
*/

bool PORT_PinRead(PORT_PIN pin)
{
    return (bool)((PORT_GroupRead((PORT_GROUP)(PORT_BASE_ADDRESS + (0x80 * (pin>>5)))) >> (pin & 0x1F)) & 0x1);
}

// *****************************************************************************
/* Function:
    bool PORT_PinLatchRead(PORT_PIN pin)

  Summary:
    Read the value driven on the selected pin.

  Description:
    This function reads the data driven on the selected I/O line/pin. The
    function does not sample the state of the hardware pin. It only returns the
    value that is written to output register. Refer to the PORT_PinRead()
    function if the state of the output pin needs to be read.

  Remarks:
    Refer plib_port.h file for more information.
*/

bool PORT_PinLatchRead(PORT_PIN pin)
{
    return (bool)((PORT_GroupLatchRead((PORT_GROUP)(PORT_BASE_ADDRESS + (0x80 * (pin>>5)))) >> (pin & 0x1F)) & 0x1);
}

// *****************************************************************************
/* Function:
    void PORT_PinToggle(PORT_PIN pin)

  Summary:
    Toggles the selected pin.

  Description:
    This function toggles/inverts the present value on the selected I/O line/pin.

  Remarks:
    Refer plib_port.h file for more information.
*/

void PORT_PinToggle(PORT_PIN pin)
{
    PORT_GroupToggle((PORT_GROUP)(PORT_BASE_ADDRESS + (0x80 * (pin>>5))), 0x1 << (pin & 0x1F));
}

// *****************************************************************************
/* Function:
    void PORT_PinSet(PORT_PIN pin)

  Summary:
    Sets the selected pin.

  Description:
    This function drives a logic 1 on the selected I/O line/pin.

  Remarks:
    Refer plib_port.h file for more information.
*/

void PORT_PinSet(PORT_PIN pin)
{
    PORT_GroupSet((PORT_GROUP)(PORT_BASE_ADDRESS + (0x80 * (pin>>5))), 0x1 << (pin & 0x1F));
}

// *****************************************************************************
/* Function:
    void PORT_PinClear(PORT_PIN pin)

  Summary:
    Clears the selected pin.

  Description:
    This function drives a logic 0 on the selected I/O line/pin.

  Remarks:
    Refer plib_port.h file for more information.
*/

void PORT_PinClear(PORT_PIN pin)
{
    PORT_GroupClear((PORT_GROUP)(PORT_BASE_ADDRESS + (0x80 * (pin>>5))), 0x1 << (pin & 0x1F));
}

// *****************************************************************************
/* Function:
    void PORT_PinInputEnable(PORT_PIN pin)

  Summary:
    Configures the selected IO pin as input.

  Description:
    This function configures the selected IO pin as input. This function
    override the MHC input output pin settings.

  Remarks:
    Refer plib_port.h file for more information.
*/

void PORT_PinInputEnable(PORT_PIN pin)
{
    PORT_GroupInputEnable((PORT_GROUP)(PORT_BASE_ADDRESS + (0x80 * (pin>>5))), 0x1 << (pin & 0x1F));
}

// *****************************************************************************
/* Function:
    void PORT_PinOutputEnable(PORT_PIN pin)

  Summary:
    Enables selected IO pin as output.

  Description:
    This function enables selected IO pin as output. Calling this function will
    override the MHC input output pin configuration.

  Remarks:
    Refer plib_port.h file for more information.
*/

void PORT_PinOutputEnable(PORT_PIN pin)
{
    PORT_GroupOutputEnable((PORT_GROUP)(PORT_BASE_ADDRESS + (0x80 * (pin>>5))), 0x1 << (pin & 0x1F));
}

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

}

#endif
// DOM-IGNORE-END
#endif // PLIB_PORT_H
