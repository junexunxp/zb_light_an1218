/*****************************************************************************
 *
 * MODULE:             JN-AN-1218
 *
 * COMPONENT:          app_reporting.c
 *
 * DESCRIPTION:        ZLO Demo : Stack <-> Light-App Interaction (Implementation)
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
#include <string.h>
#include "dbg.h"
#include "PDM.h"
#include "PDM_IDs.h"
#include "app_common.h"
#include "zcl_options.h"
#include "zcl_common.h"
#include "app_reporting.h"

#ifdef CLD_GROUPS
#include "Groups.h"
#include "Groups_internal.h"
#endif
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef DEBUG_REPORT
    #define TRACE_REPORT   FALSE
#else
    #define TRACE_REPORT   TRUE
#endif
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/


/*There are just two attributes at this point - OnOff and CurrentLevel */

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/


/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
/*Just Two reports for time being*/
PRIVATE tsReports asSavedReports[ZLO_NUMBER_OF_REPORTS];



/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/


/****************************************************************************
 *
 * NAME: eRestoreReports
 *
 * DESCRIPTION:
 * Loads the reporting information from the EEPROM/PDM
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC PDM_teStatus eRestoreReports( void )
{
    /* Restore any report data that is previously saved to flash */
    uint16 u16ByteRead;
    PDM_teStatus eStatusReportReload = PDM_eReadDataFromRecord(PDM_ID_APP_REPORTS,
                                                              asSavedReports,
                                                              sizeof(asSavedReports),
                                                              &u16ByteRead);

    DBG_vPrintf(TRACE_REPORT,"eStatusReportReload = %d\n", eStatusReportReload);
    /* Restore any application data previously saved to flash */

    return  (eStatusReportReload);
}

/****************************************************************************
 *
 * NAME: vMakeSupportedAttributesReportable
 *
 * DESCRIPTION:
 * Makes the attributes reportable for On Off and Level control
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void vMakeSupportedAttributesReportable(void)
{
    uint16 u16AttributeEnum;
    uint16 u16ClusterId;
    int i;
    uint8 u8Endpoint;

    tsZCL_AttributeReportingConfigurationRecord*    psAttributeReportingConfigurationRecord;

    u8Endpoint = app_u8GetDeviceEndpoint();
    DBG_vPrintf(TRACE_REPORT, "MAKE Reportable ep %d\n", u8Endpoint);

    for(i=0; i<ZLO_NUMBER_OF_REPORTS; i++)
    {
        u16AttributeEnum = asSavedReports[i].sAttributeReportingConfigurationRecord.u16AttributeEnum;
        u16ClusterId = asSavedReports[i].u16ClusterID;
        psAttributeReportingConfigurationRecord = &(asSavedReports[i].sAttributeReportingConfigurationRecord);
        DBG_vPrintf(TRACE_REPORT, "Cluster %04x Attribute %04x Min %d Max %d IntV %d Direct %d Change %d\n",
                u16ClusterId,
                u16AttributeEnum,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u16MinimumReportingInterval,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u16MaximumReportingInterval,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u16TimeoutPeriodField,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u8DirectionIsReceived,
                asSavedReports[i].sAttributeReportingConfigurationRecord.uAttributeReportableChange.zint8ReportableChange);
        eZCL_SetReportableFlag( u8Endpoint, u16ClusterId, TRUE, FALSE, u16AttributeEnum);
        eZCL_CreateLocalReport( u8Endpoint, u16ClusterId, 0, TRUE, psAttributeReportingConfigurationRecord);
    }
}

/****************************************************************************
 *
 * NAME: vLoadDefaultConfigForReportable
 *
 * DESCRIPTION:
 * Loads a default configuration
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/

PUBLIC void vLoadDefaultConfigForReportable(void)
{
    memset(asSavedReports, 0 ,sizeof(asSavedReports));
    int i;
    for (i=0; i<ZLO_NUMBER_OF_REPORTS; i++)
    {
        asSavedReports[i] = asDefaultReports[i];
    }

#if TRACE_REPORT

    DBG_vPrintf(TRACE_REPORT,"\nLoaded Defaults Records \n");
    for(i=0; i <ZLO_NUMBER_OF_REPORTS; i++)
    {
        DBG_vPrintf(TRACE_REPORT,"Cluster %04x Type %d Attr %04x Min %d Max %d IntV %d Direct %d Change %d\n",
                asSavedReports[i].u16ClusterID,
                asSavedReports[i].sAttributeReportingConfigurationRecord.eAttributeDataType,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u16AttributeEnum,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u16MinimumReportingInterval,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u16MaximumReportingInterval,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u16TimeoutPeriodField,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u8DirectionIsReceived,
                asSavedReports[i].sAttributeReportingConfigurationRecord.uAttributeReportableChange.zuint8ReportableChange);
    }
#endif


    /*Save this Records*/
    PDM_eSaveRecordData(PDM_ID_APP_REPORTS,
                        asSavedReports,
                        sizeof(asSavedReports));
}


