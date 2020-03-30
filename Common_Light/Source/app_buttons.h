/*****************************************************************************
 *
 * MODULE:             JN-AN-1218
 *
 * COMPONENT:          app_buttons.h
 *
 * DESCRIPTION:        ZLO Demo: DK button handler -Interface
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

#ifndef APP_BUTTONS_H
#define APP_BUTTONS_H

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
#if (defined APP_NTAG_ICODE) || (defined APP_NTAG_AES)
    typedef enum {
#if (defined BUTTON_MAP_OM15045) || (defined BUTTON_MAP_OM15053)
        APP_E_BUTTONS_NFC_FD = 0
#else
        APP_E_BUTTONS_BUTTON_1 = 0,
#ifndef BUTTON_MAP_DR1175
        APP_E_BUTTONS_BUTTON_2,
#endif
        APP_E_BUTTONS_NFC_FD /* Used for NTAG_FD */
#endif

} APP_teButtons;

#if (defined BUTTON_MAP_DR1175)
    #define APP_BUTTONS_NUM                     (2UL)
    #if (JENNIC_CHIP_FAMILY == JN516x)
        #define APP_BUTTONS_BUTTON_1            (8)
        #define APP_BUTTONS_NFC_FD              (0)  /* Used for NTAG_FD */
    #elif (JENNIC_CHIP_FAMILY == JN517x)
        #define APP_BUTTONS_BUTTON_1            (4)
        #define APP_BUTTONS_NFC_FD              (17) /* Used for NTAG_FD */
    #endif
    #define APP_BUTTONS_DIO_MASK                ((1 << APP_BUTTONS_BUTTON_1) | (1 << APP_BUTTONS_NFC_FD))
#elif (defined BUTTON_MAP_OM15045)
    #define APP_BUTTONS_NUM                     (1UL)
    #define APP_BUTTONS_NFC_FD                  (3) /* Used for NTAG_FD */
    #define APP_BUTTONS_DIO_MASK                (1 << APP_BUTTONS_NFC_FD)
#elif (defined BUTTON_MAP_OM15053)
    #define APP_BUTTONS_NUM                     (1UL)
    #define APP_BUTTONS_NFC_FD                  (6) /* Used for NTAG_FD */
    #define APP_BUTTONS_DIO_MASK                (1 << APP_BUTTONS_NFC_FD)
#else
    #define APP_BUTTONS_NUM                     (3UL)
    #if (JENNIC_CHIP_FAMILY == JN516x)
        #define APP_BUTTONS_BUTTON_1            (9)
        #define APP_BUTTONS_BUTTON_2            (10)
        #define APP_BUTTONS_NFC_FD              (0) /* Used for NTAG_FD */
    #elif (JENNIC_CHIP_FAMILY == JN517x)
        #define APP_BUTTONS_BUTTON_1            (4)
        #define APP_BUTTONS_BUTTON_2            (10)
        #define APP_BUTTONS_NFC_FD              (17) /* Used for NTAG_FD */
    #endif
    #define APP_BUTTONS_DIO_MASK                ((1 << APP_BUTTONS_BUTTON_1) | (1 << APP_BUTTONS_BUTTON_2) | (1 << APP_BUTTONS_NFC_FD) )
#endif

#else
    typedef enum {
        APP_E_BUTTONS_BUTTON_1 = 0,
        APP_E_BUTTONS_BUTTON_2
    } APP_teButtons;


    #if (defined BUTTON_MAP_DR1175)
        #define APP_BUTTONS_NUM             (1UL)
    #if (JENNIC_CHIP_FAMILY == JN516x)
        #define APP_BUTTONS_BUTTON_1        (8)
    #elif (JENNIC_CHIP_FAMILY == JN517x)
        #define APP_BUTTONS_BUTTON_1        (4)
    #endif
    #define APP_BUTTONS_DIO_MASK        (1 << APP_BUTTONS_BUTTON_1)
#else
    #define APP_BUTTONS_NUM             (2UL)
    #if (JENNIC_CHIP_FAMILY == JN516x)
        #define APP_BUTTONS_BUTTON_1        (9)
        #define APP_BUTTONS_BUTTON_2        (10)
    #elif (JENNIC_CHIP_FAMILY == JN517x)
        #define APP_BUTTONS_BUTTON_1        ()
        #define APP_BUTTONS_BUTTON_2        (10)
    #endif
    #define APP_BUTTONS_DIO_MASK        ( (1 << APP_BUTTONS_BUTTON_1) | (1 << APP_BUTTONS_BUTTON_2) )
#endif
#endif




/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC bool_t APP_bButtonInitialise(void);
#if (JENNIC_CHIP_FAMILY == JN516x)
PUBLIC void vISR_SystemController(void);
#endif
#if (JENNIC_CHIP_FAMILY == JN517x)
PUBLIC void vISR_SystemController(uint32 u32DeviceId, uint32 u32BitMap);
#endif
PUBLIC void APP_cbTimerButtonScan(void *pvParam);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

#endif /*APP_BUTTONS_H*/
