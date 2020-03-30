/*****************************************************************************
 * MODULE:             JN-AN-1218
 *
 * COMPONENT:          App_ExtendedColorLight.c
 *
 * DESCRIPTION:        ZLO Demo: Extended Color Light Implementation
 *
 ****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5168, JN5179].
 * You, and any third parties must reproduce the copyright and warranty notice
 * and any other legend of ownership on each copy or partial copy of the
 * software.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright NXP B.V. 2016. All rights reserved
 *
 ***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include "zps_gen.h"
#include "app_reporting.h"
#include "App_ExtendedColorLight.h"
#include "app_common.h"
#include "app_zcl_light_task.h"
#include "dbg.h"
#include <string.h>

#include "AppHardwareApi.h"
#include "app_light_interpolation.h"
#include "DriverBulb_Shim.h"
#include "bdb_options.h"

#include "zlo_device_id.h"

#ifdef DEBUG_LIGHT_TASK
#define TRACE_LIGHT_TASK  TRUE
#else
#define TRACE_LIGHT_TASK FALSE
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/


/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

tsZLO_ExtendedColourLightDevice sLight;
tsIdentifyColour sIdEffect;
#ifdef     APP_TOUCHLINK_ENABLED
tsCLD_ZllDeviceTable sDeviceTable = { ZLO_NUMBER_DEVICES,
                                      {
                                          { 0,
                                            HA_PROFILE_ID,
                                            ZLO_EXTENDED_COLOUR_LIGHT_DEVICE_ID,
                                            EXTENDEDCOLORLIGHT_LIGHT_ENDPOINT,
                                            APPLICATION_DEVICE_VERSION,
                                            0,
                                            0}
                                      }
};
#endif


/* define the default reports */
tsReports asDefaultReports[ZLO_NUMBER_OF_REPORTS] = \
{\
    {GENERAL_CLUSTER_ID_ONOFF,           {0, E_ZCL_BOOL,     E_CLD_ONOFF_ATTR_ID_ONOFF,                         ZLO_MIN_REPORT_INTERVAL,ZLO_MAX_REPORT_INTERVAL,0,{0}}},\
    {GENERAL_CLUSTER_ID_LEVEL_CONTROL,   {0, E_ZCL_UINT8,    E_CLD_LEVELCONTROL_ATTR_ID_CURRENT_LEVEL,          ZLO_MIN_REPORT_INTERVAL,ZLO_MAX_REPORT_INTERVAL,0,{10}}},\
    {LIGHTING_CLUSTER_ID_COLOUR_CONTROL, {0, E_ZCL_UINT8,  E_CLD_COLOURCONTROL_ATTR_CURRENT_HUE,              ZLO_MIN_REPORT_INTERVAL,ZLO_MAX_REPORT_INTERVAL,0,{10}}},\
    {LIGHTING_CLUSTER_ID_COLOUR_CONTROL, {0, E_ZCL_UINT8,  E_CLD_COLOURCONTROL_ATTR_CURRENT_SATURATION,       ZLO_MIN_REPORT_INTERVAL,ZLO_MAX_REPORT_INTERVAL,0,{10}}},\
    {LIGHTING_CLUSTER_ID_COLOUR_CONTROL, {0, E_ZCL_UINT16, E_CLD_COLOURCONTROL_ATTR_CURRENT_X,                ZLO_MIN_REPORT_INTERVAL,ZLO_MAX_REPORT_INTERVAL,0,{.zuint16ReportableChange=10}}},\
    {LIGHTING_CLUSTER_ID_COLOUR_CONTROL, {0, E_ZCL_UINT16, E_CLD_COLOURCONTROL_ATTR_CURRENT_Y,                ZLO_MIN_REPORT_INTERVAL,ZLO_MAX_REPORT_INTERVAL,0,{.zuint16ReportableChange=10}}},\
    {LIGHTING_CLUSTER_ID_COLOUR_CONTROL, {0, E_ZCL_UINT16, E_CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED, ZLO_MIN_REPORT_INTERVAL,ZLO_MAX_REPORT_INTERVAL,0,{.zuint16ReportableChange=10}}}\
};

#ifdef     APP_TOUCHLINK_ENABLED
/*
 * If overriding the ZLL Master Key in the appilation,
 * define BDB_APPLICATION_DEFINED_TL_MASTER_KEY in bdb_options.h
 * otherwize the key defined in BDB\Source\touchlink\bdb_tl_common.c will be used.
 */
