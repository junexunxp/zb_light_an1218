/*****************************************************************************
 *
 * MODULE:             JN-AN-1218
 *
 * COMPONENT:          app_start_light.c
 *
 * DESCRIPTION:        ZLO Demo: Light Node Initialisation -Implementation
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
 * Copyright NXP B.V. 2017. All rights reserved
 *
 ***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "pwrm.h"
#include "pdum_nwk.h"
#include "pdum_apl.h"
#include "pdum_gen.h"
#include "PDM.h"
#include "dbg.h"
#include "dbg_uart.h"

#include "zps_gen.h"
#include "zps_apl.h"
#include "zps_apl_af.h"
#include "zps_apl_zdo.h"
#include "string.h"

#include "AppApi.h"
#include "app_zlo_light_node.h"


#include "zcl_options.h"

#include "app_common.h"
#include "app_main.h"
#include "app_light_interpolation.h"
#include "DriverBulb_Shim.h"

#ifdef APP_NTAG_ICODE
#include "ntag_nwk.h"
#include "app_ntag_icode.h"
#endif
#ifdef APP_NTAG_AES
#include "ntag_nwk.h"
#include "app_ntag_aes.h"
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#ifndef DEBUG_APP
#define TRACE_APP   FALSE
#else
#define TRACE_APP   TRUE
#endif

#ifndef DEBUG_START_UP
#define TRACE_START FALSE
#else
#define TRACE_START TRUE
#endif

#define HALT_ON_EXCEPTION      FALSE

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

PRIVATE void APP_vInitialise(void);

#if (defined PDM_EEPROM)
#if TRACE_APP
PRIVATE void vPdmEventHandlerCallback(uint32 u32EventNumber, PDM_eSystemEventCode eSystemEventCode);
#endif
#endif

void vfExtendedStatusCallBack (ZPS_teExtendedStatus eExtendedStatus);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
extern void *_stack_low_water_mark;

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
#ifdef PDM_EEPROM
    extern uint8 u8PDM_CalculateFileSystemCapacity();
    extern uint8 u8PDM_GetFileSystemOccupancy();
#endif
/**
 * Power manager callback.
 * Called just before the device is put to sleep.
 */
PWRM_CALLBACK(PreSleep)
{
    DBG_vPrintf(TRACE_START, "Going to sleep ...\n");
}

/**
 * Power manager callback.
 * Called when the device wakes up.
 */
PWRM_CALLBACK(Wakeup)
{
    DBG_vReset();
    DBG_vPrintf(TRACE_START, "Woken up\n");
}

/**
 * Entry point for application from a cold start.
 */
