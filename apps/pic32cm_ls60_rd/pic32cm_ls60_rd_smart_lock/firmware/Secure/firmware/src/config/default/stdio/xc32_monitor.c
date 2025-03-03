/*******************************************************************************
 Debug Console Source file

  Company:
    Microchip Technology Inc.

  File Name:
    xc32_monitor.c

  Summary:
    debug console Source File

  Description:
    None

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
#include <stddef.h>
#include "definitions.h"

/* Declaration of these functions are missing in stdio_s.h for ARM parts*/
/* MISRAC 2012 deviation block start */
/* MISRA C-2012 Rule 21.2 deviated four times.  Deviation record ID -  H3_MISRAC_2012_R_21_2_DR_1 */


int read(int handle, void *buffer, unsigned int len)
{
    int nChars = 0;
    bool success = false;
    (void)len;
    if ((handle == 0)  && (len > 0))
    {
        do
        {
            success = SERCOM2_USART_Read(buffer, 1);
        }while( !success);
        nChars = 1;
    }
    return nChars;
}

int write(int handle, void * buffer, size_t count)
{
   bool success = false;
   if (handle == 1)
   {
       do
       {
           success = SERCOM2_USART_Write(buffer, count);
       }while( !success);
   }
   return count;
}

/* MISRAC 2012 deviation block end */