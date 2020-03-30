/*****************************************************************************
 *
 * MODULE:             JN-AN-1218
 *
 * COMPONENT:          app_buttons.c
 *
 * DESCRIPTION:        ZLO Demo: DK button handler -Implementation
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
#include "dbg.h"
#include "AppHardwareApi.h"
#include "app_events.h"

#include "pwrm.h"
#include "app_buttons.h"
#include "app_main.h"
#include "ZTimer.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef TRACE_APP_BUTTON
#define TRACE_APP_BUTTON               FALSE
#endif




/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
#if (defined APP_NTAG_ICODE) || (defined APP_NTAG_AES)
#if (defined BUTTON_MAP_DR1175)
    PRIVATE uint8 s_u8ButtonDebounce[APP_BUTTONS_NUM] = { 0xff, 0xff };
    PRIVATE uint8 s_u8ButtonState[APP_BUTTONS_NUM] = { 0, 0 };
    PRIVATE const uint8 s_u8ButtonDIOLine[APP_BUTTONS_NUM] =
    {
        APP_BUTTONS_BUTTON_1,
        APP_BUTTONS_NFC_FD
    };
#elif (defined BUTTON_MAP_OM15045) || (defined BUTTON_MAP_OM15053)
    PRIVATE uint8 s_u8ButtonDebounce[APP_BUTTONS_NUM] = { 0xff };
    PRIVATE uint8 s_u8ButtonState[APP_BUTTONS_NUM] = { 0 };
    PRIVATE const uint8 s_u8ButtonDIOLine[APP_BUTTONS_NUM] =
    {
        APP_BUTTONS_NFC_FD
    };
#else
    PRIVATE uint8 s_u8ButtonDebounce[APP_BUTTONS_NUM] = { 0xff, 0xff, 0xff };
    PRIVATE uint8 s_u8ButtonState[APP_BUTTONS_NUM] = { 0, 0, 0 };
    PRIVATE const uint8 s_u8ButtonDIOLine[APP_BUTTONS_NUM] =
    {
        APP_BUTTONS_BUTTON_1,
        APP_BUTTONS_BUTTON_2,
        APP_BUTTONS_NFC_FD
    };
#endif
#else
#if (defined BUTTON_MAP_DR1175)
    PRIVATE uint8 s_u8ButtonDebounce[APP_BUTTONS_NUM] = { 0xff };
    PRIVATE uint8 s_u8ButtonState[APP_BUTTONS_NUM] = { 0 };
    PRIVATE const uint8 s_u8ButtonDIOLine[APP_BUTTONS_NUM] =
    {
        APP_BUTTONS_BUTTON_1
    };
#else
    PRIVATE uint8 s_u8ButtonDebounce[APP_BUTTONS_NUM] = { 0xff, 0xff };
    PRIVATE uint8 s_u8ButtonState[APP_BUTTONS_NUM] = { 0, 0 };
    PRIVATE const uint8 s_u8ButtonDIOLine[APP_BUTTONS_NUM] =
    {
        APP_BUTTONS_BUTTON_1,
        APP_BUTTONS_BUTTON_2
    };
#endif
#endif

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 *
 * NAME: APP_bButtonInitialise
 *
 * DESCRIPTION:
 * Button Initialization
 *
 * PARAMETER: void
 *
 * RETURNS: bool
 *
 ****************************************************************************/
PUBLIC bool_t APP_bButtonInitialise(void)
{
    /* Set DIO lines to inputs with buttons connected */
    vAHI_DioSetDirection(APP_BUTTONS_DIO_MASK, 0);

    /* Turn on pull-ups for DIO lines with buttons connected */
    vAHI_DioSetPullup(APP_BUTTONS_DIO_MASK, 0);

#if (defined BUTTON_MAP_OM15045)
    vAHI_DioSetPullupDirection(APP_BUTTONS_DIO_MASK, 0);
#endif

    /* Set the edge detection for falling edges */
    vAHI_DioInterruptEdge(0, APP_BUTTONS_DIO_MASK);

    /* Enable interrupts to occur on selected edge */
    vAHI_DioInterruptEnable(APP_BUTTONS_DIO_MASK, 0);

    uint32 u32Buttons = u32AHI_DioReadInput() & APP_BUTTONS_DIO_MASK;
    if (u32Buttons != APP_BUTTONS_DIO_MASK)
    {
        return TRUE;
    }
    return FALSE;
}


