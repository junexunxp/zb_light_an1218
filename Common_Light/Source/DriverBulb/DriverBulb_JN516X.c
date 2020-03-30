/****************************************************************************
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
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
/* Standard includes */
#include <string.h>
/* SDK includes */
#include <jendefs.h>
/* Hardware includes */
#include <AppHardwareApi.h>
#include <PeripheralRegs.h>
/* JenOS includes */
#include <dbg.h>
/* Application includes */
#include "Config.h"
/* Device includes */
#include "DriverBulb.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/* PWM timer configuration */
#define PERIPHERAL_CLOCK_FREQUENCY_HZ	16000000UL		/* System frequency 16MHz */
#define PWM_TIMER_PRESCALE				6     			/* Prescale value to use   */
#define PWM_COUNT_MAX					255				/* Gives PWM frequency of ((16MHz / 2^6) / 255) */

/* Channel definitions */
#ifdef RGB
#define PWM_TIMER_RED					E_AHI_TIMER_1
#define PWM_TIMER_GREEN					E_AHI_TIMER_2
#define PWM_TIMER_BLUE					E_AHI_TIMER_3
#else
#define PWM_TIMER_WHITE					E_AHI_TIMER_3
#endif

#ifdef MK_PWM_TIMER_INVERT
#define PWM_INVERT						MK_PWM_TIMER_INVERT
#else
#define PWM_INVERT						FALSE
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE void DriverBulb_vOutput(void);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Global Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE bool_t  bIsOn      	 	= FALSE;
PRIVATE uint8   u8CurrLevel 	= 255;
#ifdef RGB
PRIVATE uint8   u8CurrRed       = PWM_COUNT_MAX;
PRIVATE uint8   u8CurrGreen     = PWM_COUNT_MAX;
PRIVATE uint8   u8CurrBlue		= PWM_COUNT_MAX;
#endif

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 *
 * NAME:       		DriverBulb_vInit
 *
 * DESCRIPTION:		Initializes the lamp drive system
 *
 * PARAMETERS:      Name     RW  Usage
 *
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void DriverBulb_vInit(void)
{
	static bool_t bInit = FALSE;

	/* Not already initialized ? */
	if (bInit == FALSE)
	{
		#if PWM_INVERT
			bool_t bInvertPWM = TRUE;
		#else
			bool_t bInvertPWM = FALSE;
		#endif

		#ifdef RGB
			/* Configure red channel PWM timer */
			vAHI_TimerEnable(PWM_TIMER_RED, PWM_TIMER_PRESCALE, FALSE, FALSE, TRUE);
			vAHI_TimerConfigureOutputs(PWM_TIMER_RED, bInvertPWM, TRUE);

			/* Configure green channel PWM timer */
			vAHI_TimerEnable(PWM_TIMER_GREEN, PWM_TIMER_PRESCALE, FALSE, FALSE, TRUE);
			vAHI_TimerConfigureOutputs(PWM_TIMER_GREEN, bInvertPWM, TRUE);

			/* Configure blue channel PWM timer */
			vAHI_TimerEnable(PWM_TIMER_BLUE, PWM_TIMER_PRESCALE, FALSE, FALSE, TRUE);
			vAHI_TimerConfigureOutputs(PWM_TIMER_BLUE, bInvertPWM, TRUE);
		#else
			/* Configure white channel PWM timer */
			vAHI_TimerEnable(PWM_TIMER_WHITE, PWM_TIMER_PRESCALE, FALSE, FALSE, TRUE);
			vAHI_TimerConfigureOutputs(PWM_TIMER_WHITE, bInvertPWM, TRUE);
		#endif

		/* Note light is on */
		bIsOn = TRUE;

		/* Set outputs */
		DriverBulb_vOutput();

		/* Now initialized */
        bInit = TRUE;
	}
}

/****************************************************************************
 *
 * NAME:       		DriverBulb_bReady
 *
 * DESCRIPTION:		Returns if lamp is ready to be operated
 *
 * PARAMETERS:      Name     RW  Usage
 *
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC bool_t DriverBulb_bReady(void)
{
	return (TRUE);
}

/****************************************************************************
 *
 * NAME:       		DriverBulb_vSetLevel
 *
 * DESCRIPTION:		Updates the PWM via the thermal control loop
 *
 *
 * PARAMETERS:      Name     RW  Usage
 *         	        u8Level  R   Light level 0-LAMP_LEVEL_MAX
 *
 ****************************************************************************/
PUBLIC void DriverBulb_vSetLevel(uint32 u32Level)
{
	/* Different value ? */
	if (u8CurrLevel != (uint8) u32Level)
	{
		/* Note the new level */
		u8CurrLevel = (uint8) MAX(1, u32Level);
		/* Is the lamp on ? */
		if (bIsOn)
		{
			/* Set outputs */
			DriverBulb_vOutput();
		}
	}
}

/****************************************************************************
 *
 * NAME:       		DriverBulb_vSetColour
 *
 * DESCRIPTION:		Updates the PWM via the thermal control loop
 *
 *
 * PARAMETERS:      Name     RW  Usage
 *         	        u8Level  R   Light level 0-LAMP_LEVEL_MAX
 *
 ****************************************************************************/
