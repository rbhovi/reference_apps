/*******************************************************************************
  Real Time Counter (RTC) PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_rtc.h

  Summary:
    RTC PLIB Header file

  Description:
    This file defines the interface to the RTC peripheral library. This
    library provides access to and control of the associated peripheral
    instance.

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

#ifndef PLIB_RTC_H
#define PLIB_RTC_H

#include "device.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
extern "C" {
#endif
// DOM-IGNORE-END
// *****************************************************************************
// *****************************************************************************
// Section:Preprocessor macros
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************

/* Frequency of Counter Clock for RTC */
#define RTC_COUNTER_CLOCK_FREQUENCY        (32768U / (1UL << (0x1U - 1U)))

#define RTC_TIMER32_INT_MASK_PER0  RTC_MODE0_INTENSET_PER0_Msk
#define RTC_TIMER32_INT_MASK_PER1  RTC_MODE0_INTENSET_PER1_Msk
#define RTC_TIMER32_INT_MASK_PER2  RTC_MODE0_INTENSET_PER2_Msk
#define RTC_TIMER32_INT_MASK_PER3  RTC_MODE0_INTENSET_PER3_Msk
#define RTC_TIMER32_INT_MASK_PER4  RTC_MODE0_INTENSET_PER4_Msk
#define RTC_TIMER32_INT_MASK_PER5  RTC_MODE0_INTENSET_PER5_Msk
#define RTC_TIMER32_INT_MASK_PER6  RTC_MODE0_INTENSET_PER6_Msk
#define RTC_TIMER32_INT_MASK_PER7  RTC_MODE0_INTENSET_PER7_Msk
#define RTC_TIMER32_INT_MASK_CMP0  RTC_MODE0_INTENSET_CMP0_Msk
#define RTC_TIMER32_INT_MASK_CMP1  RTC_MODE0_INTENSET_CMP1_Msk
#define RTC_TIMER32_INT_MASK_TAMPER  RTC_MODE0_INTENSET_TAMPER_Msk
#define RTC_TIMER32_INT_MASK_OVF  RTC_MODE0_INTENSET_OVF_Msk
#define RTC_TIMER32_INT_MASK_INVALID 0xFFFFFFFFU
// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************

typedef enum
{
    RTC_PER0_MASK = 0x0001U,
    RTC_PER1_MASK = 0x0002U,
    RTC_PER2_MASK = 0x0004U,
    RTC_PER3_MASK = 0x0008U,
    RTC_PER4_MASK = 0x0010U,
    RTC_PER5_MASK = 0x0020U,
    RTC_PER6_MASK = 0x0040U,
    RTC_PER7_MASK = 0x0080U
} RTC_PERIODIC_INT_MASK;
typedef enum
{
    BACKUP_REGISTER_0 = 0U,
    BACKUP_REGISTER_1 = 1U,
    BACKUP_REGISTER_2 = 2U,
    BACKUP_REGISTER_3 = 3U,
    BACKUP_REGISTER_4 = 4U,
    BACKUP_REGISTER_5 = 5U,
    BACKUP_REGISTER_6 = 6U,
    BACKUP_REGISTER_7 = 7U
} BACKUP_REGISTER;
 #define   TAMPER_CHANNEL_0  (0U)
 #define   TAMPER_CHANNEL_1  (1U)
 #define   TAMPER_CHANNEL_2  (2U)
 #define   TAMPER_CHANNEL_3  (3U)
 #define   TAMPER_CHANNEL_4  (4U)
typedef uint32_t TAMPER_CHANNEL;


void RTC_Initialize(void);
bool RTC_PeriodicIntervalHasCompleted ( RTC_PERIODIC_INT_MASK period );
bool RTC_Timer32CounterHasOverflowed ( void );
bool RTC_Timer32Compare0HasMatched( void );
bool RTC_Timer32Compare1HasMatched( void );
void RTC_Timer32CountSyncEnable ( void );
void RTC_Timer32CountSyncDisable ( void );
void RTC_Timer32Start ( void );
void RTC_Timer32Stop ( void );
void RTC_Timer32CounterSet ( uint32_t count );
uint32_t RTC_Timer32CounterGet ( void );
uint32_t RTC_Timer32FrequencyGet ( void );
void RTC_Timer32Compare0Set ( uint32_t compareValue );
void RTC_Timer32Compare1Set ( uint32_t compareValue );
uint32_t RTC_Timer32PeriodGet ( void );
void RTC_BackupRegisterSet( BACKUP_REGISTER reg, uint32_t value );
uint32_t RTC_BackupRegisterGet( BACKUP_REGISTER reg );
TAMPER_CHANNEL RTC_TamperSourceGet( void );
uint32_t RTC_Timer32TimeStampGet( void );

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
}
#endif
// DOM-IGNORE-END

#endif /* PLIB_RTC_H */