#ifdef BDB_APPLICATION_DEFINED_TL_MASTER_KEY
PUBLIC tsReg128 sTLMasterKey = {0x11223344, 0x55667788, 0x99aabbcc, 0xddeeff00 };
#endif
#endif
/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
#ifdef     APP_TOUCHLINK_ENABLED
PRIVATE bool bAllowInterPanEp(uint8 u8Ep, uint16 u16ProfileId);
#endif
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: eApp_ZLO_RegisterEndpoint
 *
 * DESCRIPTION:
 * Register ZLO endpoints
 *
 * PARAMETER
 * Type                        Name                    Description
 * tfpZCL_ZCLCallBackFunction  fptr                    Pointer to ZCL Callback function
 *
 *
 * RETURNS:
 * teZCL_Status
 *
 ****************************************************************************/
PUBLIC teZCL_Status eApp_ZLO_RegisterEndpoint(tfpZCL_ZCLCallBackFunction fptr)
{

    zps_vSetIgnoreProfileCheck();
#ifdef     APP_TOUCHLINK_ENABLED
    ZPS_vAplZdoRegisterInterPanFilter( bAllowInterPanEp);
#endif
    return eZLO_RegisterExtendedColourLightEndPoint(EXTENDEDCOLORLIGHT_LIGHT_ENDPOINT,
                                                    fptr,
                                                    &sLight);
}

#ifdef     APP_TOUCHLINK_ENABLED
/****************************************************************************
*
* NAME: bAllowInterPanEp
*
* DESCRIPTION: Allows the application to decide which end point receive
* inter pan messages
*
*
* PARAMETER: the end point receiving the inter pan
*
* RETURNS: True to allow reception, False otherwise
*
****************************************************************************/
PRIVATE bool bAllowInterPanEp(uint8 u8Ep, uint16 u16ProfileId) {

    if ( (u8Ep == EXTENDEDCOLORLIGHT_LIGHT_ENDPOINT) &&
          ( u16ProfileId == ZLL_PROFILE_ID))
    {
        return TRUE;
    }
    return FALSE;
}
#endif

/****************************************************************************
 *
 * NAME: vApp_eCLD_ColourControl_GetRGB
 *
 * DESCRIPTION:
 * To get RGB value
 *
 * PARAMETER
 * Type        Name                    Description
 * uint8 *     pu8Red                  Pointer to Red in RGB value
 * uint8 *     pu8Green                Pointer to Green in RGB value
 * uint8 *     pu8Blue                 Pointer to Blue in RGB value
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void vApp_eCLD_ColourControl_GetRGB(uint8 *pu8Red,uint8 *pu8Green,uint8 *pu8Blue)
{
    eCLD_ColourControl_GetRGB(EXTENDEDCOLORLIGHT_LIGHT_ENDPOINT,
                              pu8Red,
                              pu8Green,
                              pu8Blue);
}

/****************************************************************************
 *
 * NAME: vAPP_ZCL_DeviceSpecific_Init
 *
 * DESCRIPTION:
 * ZLO Device Specific initialization
 *
 * PARAMETER: void
 *
 * RETURNS: void
 *
 ****************************************************************************/
PUBLIC void vAPP_ZCL_DeviceSpecific_Init(void)
{
    /* Initialise the strings in Basic */
    /* Initialise the strings in Basic */
    uint8    au8PCode[CLD_BAS_PCODE_SIZE] = BAS_PCODE_STRING;
    memcpy(sLight.sBasicServerCluster.au8ManufacturerName, BAS_MANUF_NAME_STRING, CLD_BAS_MANUF_NAME_SIZE);
    memcpy(sLight.sBasicServerCluster.au8ModelIdentifier, BAS_MODEL_ID_STRING, CLD_BAS_MODEL_ID_SIZE);
    memcpy(sLight.sBasicServerCluster.au8DateCode, BAS_DATE_STRING, CLD_BAS_DATE_SIZE);
    memcpy(sLight.sBasicServerCluster.au8SWBuildID, BAS_SW_BUILD_STRING, CLD_BAS_SW_BUILD_SIZE);
    memcpy(sLight.sBasicServerCluster.au8ProductURL, BAS_URL_STRING, CLD_BAS_URL_SIZE);
    memcpy(sLight.sBasicServerCluster.au8ProductCode, au8PCode, CLD_BAS_PCODE_SIZE);

    sIdEffect.u8Effect = E_CLD_IDENTIFY_EFFECT_STOP_EFFECT;
    sIdEffect.u8Tick = 0;
}

/****************************************************************************
 *
 * NAME: app_u8GetDeviceEndpoint
 *
 * DESCRIPTION:
 * Return the application endpoint
 *
 * PARAMETER: void
 *
 * RETURNS: uint8
 *
 ****************************************************************************/
PUBLIC uint8 app_u8GetDeviceEndpoint( void)
{
    return EXTENDEDCOLORLIGHT_LIGHT_ENDPOINT;
}

/****************************************************************************
 *
 * NAME: vApp_ZCL_ResetDeviceStructure
 *
 * DESCRIPTION:
 * Resets the device structure
 *
 * PARAMETER: void
 *
 * RETURNS: void
 *
 ****************************************************************************/
