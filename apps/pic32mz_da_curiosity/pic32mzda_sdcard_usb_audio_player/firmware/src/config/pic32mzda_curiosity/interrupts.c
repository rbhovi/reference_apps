/*******************************************************************************
 System Interrupts File

  Company:
    Microchip Technology Inc.

  File Name:
    interrupt.c

  Summary:
    Interrupt vectors mapping

  Description:
    This file maps all the interrupt vectors to their corresponding
    implementations. If a particular module interrupt is used, then its ISR
    definition can be found in corresponding PLIB source file. If a module
    interrupt is not used, then its ISR implementation is mapped to dummy
    handler.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "configuration.h"
#include "interrupts.h"
#include "definitions.h"



// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector Functions
// *****************************************************************************
// *****************************************************************************


/* All the handlers are defined here.  Each will call its PLIB-specific function. */
// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector declarations
// *****************************************************************************
// *****************************************************************************
void CORE_TIMER_Handler (void);
void I2C1_BUS_Handler (void);
void I2C1_MASTER_Handler (void);
void USB_Handler (void);
void USB_DMA_Handler (void);
void DMA0_Handler (void);
void DMA1_Handler (void);
void DMA3_Handler (void);
void I2C2_BUS_Handler (void);
void I2C2_MASTER_Handler (void);
void SDHC_Handler (void);
void GLCD_Handler (void);


// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector definitions
// *****************************************************************************
// *****************************************************************************
void __ISR(_CORE_TIMER_VECTOR, ipl1SRS) CORE_TIMER_Handler (void)
{
    CORE_TIMER_InterruptHandler();
}

void __ISR(_I2C1_BUS_VECTOR, ipl1SRS) I2C1_BUS_Handler (void)
{
    I2C1_BUS_InterruptHandler();
}

void __ISR(_I2C1_MASTER_VECTOR, ipl1SRS) I2C1_MASTER_Handler (void)
{
    I2C1_MASTER_InterruptHandler();
}

void __ISR(_USB_VECTOR, ipl1SRS) USB_Handler (void)
{
    DRV_USBHS_InterruptHandler();
}

void __ISR(_USB_DMA_VECTOR, ipl1SRS) USB_DMA_Handler (void)
{
    DRV_USBHS_DMAInterruptHandler();
}

void __ISR(_DMA0_VECTOR, ipl1SRS) DMA0_Handler (void)
{
    DMA0_InterruptHandler();
}

void __ISR(_DMA1_VECTOR, ipl1SRS) DMA1_Handler (void)
{
    DMA1_InterruptHandler();
}

void __ISR(_DMA3_VECTOR, ipl1SRS) DMA3_Handler (void)
{
    DMA3_InterruptHandler();
}

void __ISR(_I2C2_BUS_VECTOR, ipl1SRS) I2C2_BUS_Handler (void)
{
    I2C2_BUS_InterruptHandler();
}

void __ISR(_I2C2_MASTER_VECTOR, ipl1SRS) I2C2_MASTER_Handler (void)
{
    I2C2_MASTER_InterruptHandler();
}

void __ISR(_SDHC_VECTOR, ipl1SRS) SDHC_Handler (void)
{
    SDHC_InterruptHandler();
}

void __ISR(_GLCD_VECTOR, ipl1SRS) GLCD_Handler (void)
{
    GLCD_Interrupt_Handler();
}





/*******************************************************************************
 End of File
*/
