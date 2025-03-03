/*******************************************************************************
  AFEC Peripheral Library Interface Header File

  Company
    Microchip Technology Inc.

  File Name
    plib_afec1.h

  Summary
    AFEC1 peripheral library interface.

  Description
    This file defines the interface to the AFEC peripheral library.  This
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

#ifndef PLIB_AFEC1_H    // Guards against multiple inclusion
#define PLIB_AFEC1_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

/*  This section lists the other files that are included in this file.
*/

#include "plib_afec_common.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif

// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
/*  The following data type definitions are used by the functions in this
    interface and should be considered part it.
*/
#define AFEC1_CH6_MAX_OUTPUT (65535U)
#define AFEC1_CH6_MIN_OUTPUT (0U)
#define CHANNEL_6 (6U)
/***********************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines
// *****************************************************************************
// *****************************************************************************
/* The following functions make up the methods (set of possible operations) of
   this interface.
*/

void AFEC1_Initialize (void);

void AFEC1_ChannelsEnable (AFEC_CHANNEL_MASK channelsMask);

void AFEC1_ChannelsDisable (AFEC_CHANNEL_MASK channelsMask);

void AFEC1_ChannelsInterruptEnable (AFEC_INTERRUPT_MASK channelsInterruptMask);

void AFEC1_ChannelsInterruptDisable (AFEC_INTERRUPT_MASK channelsInterruptMask);

void AFEC1_ConversionStart(void);

bool AFEC1_ChannelResultIsReady(AFEC_CHANNEL_NUM channel);

uint16_t AFEC1_ChannelResultGet(AFEC_CHANNEL_NUM channel);

void AFEC1_ConversionSequenceSet(AFEC_CHANNEL_NUM *channelList, uint8_t numChannel);

void AFEC1_ChannelGainSet(AFEC_CHANNEL_NUM channel, AFEC_CHANNEL_GAIN gain);

void AFEC1_ChannelOffsetSet(AFEC_CHANNEL_NUM channel, uint16_t offset);

void AFEC1_ComparatorChannelSet(AFEC_CHANNEL_NUM channel);

void AFEC1_CompareAllChannelsEnable(void);

void AFEC1_CompareAllChannelsDisable(void);

void AFEC1_ComparatorModeSet(AFEC_COMPARATOR_MODE cmpMode);

bool AFEC1_ComparatorStatusGet(void);
	
uint32_t AFEC1_StatusGet(void);
// *****************************************************************************

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

}

#endif
// DOM-IGNORE-END

#endif //PLIB_AFEC1_H

/**
 End of File
*/

