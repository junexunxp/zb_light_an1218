/*****************************************************************************
 *
 * MODULE:             JN-AN-1218
 *
 * COMPONENT:          app_main.c
 *
 * DESCRIPTION:        Light bulb application main file
 *
 *****************************************************************************
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
 * Copyright NXP B.V. 2017. All rights reserved
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <stdint.h>
#include "jendefs.h"
#include "ZQueue.h"
#include "ZTimer.h"
#include "portmacro.h"
#include "zps_apl_af.h"
#include "mac_vs_sap.h"
#include "AppHardwareApi.h"
#include "dbg.h"
#include "app_main.h"
#include "app_buttons.h"
#include "app_events.h"
#include "app_zcl_light_task.h"

#include "app_manage_temperature.h"
#include "PDM.h"
#include "app_zlo_light_node.h"
#include "app_power_on_counter.h"

//#include "pdum_gen.h"

#ifdef Light_ColorLight
#include "App_Light_ColorLight.h"
#endif

#ifdef Light_DimmableLight
#include "App_Light_DimmableLight.h"
#endif

#ifdef Light_DimmablePlug
#include "App_Light_DimmablePlug.h"
#endif

#ifdef Light_ExtendedColorLight
#include "App_Light_ExtendedColorLight.h"
#endif

#ifdef Light_ColorTemperatureLight
#include "APP_Light_ColorTemperatureLight.h"
#endif

#ifdef Light_OnOffLight
#include "APP_Light_OnOffLight.h"
#endif

#ifdef Light_OnOffPlug
#include "APP_Light_OnOffPlug.h"
#endif

#ifdef APP_NTAG_ICODE
#include "app_ntag_icode.h"
#endif
#ifdef APP_NTAG_AES
#include "app_ntag_aes.h"
#endif

#if JENNIC_CHIP_FAMILY == JN517x
#define NVIC_INT_PRIO_LEVEL_SYSCTRL (13)
#define NVIC_INT_PRIO_LEVEL_BBC     (7)
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#ifndef DEBUG_APP
#define TRACE_APP   FALSE
#else
#define TRACE_APP   TRUE
#endif

#define TIMER_QUEUE_SIZE             8
#define MLME_QUEQUE_SIZE             8
#define MCPS_QUEUE_SIZE             20
#define ZPS_QUEUE_SIZE                  1
#define BDB_QUEUE_SIZE               2
#define APP_QUEUE_SIZE                  8
#define MCPS_DCFM_QUEUE_SIZE 		5

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/


PUBLIC uint8 u8TimerButtonScan;
PUBLIC uint8 u8TimerRadioRecal;
PUBLIC uint8 u8TimerTick;
PUBLIC uint8 u8TimerPowerOn;

#define APP_NUM_STD_TMRS            4

#ifdef CLD_GREENPOWER
    PUBLIC uint8 u8GPTimerTick;
    #define APP_NUM_GP_TMRS             1
    #define GP_TIMER_QUEUE_SIZE         2
#else
    #define APP_NUM_GP_TMRS             0
#endif

#if (defined APP_NTAG_ICODE) || (defined APP_NTAG_AES)
    PUBLIC uint8 u8TimerNtag;
    #define APP_NUM_NTAG_TMRS          1
#else
    #define APP_NUM_NTAG_TMRS          0
#endif

#ifdef OTA_CLD_ATTR_REQUEST_DELAY
    PUBLIC uint8 u8TimerZclMsTick;
    #define APP_NUM_ZCL_MS_TMRS         1
#else
    #define APP_NUM_ZCL_MS_TMRS         0
#endif

#define APP_ZTIMER_STORAGE   (APP_NUM_STD_TMRS + APP_NUM_GP_TMRS + APP_NUM_NTAG_TMRS + APP_NUM_ZCL_MS_TMRS)

PUBLIC tszQueue APP_msgBdbEvents;
PUBLIC tszQueue APP_msgAppEvents;


/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

PRIVATE ZTIMER_tsTimer asTimers[APP_ZTIMER_STORAGE + BDB_ZTIMER_STORAGE + 2]; //added +2 by patrick

PRIVATE zps_tsTimeEvent asTimeEvent[TIMER_QUEUE_SIZE];
PRIVATE MAC_tsMcpsVsDcfmInd asMacMcpsDcfmInd[MCPS_QUEUE_SIZE];
PRIVATE MAC_tsMlmeVsDcfmInd asMacMlmeVsDcfmInd[MLME_QUEQUE_SIZE];
PRIVATE BDB_tsZpsAfEvent asBdbEvent[BDB_QUEUE_SIZE];
PRIVATE MAC_tsMcpsVsCfmData asMacMcpsDcfm[MCPS_DCFM_QUEUE_SIZE];


PRIVATE APP_tsLightEvent asAppEvent[APP_QUEUE_SIZE];
#ifdef CLD_GREENPOWER
PUBLIC tszQueue APP_msgGPZCLTimerEvents;
uint8 au8GPZCLEvent[ GP_TIMER_QUEUE_SIZE];
uint8 u8GPZCLTimerEvent;
#endif
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

extern void zps_taskZPS(void);
extern void PWRM_vManagePower(void);

#if 1 //patrick
PUBLIC void TurnOnGreen(void)
{
	vAHI_DioSetOutput(1<<16 , 1<<17);
//	uint32 DIOStatus=u32AHI_DioReadInput();
//	DBG_vPrintf(1,"\n TurnOnRed DIO status =%x",DIOStatus);
}

PUBLIC void TurnOnRed(void)
{
	vAHI_DioSetOutput(1<<17 , 1<<16);
//	uint32 DIOStatus=u32AHI_DioReadInput();
//	DBG_vPrintf(1,"\n TurnOnYellow DIO status =%x",DIOStatus);
}

PUBLIC void TurnOffGreen(void)
{
	vAHI_DioSetOutput(0, 1<<16|1<<17);
//	uint32 DIOStatus=u32AHI_DioReadInput();
//	DBG_vPrintf(1,"\n TurnOnRed DIO status =%x",DIOStatus);
}

PUBLIC void TurnOffRed(void)
{
	//vAHI_DioSetOutput(1<<17|1<<16,0);
	vAHI_DioSetOutput(0, 1<<16|1<<17);
//	uint32 DIOStatus=u32AHI_DioReadInput();
//	DBG_vPrintf(1,"\n TurnOnYellow DIO status =%x",DIOStatus);
}

PUBLIC void NwkCh_MACAddr(void)
{
	uint32 u32Channel;
	uint16 NwkAddr=ZPS_u16AplZdoGetNwkAddr();
	uint64 MacAddr=ZPS_u64AplZdoGetIeeeAddr();
	
	if (eAppApiPlmeGet(PHY_PIB_ATTR_CURRENT_CHANNEL, &u32Channel)== PHY_ENUM_SUCCESS)
		DBG_vPrintf(1,"\n Nwk Channel=%d,Short Addr=0x%04x,MAC Addr=0x%016llx,Ver=2.0,Type=DimmableLight\n",u32Channel,NwkAddr,MacAddr);
}

extern PUBLIC void FactoryNew(void);

PUBLIC void SWRst(void) //Reset Node
{
	vAHI_SwReset();
}

PUBLIC void vDisplayBindingTable( void )
{
    uint32   j = 0;
    uint64 u64Addr;

    ZPS_tsAplAib * tsAplAib  = ZPS_psAplAibGetAib();

    DBG_vPrintf(1, "\n Bind Size %d",  tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].u32SizeOfBindingTable );

    for( j = 0 ; j < tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].u32SizeOfBindingTable ; j++ )
    {
        if (tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[j].u8DstAddrMode == ZPS_E_ADDR_MODE_GROUP)
        {
            // Group
            DBG_vPrintf(1, "\n Group Addr 0x%x, ", tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[j].u16AddrOrLkUp);
            DBG_vPrintf(1, "Dest Ep %d, ", tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[j].u8DestinationEndPoint);
            DBG_vPrintf(1, "Cluster Id 0x%x ", tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[j].u16ClusterId);
        }
        else
        {
            u64Addr = ZPS_u64NwkNibGetMappedIeeeAddr( ZPS_pvAplZdoGetNwkHandle(), tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[j].u16AddrOrLkUp);
            DBG_vPrintf(1, "\n MAC addr 0x%x %x, ", (uint32)(u64Addr>>32), (uint32)(u64Addr&0xffffffff));
            DBG_vPrintf(1, " Dest EP %d, ", tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[j].u8DestinationEndPoint);
            DBG_vPrintf(1, "Cluster Id 0x%x ", tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[j].u16ClusterId);
        }
    }
    DBG_vPrintf(1, "\n");
}

PUBLIC void vDisplayAddressMapTable(void)
{
    ZPS_tsNwkNib * thisNib;
    thisNib = ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle());

    uint8 i = 0;

    DBG_vPrintf(1,"\n Address Map Size: %d", thisNib->sTblSize.u16AddrMap);

    for( i=0;i<thisNib->sTblSize.u16AddrMap;i++)
    {
        DBG_vPrintf(1,"\n Short Addr: %04x, Ext Addr: %016llx,", thisNib->sTbl.pu16AddrMapNwk[i], ZPS_u64NwkNibGetMappedIeeeAddr(ZPS_pvAplZdoGetNwkHandle(),thisNib->sTbl.pu16AddrLookup[i]));
    }
}

PUBLIC void vDisplayNT( void )
{
    ZPS_tsNwkNib * thisNib = ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle());
    uint8 i;

    DBG_vPrintf(1, "\n NT Size: %d\n", thisNib->sTblSize.u16NtActv);

    for( i = 0 ; i < thisNib->sTblSize.u16NtActv ; i++ )
    {
        DBG_vPrintf(1, "SAddr: 0x%04x - ExtAddr: 0x%016llx - LQI: %i - Failed TX's: %i - Auth: %i - %i %i %i %i %i %i - Active: %i - %i %i %i\n",
                    thisNib->sTbl.psNtActv[i].u16NwkAddr,
                    ZPS_u64NwkNibGetMappedIeeeAddr(ZPS_pvAplZdoGetNwkHandle(),thisNib->sTbl.psNtActv[i].u16Lookup),
                    thisNib->sTbl.psNtActv[i].u8LinkQuality,
                    thisNib->sTbl.psNtActv[i].u8TxFailed,
                    thisNib->sTbl.psNtActv[i].uAncAttrs.bfBitfields.u1Authenticated,
                    thisNib->sTbl.psNtActv[i].uAncAttrs.bfBitfields.u1DeviceType,
                    thisNib->sTbl.psNtActv[i].uAncAttrs.bfBitfields.u1ExpectAnnc,
                    thisNib->sTbl.psNtActv[i].uAncAttrs.bfBitfields.u1LinkStatusDone,
                    thisNib->sTbl.psNtActv[i].uAncAttrs.bfBitfields.u1PowerSource,
                    thisNib->sTbl.psNtActv[i].uAncAttrs.bfBitfields.u1RxOnWhenIdle,
                    thisNib->sTbl.psNtActv[i].uAncAttrs.bfBitfields.u1SecurityMode,
                    thisNib->sTbl.psNtActv[i].uAncAttrs.bfBitfields.u1Used,
                    thisNib->sTbl.psNtActv[i].uAncAttrs.bfBitfields.u2Relationship,
                    thisNib->sTbl.psNtActv[i].u8Age,
                    thisNib->sTbl.psNtActv[i].uAncAttrs.bfBitfields.u3OutgoingCost
                    );
    }
}

PUBLIC void vClearNTEntry( uint64 u64AddressToRemove )
{

    ZPS_tsNwkNib * thisNib = ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle());
    uint8 i;

    DBG_vPrintf(1, "\n NT Size: %d\n", thisNib->sTblSize.u16NtActv);

    for( i = 0 ; i < thisNib->sTblSize.u16NtActv ; i++ )
    {

        if(ZPS_u64NwkNibGetMappedIeeeAddr(ZPS_pvAplZdoGetNwkHandle(),thisNib->sTbl.psNtActv[i].u16Lookup) == u64AddressToRemove)
        {
             memset( &thisNib->sTbl.psNtActv[i], 0, sizeof(ZPS_tsNwkActvNtEntry) );
             thisNib->sTbl.psNtActv[i].u16NwkAddr = ZPS_NWK_INVALID_NWK_ADDR;
             thisNib->sTbl.psNtActv[i].u16Lookup = 0xFFFF;
        }
    }
}

PUBLIC void ClearAllNTEntry(void)
{
    ZPS_tsNwkNib * thisNib = ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle());
    uint8 i;
	uint64 IeeeAddr;
	
    for( i = 0 ; i < thisNib->sTblSize.u16NtActv ; i++ )
    {
    	IeeeAddr = ZPS_u64NwkNibGetMappedIeeeAddr(ZPS_pvAplZdoGetNwkHandle(),thisNib->sTbl.psNtActv[i].u16Lookup);
		if (IeeeAddr)
		{
             memset( &thisNib->sTbl.psNtActv[i], 0, sizeof(ZPS_tsNwkActvNtEntry) );
             thisNib->sTbl.psNtActv[i].u16NwkAddr = ZPS_NWK_INVALID_NWK_ADDR;
             thisNib->sTbl.psNtActv[i].u16Lookup = 0xFFFF;			
		}
    }
}

PUBLIC void vDisplayRoutingTable(void)
{
    ZPS_tsNwkNib * thisNib;
    thisNib = ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle());

    uint8 i = 0;

    DBG_vPrintf(1,"\n Routing Table Size %d\n", thisNib->sTblSize.u16Rt);

    for( i=0;i<thisNib->sTblSize.u16Rt;i++)
    {
        DBG_vPrintf(1,"Status: %d, Short Address: %02x, Next Hop: %02x\n",
                thisNib->sTbl.psRt[i].uAncAttrs.bfBitfields.u3Status,
                thisNib->sTbl.psRt[i].u16NwkDstAddr,
                thisNib->sTbl.psRt[i].u16NwkNxtHopAddr);
    }
}

PUBLIC void vClearRoutingTable(void)
{
    ZPS_tsNwkNib * thisNib;
    thisNib = ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle());

    uint8 i = 0;

    DBG_vPrintf(1,"\n Routing Table Size %d", thisNib->sTblSize.u16Rt);

    for( i=0;i<thisNib->sTblSize.u16Rt;i++)
    {
        thisNib->sTbl.psRt[i].uAncAttrs.bfBitfields.u3Status = ZPS_NWK_RT_INACTIVE;
        thisNib->sTbl.psRt[i].u16NwkDstAddr = 0xfffe;
        thisNib->sTbl.psRt[i].u16NwkNxtHopAddr = 0;
     }
}

PUBLIC void vDisplayRouteRecordTable(void)
{
    ZPS_tsNwkNib * thisNib;
    thisNib = ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle());

    uint8 i, j = 0;

    DBG_vPrintf(1, "\n RRT Size: %d\n", thisNib->sTblSize.u16Rct);

    for( i=0;i<thisNib->sTblSize.u16Rct;i++)
    {
        DBG_vPrintf(1,"\n Relay Count: %i NwkdstAddr: 0x%04x", thisNib->sTbl.psRct[i].u8RelayCount, thisNib->sTbl.psRct[i].u16NwkDstAddr);
        for ( j = 0 ; j < thisNib->u8MaxSourceRoute  ; j++)
        {
             DBG_vPrintf(1,"\n Path[%i]: %i", j, thisNib->sTbl.psRct[i].au16Path[j]);
        }
    }
}

PUBLIC void vPrintAPSTable(void)
{
    uint8 i;
    uint8 j;

    ZPS_tsAplAib * tsAplAib;

    tsAplAib = ZPS_psAplAibGetAib();

    for ( i = 0 ; i < (tsAplAib->psAplDeviceKeyPairTable->u16SizeOfKeyDescriptorTable + 1) ; i++ )
    {
        DBG_vPrintf(TRUE, "%d MAC: %016llx Key: ", i, ZPS_u64NwkNibGetMappedIeeeAddr(ZPS_pvAplZdoGetNwkHandle(),tsAplAib->psAplDeviceKeyPairTable->psAplApsKeyDescriptorEntry[i].u16ExtAddrLkup));
        for(j=0; j<16;j++)
        {
            DBG_vPrintf(TRUE, "%02x ", tsAplAib->psAplDeviceKeyPairTable->psAplApsKeyDescriptorEntry[i].au8LinkKey[j]);
        }
        DBG_vPrintf(TRUE, "\n");
    }
}

PUBLIC void vDisplayDiscNT(void)
{
    ZPS_tsNwkNib * thisNib;
    uint8 i;

    thisNib = ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle());

    DBG_vPrintf(1, "\n Disc NT Size = %d\n", thisNib->sTblSize.u8NtDisc);

    for( i = 0; i < thisNib->sTblSize.u8NtDisc; i++)
    {
        DBG_vPrintf(1, "\nIndex: %d", i );

        DBG_vPrintf(1, " EPID: %016llx", thisNib->sTbl.psNtDisc[i].u64ExtPanId);

        DBG_vPrintf(1, " PAN: %04x", thisNib->sTbl.psNtDisc[i].u16PanId);

        DBG_vPrintf(1, " SAddr: %04x", thisNib->sTbl.psNtDisc[i].u16NwkAddr);

        DBG_vPrintf(1, " LQI %d\n", thisNib->sTbl.psNtDisc[i].u8LinkQuality);

        DBG_vPrintf(1, " CH: %d", thisNib->sTbl.psNtDisc[i].u8LogicalChan);

        DBG_vPrintf(1, " PJ: %d", thisNib->sTbl.psNtDisc[i].uAncAttrs.bfBitfields.u1JoinPermit);

        DBG_vPrintf(1, " Coord: %d", thisNib->sTbl.psNtDisc[i].uAncAttrs.bfBitfields.u1PanCoord);

        DBG_vPrintf(1, " RT Cap: %d", thisNib->sTbl.psNtDisc[i].uAncAttrs.bfBitfields.u1ZrCapacity);

        DBG_vPrintf(1, " ED Cap: %d", thisNib->sTbl.psNtDisc[i].uAncAttrs.bfBitfields.u1ZedCapacity);

        DBG_vPrintf(1, " Depth: %d", thisNib->sTbl.psNtDisc[i].uAncAttrs.bfBitfields.u4Depth);

        DBG_vPrintf(1, " StPro: %d", thisNib->sTbl.psNtDisc[i].uAncAttrs.bfBitfields.u4StackProfile);

        DBG_vPrintf(1, " PP: %d\r\n", thisNib->sTbl.psNtDisc[i].uAncAttrs.bfBitfields.u1PotentialParent);
    }
}

PUBLIC void vDisplayChildTable(void)
{
    uint8 i;
    ZPS_tsNwkNib * thisNib;
    thisNib = ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle());
    DBG_vPrintf(1, "\n Child Table Size = %d\n", thisNib->sTblSize.u8ChildTable);
}

PUBLIC void FB_Target_Identify(void)
{
	if (sZllState.eNodeState == E_RUNNING)
	{
		BDB_eNsStartNwkSteering();
		BDB_eFbTriggerAsTarget(app_u8GetDeviceEndpoint());
#ifdef APP_TOUCHLINK_ENABLED
		sBDB.sAttrib.bTLStealNotAllowed = FALSE;
#endif
#ifdef CLD_GREENPOWER
		vApp_GP_EnterCommissioningMode();
#endif
	}
	else
		DBG_vPrintf(1, "\n Find & Bind node must already join nwk !!!\n");
}





void ConsoleIn(void)
{

	uint16 UartFifoLevel=u16AHI_UartReadRxFifoLevel(E_AHI_UART_0);
	uint8 u8status;
    uint8 u8SeqNum = 0;
	if (UartFifoLevel-->0)
	{
		uint8 ch=u8AHI_UartReadData(E_AHI_UART_0);
	//	DBG_vPrintf(1,"\n *** Uart Read %x ",ch);

	switch (ch)
	{
		case '0':
			vDisplayAddressMapTable();
			break;
	
		case '1':
			vDisplayBindingTable();
			break;

		case '2':
			vDisplayRoutingTable();
			break;
	
		case '3':
			vDisplayRouteRecordTable();
			break;

		case '4':
			vDisplayNT();
			break;

		case '5':
			vDisplayDiscNT();
			break;

		case '6':
			vPrintAPSTable();
			break;

		case '7':
			vDisplayChildTable();
			break;

		case 'a':
			vClearRoutingTable();
			break;

		case 'b':
			ClearAllNTEntry();
			break;	

		case 'c':
			FB_Target_Identify();
			break;

		case 'd': //Local Identify : fix to 3 times
			LocalIdentify=1;
 			GREENLedIdentCount=6; // double times 2x3=6
			ZTIMER_eStart (GREENLEDBlinkTimer,1); 
			break;

		case 'f':  //factory new
			FactoryNew();
			break;	
			
		case 'i':  // get node infomation : Channel number and Short & IEEE Address
			NwkCh_MACAddr();
			break;				

		case 'r':
			SWRst();
			break;

		case 'v':
			DBG_vPrintf(1,"\n Dimmable Light v1.0\n");
			break;


			break;

		default:
			break;
	}
}	
}

#endif
/****************************************************************************
 *
 * NAME: APP_vMainLoop
 *
 * DESCRIPTION:
 * Main application loop
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void APP_vMainLoop(void)
{

    while (TRUE)
    {
		ConsoleIn();//patrick 
        DBG_vPrintf(FALSE, "APP: Entering zps_taskZPS\n");
        zps_taskZPS();
        DBG_vPrintf(FALSE, "APP: Entering bdb_taskBDB\n");
        bdb_taskBDB();
        DBG_vPrintf(FALSE, "APP: Entering ZTIMER_vTask\n");
        ZTIMER_vTask();

        DBG_vPrintf(FALSE, "APP: Entering APP_taskLight\n");
        APP_taskLight();

        /* Re-load the watch-dog timer. Execution must return through the idle
         * task before the CPU is suspended by the power manager. This ensures
         * that at least one task / ISR has executed within the watchdog period
         * otherwise the system will be reset.
         */
        vAHI_WatchdogRestart();

        /*
         * suspends CPU operation when the system is idle or puts the device to
         * sleep if there are no activities in progress
         */
        PWRM_vManagePower();
    }
}