PUBLIC void vApp_ZCL_ResetDeviceStructure(void)
{
    memset(&sLight,0,sizeof(tsZLO_ExtendedColourLightDevice));
}

/****************************************************************************
 *
 * NAME: APP_vHandleIdentify
 *
 * DESCRIPTION:
 * ZLO Device Specific identify
 *
 * PARAMETER: the identify time
 *
 * RETURNS: void
 *
 ****************************************************************************/
PUBLIC void APP_vHandleIdentify(uint16 u16Time) {

    uint8 u8Red, u8Green, u8Blue;

    DBG_vPrintf(TRACE_LIGHT_TASK, "JP Time %d\n", u16Time);

    if (sIdEffect.u8Effect < E_CLD_IDENTIFY_EFFECT_STOP_EFFECT) {
        /* do nothing */
    }
    else if (u16Time == 0)
    {
            /*
             * Restore to off/off/colour state
             */


        vApp_eCLD_ColourControl_GetRGB(&u8Red, &u8Green, &u8Blue);

        DBG_vPrintf(TRACE_LIGHT_TASK, "R %d G %d B %d L %d Hue %d Sat %d\n", u8Red, u8Green, u8Blue,
                            sLight.sLevelControlServerCluster.u8CurrentLevel,
                            sLight.sColourControlServerCluster.u8CurrentHue,
                            sLight.sColourControlServerCluster.u8CurrentSaturation);

        vRestoreVars();
        vRGBLight_SetLevels(sLight.sOnOffServerCluster.bOnOff,
                            sLight.sLevelControlServerCluster.u8CurrentLevel,
                            u8Red,
                            u8Green,
                            u8Blue);
    }
    else
        {
            /* Set the Identify levels */

            vSaveVars();
            vRGBLight_SetLevels(TRUE, 254, 250, 0, 0);
        }
}

/****************************************************************************
 *
 * NAME: vIdEffectTick
 *
 * DESCRIPTION:
 * ZLO Device Specific identify tick
 *
 * PARAMETER: uint8 End Point identifier
 *
 * RETURNS: void
 *
 ****************************************************************************/
PUBLIC void vIdEffectTick(uint8 u8Endpoint) {

    if (u8Endpoint != EXTENDEDCOLORLIGHT_LIGHT_ENDPOINT) {
        return;
    }
    if (sIdEffect.u8Effect < E_CLD_IDENTIFY_EFFECT_STOP_EFFECT)
    {
        if (sIdEffect.u8Tick > 0)
        {

            sIdEffect.u8Tick--;
            /* Set the light parameters */
            vRGBLight_SetLevels(TRUE, sIdEffect.u8Level,sIdEffect.u8Red,sIdEffect.u8Green,sIdEffect.u8Blue);
            /* Now adjust parameters ready for for next round */
            switch (sIdEffect.u8Effect) {
                case E_CLD_IDENTIFY_EFFECT_BLINK:
                    break;

                case E_CLD_IDENTIFY_EFFECT_BREATHE:
                    if (sIdEffect.bDirection) {
                        if (sIdEffect.u8Level >= 250) {
                            sIdEffect.u8Level -= 50;
                            sIdEffect.bDirection = 0;
                        } else {
                            sIdEffect.u8Level += 50;
                        }
                    } else {
                        if (sIdEffect.u8Level == 0) {
                            // go back up, check for stop
                            sIdEffect.u8Count--;
                            if ((sIdEffect.u8Count) && ( !sIdEffect.bFinish)) {
                                sIdEffect.u8Level += 50;
                                sIdEffect.bDirection = 1;
                            } else {
                                /* lpsw2773 - stop the effect on the next tick */
                                sIdEffect.u8Tick = 0;
                            }
                        } else {
                            sIdEffect.u8Level -= 50;
                        }
                    }
                    break;
                default:
                    if ( sIdEffect.bFinish ) {
                        sIdEffect.u8Tick = 0;
                    }
                }
        } else {
            /*
             * Effect finished, restore the light
             */
            sIdEffect.u8Effect = E_CLD_IDENTIFY_EFFECT_STOP_EFFECT;
            sIdEffect.bDirection = FALSE;
            APP_ZCL_vSetIdentifyTime(0);
                uint8 u8Red, u8Green, u8Blue;
                vApp_eCLD_ColourControl_GetRGB(&u8Red, &u8Green, &u8Blue);
                DBG_vPrintf(TRACE_LIGHT_TASK, "EF - R %d G %d B %d L %d Hue %d Sat %d\n",
                                    u8Red,
                                    u8Green,
                                    u8Blue,
                                    sLight.sLevelControlServerCluster.u8CurrentLevel,
                                    sLight.sColourControlServerCluster.u8CurrentHue,
                                    sLight.sColourControlServerCluster.u8CurrentSaturation);

                vRestoreVars();
                vRGBLight_SetLevels(sLight.sOnOffServerCluster.bOnOff,
                                    sLight.sLevelControlServerCluster.u8CurrentLevel,
                                    u8Red,
                                    u8Green,
                                    u8Blue);
        }
    }
}