PUBLIC void vAppMain(void)
{
#if JENNIC_CHIP_FAMILY == JN516x
    // Wait until FALSE i.e. on XTAL  - otherwise uart data will be at wrong speed
    while (bAHI_GetClkSource() == TRUE);
    bAHI_SetClockRate(3); /* Move CPU to 32 MHz  vAHI_OptimiseWaitStates automatically called */
#endif

    /* Initialise the debug diagnostics module to use UART0 at 115K Baud;
     * Do not use UART 1 if LEDs are used, as it shares DIO with the LEDS
     */
    DBG_vUartInit(DBG_E_UART_0, DBG_E_UART_BAUD_RATE_115200);
#ifdef DEBUG_921600
    {
        /* Bump baud rate up to 921600 */
        vAHI_UartSetBaudDivisor(DBG_E_UART_0, 2);
        vAHI_UartSetClocksPerBit(DBG_E_UART_0, 8);
    }
#endif


    /*
     * Initialise the stack overflow exception to trigger if the end of the
     * stack is reached. See the linker command file to adjust the allocated
     * stack size.
     */
#if ( JENNIC_CHIP_FAMILY == JN516x )
    vAHI_SetStackOverflow(TRUE, (uint32)&_stack_low_water_mark);
    DBG_vPrintf(TRACE_START, "\nStack low water mark is at %08x", (uint32)&_stack_low_water_mark);
#endif

    /*
     * Catch resets due to watchdog timer expiry. Comment out to harden code.
     */
    if (bAHI_WatchdogResetEvent())
    {
        DBG_vPrintf(TRACE_APP, "APP: Watchdog timer has reset device!\n");
#if HALT_ON_EXCEPTION
        vAHI_WatchdogStop();
        while (1);
#endif
    }

#ifndef JENNIC_MAC_MiniMacShim
    /* initialise ROM based software modules */
    u32AppApiInit(NULL, NULL, NULL, NULL, NULL, NULL);
#endif


    /* idle task commences here */
    DBG_vPrintf(TRUE, "***********************************************\n");
    DBG_vPrintf(TRUE, "* LIGHT NODE RESET                            *\n");
    DBG_vPrintf(TRUE, "***********************************************\n");

    APP_vSetUpHardware();

    DBG_vPrintf(TRACE_APP, "APP: Entering APP_vInitResources()\n");
    APP_vInitResources();

#if (JENNIC_CHIP_FAMILY == JN517x)
    //DIO2 = SDA
    //DIO3 = SCL
    /* Set DIO lines to outputs */
    vAHI_DioSetDirection(0, (1<<3|1<<2));
    vAHI_DioSetOutput((1<<3|1<<2), 0);
#endif


    /* Early call to Bulb initialisation to enable fast start up    */
    vBULB_Init();

    /*
     * Start the bulb in Off state
     * ZCL start up will turn it on if required
     */
#if (defined CLD_COLOUR_CONTROL)
    vLI_SetCurrentValues(0,0,0,0,0);
#if  !((defined DR1221) || (defined DR1221_Dimic) || (defined OM15045))
    vRGBLight_SetLevels(0,0,0,0,0);
#endif
#if  ((defined DR1221) || (defined DR1221_Dimic) || (defined OM15045))
    vTunableWhiteLightSetLevels(0,0,CLD_COLOURCONTROL_COLOUR_TEMPERATURE_PHY_MAX);
#endif
#elif ( defined MONO_WITH_LEVEL)
    vLI_SetCurrentValues(0, 0, 0, 0, 0 );
    vSetBulbState( 0, 0);
#else
    vSetBulbState( 0);
#endif

    APP_vInitialise();

#if (defined APP_NTAG_ICODE) || (defined APP_NTAG_AES)
    DBG_vPrintf(TRACE_APP, "\nAPP: Entering APP_vNtagPdmLoad()");
    /* Didn't start BDB using PDM data ? */
    if (FALSE == APP_bNtagPdmLoad())
#endif
    {
        DBG_vPrintf(TRACE_APP, "APP: Entering BDB_vStart()\n");
        BDB_vStart();

#ifdef APP_NTAG_AES
        DBG_vPrintf(TRACE_APP, "\nAPP: Entering APP_vNtagStart()");
        APP_vNtagStart(NFC_NWK_LIGHT);
#endif
    }

#ifdef APP_NTAG_ICODE
    /* Not waking from deep sleep ? */
    if (0 == (u16AHI_PowerStatus() & (1 << 11)))
    {
        DBG_vPrintf(TRACE_APP, "\nAPP: Entering APP_vNtagStart()");
        APP_vNtagStart(app_u8GetDeviceEndpoint());
    }
#endif

    DBG_vPrintf(TRACE_APP, "APP: Entering APP_vMainLoop()\n");
    APP_vMainLoop();
}

/**
 * Power manager callback.
 * Called to allow the application to register
 * sleep and wake callbacks.
 */