/****************************************************************************
 *
 * NAME: APP_vSetUpHardware
 *
 * DESCRIPTION:
 * Set up interrupts
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void APP_vSetUpHardware(void)
{
#if (JENNIC_CHIP_FAMILY == JN517x)

    vAHI_SysCtrlRegisterCallback ( vISR_SystemController );
    u32AHI_Init();
    vAHI_InterruptSetPriority ( MICRO_ISR_MASK_BBC,        NVIC_INT_PRIO_LEVEL_BBC );
    vAHI_InterruptSetPriority ( MICRO_ISR_MASK_SYSCTRL, NVIC_INT_PRIO_LEVEL_SYSCTRL );
#endif


#if (JENNIC_CHIP_FAMILY == JN516x)
    TARGET_INITIALISE();
    /* clear interrupt priority level  */
    SET_IPL(0);
    portENABLE_INTERRUPTS();
#endif
}


/****************************************************************************
 *
 * NAME: APP_vInitResources
 *
 * DESCRIPTION:
 * Initialise resources (timers, queue's etc)
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void APP_vInitResources(void)
{

       DBG_vPrintf(TRACE_APP, "APP: Initialising resources...\n");

    /* Initialise the Z timer module */
    ZTIMER_eInit(asTimers, sizeof(asTimers) / sizeof(ZTIMER_tsTimer));


    /* Create Z timers */
    ZTIMER_eOpen(&u8TimerButtonScan,    APP_cbTimerButtonScan,      NULL, ZTIMER_FLAG_PREVENT_SLEEP);
    ZTIMER_eOpen(&u8TimerRadioRecal,    APP_cbTimerRadioRecal,      NULL, ZTIMER_FLAG_PREVENT_SLEEP);
    ZTIMER_eOpen(&u8TimerTick,          APP_cbTimerZclTick,         NULL, ZTIMER_FLAG_PREVENT_SLEEP);
    ZTIMER_eOpen(&u8TimerPowerOn,       APP_cbTimerPowerCount,      NULL, ZTIMER_FLAG_PREVENT_SLEEP);
    #ifdef CLD_GREENPOWER
        ZTIMER_eOpen(&u8GPTimerTick,    APP_cbTimerGPZclTick,       NULL, ZTIMER_FLAG_PREVENT_SLEEP);
    #endif
    #if (defined APP_NTAG_ICODE) || (defined APP_NTAG_AES)
        ZTIMER_eOpen(&u8TimerNtag,      APP_cbNtagTimer,            NULL, ZTIMER_FLAG_PREVENT_SLEEP);
    #endif
    #ifdef OTA_CLD_ATTR_REQUEST_DELAY
        ZTIMER_eOpen(&u8TimerZclMsTick, APP_cbTimerZclMsTick,       NULL, ZTIMER_FLAG_PREVENT_SLEEP);
    #endif

