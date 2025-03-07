/*******************************************************************************
  oled_c c source file

  Company:
    Microchip Technology Inc.

  File Name:
    oled_c.c

  Summary:
  This file contains the "oled c" function 

  Description:
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
#include "oled_c.h"

leFixedString ctrStr;
static leChar counterStr[15];

void oled_c_initialize(void)
{
    
    leFixedString_Constructor(&ctrStr, counterStr, 4);
    ctrStr.fn->setFont(&ctrStr, leStringTable_GetStringFont(leGetState()->stringTable,
                                                          string_String.index,
                                                          0));
}

void oled_c_Text_Field_counter(uint8_t counter)
{
    char lcl_counterStr[15];
    
    sprintf( (char*)lcl_counterStr, "%02d F",counter);
    
    ctrStr.fn->setFromCStr(&ctrStr, (char*)lcl_counterStr);
    Screen0_counter->fn->setString(Screen0_counter, (leString*)&ctrStr);
    Screen0_temp->fn->setVisible(Screen0_temp, LE_TRUE);
    Screen0_light->fn->setVisible(Screen0_light, LE_TRUE);
}

void oled_c_Text_Field_light(uint16_t adc_val)
{
    char lcl_counterStr[15];
    
    sprintf( (char *)lcl_counterStr, "%x",adc_val);
    
    ctrStr.fn->setFromCStr(&ctrStr, (char*)lcl_counterStr);
    Screen0_temp_val->fn->setString(Screen0_temp_val, (leString*)&ctrStr);
   
}

void oled_c_Image_mchplogo_visible(void)
{
    Screen0_mchplogo->fn->setVisible(Screen0_mchplogo, LE_TRUE);
}

void oled_c_Image_mchplogo_invisible(void)
{
    Screen0_mchplogo->fn->setVisible(Screen0_mchplogo, LE_FALSE);
}

void oled_c_Image_ns_invisible(void)
{
    Screen0_nonsecure->fn->setVisible(Screen0_nonsecure, LE_FALSE);
}

void oled_c_Image_s_invisible(void)
{
    Screen0_secure->fn->setVisible(Screen0_secure, LE_FALSE);
}


void oled_c_dispaly_refresh(void)
{
    GFX_DISP_INTF_PIN_EN_Clear();
    GFX_DISP_INTF_PIN_RW_Clear();
    GFX_DISP_INTF_PIN_EN_Set();
}


// ------------------------------------------------------------------------- END