#ifdef RGB
PUBLIC void DriverBulb_vSetColour(uint32 u32Red, uint32 u32Green, uint32 u32Blue)
{
	/* Different value ? */
	if (u8CurrRed != (uint8) u32Red || u8CurrGreen != (uint8) u32Green || u8CurrBlue != (uint8) u32Blue)
	{
		/* Note the new values */
		u8CurrRed   = (uint8) u32Red;
		u8CurrGreen = (uint8) u32Green;
		u8CurrBlue  = (uint8) u32Blue;
		/* Is the lamp on ? */
		if (bIsOn)
		{
			/* Set outputs */
			DriverBulb_vOutput();
		}
	}
}
#endif
/****************************************************************************
 *
 * NAME:            DriverBulb_vOn
 *
 * DESCRIPTION:     Turns the lamp on, over-driving if user deep-dimmed
 *                  before turning off otherwise ignition failures occur
 *
 ****************************************************************************/
PUBLIC void DriverBulb_vOn(void)
{
	/* Lamp is not on ? */
	if (bIsOn != TRUE)
	{
		/* Note light is on */
		bIsOn = TRUE;
		/* Set outputs */
		DriverBulb_vOutput();
	}
}

/****************************************************************************
 *
 * NAME:            DriverBulb_vOff
 *
 * DESCRIPTION:     Turns the lamp off
 *
 *
 * PARAMETERS:      Name     RW  Usage
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void DriverBulb_vOff(void)
{
	/* Lamp is on ? */
	if (bIsOn == TRUE)
	{
		/* Note light is off */
		bIsOn = FALSE;
		/* Set outputs */
		DriverBulb_vOutput();
	}
}

/****************************************************************************
 *
 * NAMES:           DriverBulb_bOn, u16ReadBusVoltage, u16ReadChipTemperature
 *
 * DESCRIPTION:		Access functions for Monitored Lamp Parameters
 *
 *
 * PARAMETERS:      Name     RW  Usage
 *
 * RETURNS:
 * Lamp state, Bus Voltage (Volts), Chip Temperature (Degrees C)
 *
 ****************************************************************************/
PUBLIC bool_t DriverBulb_bOn(void)
{
	return (bIsOn);
}

/****************************************************************************
 *
 * NAMES:           DriverBulb_vTick
 *
 * DESCRIPTION:		Hook for 10 ms Ticks from higher layer for timing
 *                  This currently restores very low dim levels after
 *                  we've started up with sufficient DCI level to ensure
 *                  ignition
 *
 ****************************************************************************/
PUBLIC void DriverBulb_vTick(void)
{
}

/****************************************************************************
 *
 * NAME:			DriverBulb_i16Analogue
 *
 * DESCRIPTION:     ADC based measurement of bus voltage is not support in
 * 					this driver.
 *
 * PARAMETERS:      Name	     RW      Usage
 *                  u8Adc        R       ADC Channel
 *                  u16AdcRead   R       Raw ADC Value
 *
 * RETURNS:         Bus voltage
 *
 ****************************************************************************/
PUBLIC int16 DriverBulb_i16Analogue(uint8 u8Adc, uint16 u16AdcRead)
{
    return 0;
}

/****************************************************************************
 *
 * NAME:			DriverBulb_bFailed
 *
 * DESCRIPTION:     Access function for Failed bulb state
 *
 *
 * RETURNS:         bulb state
 *
 ****************************************************************************/
PUBLIC bool_t DriverBulb_bFailed(void)
{
	return (FALSE);
}

/****************************************************************************
 *
 * NAME:			DriverBulb_vOutput
 *
 * DESCRIPTION:
 *
 * RETURNS:         void
 *
 ****************************************************************************/
PRIVATE void DriverBulb_vOutput(void)
{
	#ifdef RGB
		uint8   u8Red;
		uint8   u8Green;
		uint8   u8Blue;

		/* Is bulb on ? */
		if (bIsOn)
		{
			/* Scale colour for brightness level */
			u8Red   = (uint8)(((uint32)u8CurrRed   * (uint32)u8CurrLevel) / (uint32)255);
			u8Green = (uint8)(((uint32)u8CurrGreen * (uint32)u8CurrLevel) / (uint32)255);
			u8Blue  = (uint8)(((uint32)u8CurrBlue  * (uint32)u8CurrLevel) / (uint32)255);

			/* Don't allow fully off */
			if (u8Red   == 0) u8Red   = 1;
			if (u8Green == 0) u8Green = 1;
			if (u8Blue  == 0) u8Blue  = 1;
		}
		else /* Turn off */
		{
			u8Red   = 0;
			u8Green = 0;
			u8Blue  = 0;
		}

		/* Set RGB channel levels */
		vAHI_TimerStartRepeat(PWM_TIMER_RED,   (PWM_COUNT_MAX - u8Red  ), PWM_COUNT_MAX);
		vAHI_TimerStartRepeat(PWM_TIMER_GREEN, (PWM_COUNT_MAX - u8Green), PWM_COUNT_MAX);
		vAHI_TimerStartRepeat(PWM_TIMER_BLUE,  (PWM_COUNT_MAX - u8Blue ), PWM_COUNT_MAX);
	#else
		/* Is bulb on ? */
		if (bIsOn)
		{
			vAHI_TimerStartRepeat(PWM_TIMER_WHITE,  (PWM_COUNT_MAX - u8CurrLevel), PWM_COUNT_MAX);
		}
		else
		{
			vAHI_TimerStartRepeat(PWM_TIMER_WHITE, PWM_COUNT_MAX, PWM_COUNT_MAX);
		}
	#endif
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
