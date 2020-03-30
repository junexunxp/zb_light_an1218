/*****************************************************************************
 *
 * MODULE:             JN-AN-1218
 *
 * COMPONENT:          App_DimmableLight.h
 *
 * DESCRIPTION:        ZLO Demo: Dimmable Light Interface
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

#ifndef APP_DIMMABLE_LIGHT_H
#define APP_DIMMABLE_LIGHT_H

#include "dimmable_light.h"
#include "commission_endpoint.h"

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

extern tsZLO_DimmableLightDevice sLight;
extern tsCLD_ZllDeviceTable sDeviceTable;
extern tsReports asDefaultReports[];

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eApp_ZLO_RegisterEndpoint(tfpZCL_ZCLCallBackFunction fptr);
PUBLIC void vAPP_ZCL_DeviceSpecific_Init(void);
PUBLIC void vSetBulbState(bool bOn, uint8 u8Level);
PUBLIC void vStartEffect(uint8 u8Effect);
PUBLIC void vIdEffectTick( uint8 u8Endpoint);
PUBLIC void APP_vHandleIdentify(uint16 u16Time);
PUBLIC void vCreateInterpolationPoints(void);
PUBLIC uint8 app_u8GetDeviceEndpoint( void);
PUBLIC void vApp_ZCL_ResetDeviceStructure(void);
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* APP_DIMMABLE_LIGHT_H */