void vAppRegisterPWRMCallbacks(void)
{
    /* nothing to register as device does not sleep */
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/


/**
 * Initialises Zigbee stack, hardware and application.
 */
PRIVATE void APP_vInitialise(void)
{
    /* Initialise the debug diagnostics module to use UART0 at 115K Baud;
     * Do not use UART 1 if LEDs are used, as it shares DIO with the LEDS
     */
    DBG_vUartInit(DBG_E_UART_0, DBG_E_UART_BAUD_RATE_115200);

    /* Initialise JenOS modules. Initialise Power Manager even on non-sleeping nodes
     * as it allows the device to doze when in the idle task
     */
    PWRM_vInit(E_AHI_SLEEP_OSCON_RAMON);

    /* Initialise the Persistent Data Manager */
    PDM_eInitialise(63);

#if (defined PDM_EEPROM)
#if TRACE_APP
    PDM_vRegisterSystemCallback(vPdmEventHandlerCallback);
#endif
#endif

    /* Initialise Protocol Data Unit Manager */
    PDUM_vInit();
    ZPS_vExtendedStatusSetCallback(vfExtendedStatusCallBack);
	
#if 1 //patrick to init DIO16 & DIO17
		vAHI_DioSetDirection(0, 1<<16|1<<17);
		vAHI_DioSetOutput(0,1<<16|1<<17); // GREEN and RED LED are Off	
#endif	

    /* initialise application */
    APP_vInitialiseNode();
}

#if (defined PDM_EEPROM)
#if TRACE_APP
/****************************************************************************
 *
 * NAME: vPdmEventHandlerCallback
 *
 * DESCRIPTION:
 * Handles PDM callback, information the application of PDM conditions
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vPdmEventHandlerCallback(uint32 u32EventNumber, PDM_eSystemEventCode eSystemEventCode)
{

    switch (eSystemEventCode) {
        /*
         * The next three events will require the application to take some action
         */
        case E_PDM_SYSTEM_EVENT_WEAR_COUNT_TRIGGER_VALUE_REACHED:
            DBG_vPrintf(TRACE_APP, "PDM: Segment %d reached trigger wear level\n", u32EventNumber);
            break;
        case E_PDM_SYSTEM_EVENT_DESCRIPTOR_SAVE_FAILED:
            DBG_vPrintf(TRACE_APP, "PDM: Record Id %d failed to save\n", u32EventNumber);
            DBG_vPrintf(TRACE_APP, "PDM: Capacity %d\n", u8PDM_CalculateFileSystemCapacity() );
            DBG_vPrintf(TRACE_APP, "PDM: Occupancy %d\n", u8PDM_GetFileSystemOccupancy() );
            break;
        case E_PDM_SYSTEM_EVENT_PDM_NOT_ENOUGH_SPACE:
            DBG_vPrintf(TRACE_APP, "PDM: Record %d not enough space\n", u32EventNumber);
            DBG_vPrintf(TRACE_APP, "PDM: Capacity %d\n", u8PDM_CalculateFileSystemCapacity() );
            DBG_vPrintf(TRACE_APP, "PDM: Occupancy %d\n", u8PDM_GetFileSystemOccupancy() );
            break;

        /*
         *  The following events are really for information only
         */
        case E_PDM_SYSTEM_EVENT_EEPROM_SEGMENT_HEADER_REPAIRED:
            DBG_vPrintf(TRACE_APP, "PDM: Segment %d header repaired\n", u32EventNumber);
            break;
        case E_PDM_SYSTEM_EVENT_SYSTEM_INTERNAL_BUFFER_WEAR_COUNT_SWAP:
            DBG_vPrintf(TRACE_APP, "PDM: Segment %d buffer wear count swap\n", u32EventNumber);
            break;
        case E_PDM_SYSTEM_EVENT_SYSTEM_DUPLICATE_FILE_SEGMENT_DETECTED:
            DBG_vPrintf(TRACE_APP, "PDM: Segement %d duplicate selected\n", u32EventNumber);
            break;
        default:
            DBG_vPrintf(TRACE_APP, "PDM: Unexpected call back Code %d Number %d\n", eSystemEventCode, u32EventNumber);
            break;
    }
}
#endif
#endif


PUBLIC void vDebug(char *pcMessage)
{
    DBG_vPrintf(TRACE_START, "%s",pcMessage);
}

PUBLIC void vDebugHex(uint32 u32Data, int iSize)
{
    DBG_vPrintf(TRACE_START, "%x",u32Data);
}


void vfExtendedStatusCallBack (ZPS_teExtendedStatus eExtendedStatus)
{
    DBG_vPrintf(TRUE,"ERROR: Extended status %x\n", eExtendedStatus);
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