/****************************************************************************
 *
 * NAME: vSaveReportableRecord
 *
 * DESCRIPTION:
 * Loads a default configuration
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void vSaveReportableRecord(  uint16 u16ClusterID,
                                    tsZCL_AttributeReportingConfigurationRecord* psAttributeReportingConfigurationRecord)
{
    int iIndex;

    if (u16ClusterID == GENERAL_CLUSTER_ID_ONOFF)
    {
        iIndex = REPORT_ONOFF_SLOT;
    }
    else if (u16ClusterID == GENERAL_CLUSTER_ID_LEVEL_CONTROL)
    {
        iIndex = REPORT_LEVEL_SLOT;
    }
#ifdef CLD_COLOUR_CONTROL
    else if (u16ClusterID == LIGHTING_CLUSTER_ID_COLOUR_CONTROL)
    {
        switch (psAttributeReportingConfigurationRecord->u16AttributeEnum)
        {
#ifdef REPORT_HUE
        case E_CLD_COLOURCONTROL_ATTR_CURRENT_HUE:
            iIndex = REPORT_COLOR_HUE_SLOT;
            break;
#endif
#ifdef REPORT_SATURATION
        case E_CLD_COLOURCONTROL_ATTR_CURRENT_SATURATION:
                    iIndex = REPORT_COLOR_SAT_SLOT;
                    break;
#endif
#ifdef REPORT_CURRENT_X
        case E_CLD_COLOURCONTROL_ATTR_CURRENT_X:
                    iIndex = REPORT_COLOR_X_SLOT;
                    break;
#endif
#ifdef REPORT_CURRENT_Y
        case E_CLD_COLOURCONTROL_ATTR_CURRENT_Y:
                    iIndex = REPORT_COLOR_Y_SLOT;
                    break;
#endif
#ifdef REPORT_COL_TEMP
        case E_CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED:
                    iIndex = REPORT_COLOR_COLTEMP_SLOT;
                    break;
#endif
        default:
        return;
        }

    }
#endif  // ifdef CLD_COLOUR_CONTROL
    else
    {
        return;
    }

    DBG_vPrintf(TRACE_REPORT, "Save to report %d\n", iIndex);

    /*For CurrentLevel attribute in LevelControl Cluster*/
    asSavedReports[iIndex].u16ClusterID=u16ClusterID;
    memcpy( &(asSavedReports[iIndex].sAttributeReportingConfigurationRecord),
            psAttributeReportingConfigurationRecord,
            sizeof(tsZCL_AttributeReportingConfigurationRecord) );



    DBG_vPrintf(TRACE_REPORT,"Cluster %04x Type %d Attrib %04x Min %d Max %d IntV %d Direction %d Change %d\n",
            asSavedReports[iIndex].u16ClusterID,
            asSavedReports[iIndex].sAttributeReportingConfigurationRecord.eAttributeDataType,
            asSavedReports[iIndex].sAttributeReportingConfigurationRecord.u16AttributeEnum,
            asSavedReports[iIndex].sAttributeReportingConfigurationRecord.u16MinimumReportingInterval,
            asSavedReports[iIndex].sAttributeReportingConfigurationRecord.u16MaximumReportingInterval,
            asSavedReports[iIndex].sAttributeReportingConfigurationRecord.u16TimeoutPeriodField,
            asSavedReports[iIndex].sAttributeReportingConfigurationRecord.u8DirectionIsReceived,
            asSavedReports[iIndex].sAttributeReportingConfigurationRecord.uAttributeReportableChange.zuint8ReportableChange    );

    /*Save this Records*/
    PDM_eSaveRecordData(PDM_ID_APP_REPORTS,
                        asSavedReports,
                        sizeof(asSavedReports));
}