/****************************************************************************
 *
 * NAME: vStartEffect
 *
 * DESCRIPTION:
 * ZLO Device Specific identify effect set up
 *
 * PARAMETER: uint8 Effect identifier
 *
 * RETURNS: void
 *
 ****************************************************************************/
PUBLIC void vStartEffect(uint8 u8Effect) {
    vSaveVars();
    switch (u8Effect) {
        case E_CLD_IDENTIFY_EFFECT_BLINK:
            sIdEffect.u8Effect = E_CLD_IDENTIFY_EFFECT_BLINK;
            sIdEffect.u8Level = 250;
            sIdEffect.u8Red = 255;
            sIdEffect.u8Green = 0;
            sIdEffect.u8Blue = 0;
            sIdEffect.bFinish = FALSE;
            APP_ZCL_vSetIdentifyTime(2);
            sIdEffect.u8Tick = 10;
            break;
        case E_CLD_IDENTIFY_EFFECT_BREATHE:
            sIdEffect.u8Effect = E_CLD_IDENTIFY_EFFECT_BREATHE;
            sIdEffect.bDirection = 1;
            sIdEffect.bFinish = FALSE;
            sIdEffect.u8Level = 0;
            sIdEffect.u8Count = 15;
            eCLD_ColourControl_GetRGB( EXTENDEDCOLORLIGHT_LIGHT_ENDPOINT, &sIdEffect.u8Red, &sIdEffect.u8Green, &sIdEffect.u8Blue);
            APP_ZCL_vSetIdentifyTime(17);
            sIdEffect.u8Tick = 200;
            break;
        case E_CLD_IDENTIFY_EFFECT_OKAY:
            sIdEffect.u8Effect = E_CLD_IDENTIFY_EFFECT_OKAY;
            sIdEffect.bFinish = FALSE;
            sIdEffect.u8Level = 250;
            sIdEffect.u8Red = 0;
            sIdEffect.u8Green = 255;
            sIdEffect.u8Blue = 0;
            APP_ZCL_vSetIdentifyTime(2);
            sIdEffect.u8Tick = 10;
            break;
        case E_CLD_IDENTIFY_EFFECT_CHANNEL_CHANGE:
            sIdEffect.u8Effect = E_CLD_IDENTIFY_EFFECT_CHANNEL_CHANGE;
            sIdEffect.u8Level = 250;
            sIdEffect.u8Red = 255;
            sIdEffect.u8Green = 127;
            sIdEffect.u8Blue = 4;
            sIdEffect.bFinish = FALSE;
            APP_ZCL_vSetIdentifyTime(9);
            sIdEffect.u8Tick = 80;
            break;

        case E_CLD_IDENTIFY_EFFECT_FINISH_EFFECT:
            if (sIdEffect.u8Effect < E_CLD_IDENTIFY_EFFECT_STOP_EFFECT)
            {
                DBG_vPrintf(TRACE_LIGHT_TASK, "\n<FINISH>");
                sIdEffect.bFinish = TRUE;
            }
            break;
        case E_CLD_IDENTIFY_EFFECT_STOP_EFFECT:
            sIdEffect.u8Effect = E_CLD_IDENTIFY_EFFECT_STOP_EFFECT;
            APP_ZCL_vSetIdentifyTime(1);
            break;
    }
}

/****************************************************************************
 *
 * NAME: vRGBLight_SetLevels
 *
 * DESCRIPTION:
 * Set the RGB and levels
 *
 * PARAMETER: On/Off status, level, red, green and blue colour values
 *
 * RETURNS: void
 ****************************************************************************/
PUBLIC void vRGBLight_SetLevels(bool_t bOn, uint8 u8Level, uint8 u8Red, uint8 u8Green, uint8 u8Blue)
{
    if (bOn == TRUE)
    {
        vLI_Start(u8Level, u8Red, u8Green, u8Blue, 0);
    }
    else
    {
        vLI_Stop();
    }
    vBULB_SetOnOff(bOn);
}


/****************************************************************/
/* OS Stub functions to allow single osconfig diagram (ZLL/ZHA) */
/* to be used for all driver variants (just clear interrupt)    */
/****************************************************************/

#ifndef DR1192

PUBLIC void vISR_Timer3(void)
{
    (void) u8AHI_TimerFired(E_AHI_TIMER_3);
}

PUBLIC void vISR_Timer4(void)
{
    (void) u8AHI_TimerFired(E_AHI_TIMER_4);
}

#endif
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

