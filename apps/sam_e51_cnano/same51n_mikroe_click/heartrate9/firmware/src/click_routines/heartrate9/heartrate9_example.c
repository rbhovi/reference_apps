/*******************************************************************************
  Heart Rate 9 click routine example source file

  Company
    Microchip Technology Inc.

  File Name
    heartrate9_example.c

  Summary
    Heart Rate 9 click routine example implementation File.

  Description
    This file defines the usage of the Heart Rate 9 click routine APIs.

  Remarks:
    None.
 
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*
    (c) 2021 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/
// DOM-IGNORE-END

/**
  Section: Included Files
 */

#include <stdio.h>
#include <stdint.h>
#include "definitions.h"                // SYS function prototypes
#include "heartrate9.h"

/**
  Section: Example Code
 */

void heartrate9_example(void) {
    int8_t  heartrate_data = -1;
    // Check the Heartrate Ready Status
    if(true == is_heartrate9_byte_ready())
    {
        // Return the Heartrate 
        while(heartrate_data == -1)
        {
            heartrate_data      = heartrate9_read_byte();
        }
        printf("Heartrate = %d bpm \t\r\n", (uint8_t)heartrate_data);
    }
}