/**
 * ISR called on DIO interrupt
 */
#if (JENNIC_CHIP_FAMILY == JN516x)
PUBLIC void vISR_SystemController(void)
{
    /* clear pending DIO changed bits by reading register */
    (void) u32AHI_DioInterruptStatus();

    /* disable edge detection until scan complete */
    vAHI_DioInterruptEnable(0, APP_BUTTONS_DIO_MASK);

    ZTIMER_eStart(u8TimerButtonScan, ZTIMER_TIME_MSEC(10));
}
#endif

#if (JENNIC_CHIP_FAMILY == JN517x)
PUBLIC void vISR_SystemController(uint32 u32DeviceId, uint32 u32BitMap)
{
    if(u32BitMap & APP_BUTTONS_DIO_MASK)
    {
        /* disable edge detection until scan complete */
        vAHI_DioInterruptEnable(0, APP_BUTTONS_DIO_MASK);

        ZTIMER_eStart(u8TimerButtonScan, ZTIMER_TIME_MSEC(10));
    }
}
#endif

/****************************************************************************
 *
 * NAME: APP_cbTimerButtonScan
 *
 * DESCRIPTION:
 * Timer callback to debounce the button presses
 *
 * PARAMETER:
 *
 * RETURNS:
 *
 ****************************************************************************/
PUBLIC void APP_cbTimerButtonScan(void *pvParam)
{
    /*
     * The DIO changed status register is reset here before the scan is performed.
     * This avoids a race condition between finishing a scan and re-enabling the
     * DIO to interrupt on a falling edge.
     */
    (void) u32AHI_DioInterruptStatus();

    uint8 u8AllReleased = 0xff;
    unsigned int i;
    uint32 u32DIOState = u32AHI_DioReadInput() & APP_BUTTONS_DIO_MASK;


    for (i = 0; i < APP_BUTTONS_NUM; i++)
    {
        uint8 u8Button = (uint8) ((u32DIOState >> s_u8ButtonDIOLine[i]) & 1);

        s_u8ButtonDebounce[i] <<= 1;
        s_u8ButtonDebounce[i] |= u8Button;
        u8AllReleased &= s_u8ButtonDebounce[i];

        if (0 == s_u8ButtonDebounce[i] && !s_u8ButtonState[i])
        {
            s_u8ButtonState[i] = TRUE;

            /*
             * button consistently depressed for 8 scan periods
             * so post message to application task to indicate
             * a button down event
             */
            APP_tsLightEvent sButtonEvent;
            sButtonEvent.eType = APP_E_EVENT_BUTTON_DOWN;
            sButtonEvent.uEvent.sButton.u8Button = i;

            //DBG_vPrintf(TRACE_APP_BUTTON, "Button DN=%d\n", i);

            ZQ_bQueueSend(&APP_msgAppEvents, &sButtonEvent);
        }
        else if (0xff == s_u8ButtonDebounce[i] && s_u8ButtonState[i] != FALSE)
        {
            s_u8ButtonState[i] = FALSE;

            /*
             * button consistently released for 8 scan periods
             * so post message to application task to indicate
             * a button up event
             */
            APP_tsLightEvent sButtonEvent;
            sButtonEvent.eType = APP_E_EVENT_BUTTON_UP;
            sButtonEvent.uEvent.sButton.u8Button = i;

            //DBG_vPrintf(TRACE_APP_BUTTON, "Button UP=%i\n", i);

            ZQ_bQueueSend(&APP_msgAppEvents, &sButtonEvent);
        }
    }

    if (0xff == u8AllReleased)
    {
        /*
         * all buttons high so set dio to interrupt on change
         */
        //DBG_vPrintf(TRACE_APP_BUTTON, "ALL UP\n", i);
        vAHI_DioInterruptEnable(APP_BUTTONS_DIO_MASK, 0);
    }
    else
    {
        /*
         * one or more buttons is still depressed so continue scanning
         */
        ZTIMER_eStop(u8TimerButtonScan);
        ZTIMER_eStart(u8TimerButtonScan, ZTIMER_TIME_MSEC(10));
    }
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