#if 1 //patrick
		ZTIMER_eOpen(&REDLEDBlinkTimer, 	 APP_cbBlinkRed,	  NULL, ZTIMER_FLAG_PREVENT_SLEEP); 
		ZTIMER_eOpen(&GREENLEDBlinkTimer,	 APP_cbBlinkGreen,	  NULL, ZTIMER_FLAG_PREVENT_SLEEP); 
#endif

    /* Create all the queues */
    ZQ_vQueueCreate(&APP_msgBdbEvents,        BDB_QUEUE_SIZE,         sizeof(BDB_tsZpsAfEvent),   (uint8*)asBdbEvent);
    ZQ_vQueueCreate(&APP_msgAppEvents,         APP_QUEUE_SIZE,            sizeof(APP_tsLightEvent),    (uint8*)asAppEvent);
    ZQ_vQueueCreate(&zps_msgMlmeDcfmInd,         MLME_QUEQUE_SIZE,        sizeof(MAC_tsMlmeVsDcfmInd),(uint8*)asMacMlmeVsDcfmInd);
    ZQ_vQueueCreate(&zps_msgMcpsDcfmInd,         MCPS_QUEUE_SIZE,        sizeof(MAC_tsMcpsVsDcfmInd),(uint8*)asMacMcpsDcfmInd);
    ZQ_vQueueCreate(&zps_TimeEvents,            TIMER_QUEUE_SIZE,        sizeof(zps_tsTimeEvent),    (uint8*)asTimeEvent);
    ZQ_vQueueCreate(&zps_msgMcpsDcfm, MCPS_DCFM_QUEUE_SIZE,				sizeof(MAC_tsMcpsVsCfmData),(uint8*)asMacMcpsDcfm);
#ifdef CLD_GREENPOWER
    ZQ_vQueueCreate(&APP_msgGPZCLTimerEvents,        GP_TIMER_QUEUE_SIZE,         sizeof(uint8),   (uint8*)au8GPZCLEvent);
#endif
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
