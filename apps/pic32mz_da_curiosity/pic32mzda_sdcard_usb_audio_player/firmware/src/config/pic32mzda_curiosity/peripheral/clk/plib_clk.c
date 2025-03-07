/*******************************************************************************
  SYS CLK Static Functions for Clock System Service

  Company:
    Microchip Technology Inc.

  File Name:
    plib_clk.c

  Summary:
    SYS CLK static function implementations for the Clock System Service.

  Description:
    The Clock System Service provides a simple interface to manage the
    oscillators on Microchip microcontrollers. This file defines the static
    implementation for the Clock System Service.

  Remarks:
    Static functions incorporate all system clock configuration settings as
    determined by the user via the Microchip Harmony Configurator GUI.
    It provides static version of the routines, eliminating the need for an
    object ID or object handle.

    Static single-open interfaces also eliminate the need for the open handle.

*******************************************************************************/

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

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#include "device.h"
#include "plib_clk.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void CLK_Initialize( void )

  Summary:
    Initializes hardware and internal data structure of the System Clock.

  Description:
    This function initializes the hardware and internal data structure of System
    Clock Service.

  Remarks:
    This is configuration values for the static version of the Clock System
    Service module is determined by the user via the MHC GUI.

    The objective is to eliminate the user's need to be knowledgeable in the
    function of the 'configuration bits' to configure the system oscillators.
*/

void CLK_Initialize( void )
{
    /* unlock system for clock configuration */
    SYSKEY = 0x00000000U;
    SYSKEY = 0xAA996655U;
    SYSKEY = 0x556699AAU;


    /* Set up Reference Clock 1 */
    /* REFO1CON register */
    /* ROSEL =  SYSCLK */
    /* DIVSWEN = 1 */
    /* RODIV = 8 */
    REFO1CON = 0x80200;

    /* REFO1TRIM register */
    /* ROTRIM = 443 */
    REFO1TRIM = 0xdd800000;

    /* Enable oscillator (ON bit) and Enable Output (OE bit) */
    REFO1CONSET = 0x00001000U | 0x00008000U;

    /* Set up Reference Clock 4 */
    /* REFO4CON register */
    /* ROSEL =  SYSCLK */
    /* DIVSWEN = 1 */
    /* RODIV = 1 */
    REFO4CON = 0x10200;

    /* Enable oscillator (ON bit) */
    REFO4CONSET = 0x00008000;

    /* Set up Reference Clock 5 */
    /* Enable oscillator (ON bit) */
    REFO5CONSET = 0x00008000;

    /* CFGMPLL */
    /* MPLLVREGDIS = ENABLED */
    /* INTVREFCON = EXTERNAL_DDRV */
    /* MPLLIDIV = DIV_1 */
    /* MPLLMULT = MUL_25 */
    /* MPLLODIV2 = DIV_1 */
    /* MPLLODIV1 = DIV_3 */
    /* MPLLDIS = ENABLED */

    CFGMPLLbits.MPLLVREGDIS = 0;

    while(CFGMPLLbits.MPLLVREGRDY == 0U)
    {
        /* Wait for completion */
    }

    CFGMPLL = 0xb001901;

    while(CFGMPLLbits.MPLLRDY == 0U)
    {
        /* Wait for completion */
    }
  

    /* Peripheral Module Disable Configuration */

    CFGCONbits.PMDLOCK = 0;

    PMD1 = 0x101101U;
    PMD2 = 0x3U;
    PMD3 = 0x1ff01ffU;
    PMD4 = 0x1ffU;
    PMD5 = 0x301c3a37U;
    PMD6 = 0x10831e00U;
    PMD7 = 0x500000U;

    CFGCONbits.PMDLOCK = 1;

    /* Lock system since done with clock configuration */
    SYSKEY = 0x33333333U;
}