PRIVATE uint8 u8GetRecordIndex(uint16 u16ClusterID,
		                       uint16 u16AttributeEnum)
{
	uint8 u8Index = 0xFF;

    if (u16ClusterID == GENERAL_CLUSTER_ID_ONOFF)
    {
        u8Index = REPORT_ONOFF_SLOT;
    }
    else if (u16ClusterID == GENERAL_CLUSTER_ID_LEVEL_CONTROL)
    {
        u8Index = REPORT_LEVEL_SLOT;
    }
#ifdef CLD_COLOUR_CONTROL
    else if (u16ClusterID == LIGHTING_CLUSTER_ID_COLOUR_CONTROL)
    {
        switch (u16AttributeEnum)
        {
#ifdef REPORT_HUE
        case E_CLD_COLOURCONTROL_ATTR_CURRENT_HUE:
            u8Index = REPORT_COLOR_HUE_SLOT;
            break;
#endif
#ifdef REPORT_SATURATION
        case E_CLD_COLOURCONTROL_ATTR_CURRENT_SATURATION:
                    u8Index = REPORT_COLOR_SAT_SLOT;
                    break;
#endif
#ifdef REPORT_CURRENT_X
        case E_CLD_COLOURCONTROL_ATTR_CURRENT_X:
                    u8Index = REPORT_COLOR_X_SLOT;
                    break;
#endif
#ifdef REPORT_CURRENT_Y
        case E_CLD_COLOURCONTROL_ATTR_CURRENT_Y:
                    u8Index = REPORT_COLOR_Y_SLOT;
                    break;
#endif
#ifdef REPORT_COL_TEMP
        case E_CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED:
                    u8Index = REPORT_COLOR_COLTEMP_SLOT;
                    break;
#endif
        default:
	        u8Index= 0xff;
        }
    }		
#endif
    else
    {
    	u8Index = 0xFF;
    }

	return u8Index;
}


PUBLIC void vRestoreDefaultRecord(  uint8                                        u8EndPointID,
									uint16                                       u16ClusterID,
									tsZCL_AttributeReportingConfigurationRecord* psAttributeReportingConfigurationRecord)
{
    uint8 u8Index = u8GetRecordIndex(u16ClusterID,psAttributeReportingConfigurationRecord->u16AttributeEnum);

    if(u8Index == 0xFF)
    	return;

    eZCL_CreateLocalReport( u8EndPointID, u16ClusterID, 0, TRUE, &(asDefaultReports[u8Index].sAttributeReportingConfigurationRecord));

    DBG_vPrintf(TRACE_REPORT, "Save to report %d\n", u8Index);

    memcpy( &(asSavedReports[u8Index].sAttributeReportingConfigurationRecord),
    		&(asDefaultReports[u8Index].sAttributeReportingConfigurationRecord),
            sizeof(tsZCL_AttributeReportingConfigurationRecord) );

    DBG_vPrintf(TRACE_REPORT,"Cluster %04x Type %d Attrib %04x Min %d Max %d IntV %d Direction %d Change %d\n",
            asSavedReports[u8Index].u16ClusterID,
            asSavedReports[u8Index].sAttributeReportingConfigurationRecord.eAttributeDataType,
            asSavedReports[u8Index].sAttributeReportingConfigurationRecord.u16AttributeEnum,
            asSavedReports[u8Index].sAttributeReportingConfigurationRecord.u16MinimumReportingInterval,
            asSavedReports[u8Index].sAttributeReportingConfigurationRecord.u16MaximumReportingInterval,
            asSavedReports[u8Index].sAttributeReportingConfigurationRecord.u16TimeoutPeriodField,
            asSavedReports[u8Index].sAttributeReportingConfigurationRecord.u8DirectionIsReceived,
            asSavedReports[u8Index].sAttributeReportingConfigurationRecord.uAttributeReportableChange.zuint8ReportableChange    );

    /*Save this Records*/
    PDM_eSaveRecordData(PDM_ID_APP_REPORTS,
                        asSavedReports,
                        sizeof(asSavedReports));

}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
