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

/* SDK includes */
#include <jendefs.h>
#include <AppHardwareApi.h>
#include <PeripheralRegs.h>
#include <MicroSpecific.h>
#include "Printf.h"

#ifdef RTOS
#include "os.h"
#endif

/* Device includes */
#include "DriverBulb.h"


/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#define ON_MASK   0xFFFFFFFFUL
#define MAX_VALUE 255
#define TIMER_START_SINGLE     (1 + 8 + 16 + 32 + 512 + 0)  /* EN + OE + SINGLE + RST + GDIS + INVOUT*/
#define SETTLE_TIME   45U
#define PRESCALE  0
#define ADC_CHANNELS 6
#define MEAS_DIO_MASK (1 << 8)
#define EMITTERS 3
#define PHASE_ADV120 21845
#define PHASE_ADV240 43690
#define OPTIMAL_MODULATION_INDEX_MASK 0xe000
#define GPIO_DIN_REG 0x02002008
#define T_CONV 2000   /* 125us/62.5ns */

/*Timer PWM waveforms on DIO10,11,12 */
#define TIMER_DIO_MASK (E_AHI_DIO10_INT | E_AHI_DIO11_INT | E_AHI_DIO12_INT)

#define TIMER_EN  1UL
#define TRACE_DIO_MASK (1 << 5)

#if (TRACE_DIO_MASK)
#define TRACE_ADC_ON	   vREG_GpioWrite(REG_GPIO_DOUT,(u32REG_GpioRead(REG_GPIO_DIN) |  TRACE_DIO_MASK))
#define TRACE_ADC_OFF      vREG_GpioWrite(REG_GPIO_DOUT,(u32REG_GpioRead(REG_GPIO_DIN) & ~TRACE_DIO_MASK))
#else
#define TRACE_ADC_ON
#define TRACE_COMP_OFF
#endif

#ifdef RTOS
#define DRIVER_ISR(x) OS_ISR(x)
#else
#define DRIVER_ISR(x) PRIVATE void x(void)
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef struct
{
	u32Register u32Count;
	uint32      u32Rise;
	uint32      u32Fall;
	uint32      u32Control;
	uint32      u32Prescale;
	uint32      u32IntStatus;
	uint32      u32IntEnable;
	uint32      u32MaskedIntStatus;
}tsTimer; /* Timer HAL Interface */

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/* Interrupt service routines */
#ifndef RTOS
PRIVATE void vISR_PwmTimer(void);
PRIVATE void vISR_MeasureLedStringVoltage(void);
#endif

/* Worker Functions */
PRIVATE void vJitterPwmWaveform(tsTimer * psAnyTimer, uint32 u32LedStringId);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Global Variables                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/


PRIVATE  struct
{
	uint32 u32OnOffMask;
	uint32 u32Level;
	uint32 au32Colour[EMITTERS];
	uint32 u32AdcChannel;
	uint16 au16AdcVal[ADC_CHANNELS];
	volatile bool_t bSafeToSample;
	volatile bool_t bOverlapSufficient;
	volatile bool_t bConversionPending;
} sDriver = {ON_MASK,MAX_VALUE,{MAX_VALUE,MAX_VALUE,MAX_VALUE},0,{0,0,0,0,0,0},FALSE,FALSE,FALSE};

//
tsTimer * const psTimerS = (tsTimer *)REG_TMR4_BASE;

//tsTimer * const psTimerW = (tsTimer *)REG_TMR3_BASE;


tsTimer * const apsRGBTimers[3] = { (tsTimer *)REG_TMR0_BASE,
		                            (tsTimer *)REG_TMR2_BASE,
		                            (tsTimer *)REG_TMR1_BASE};

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

extern void vAHI_InterruptSetPriority(uint16 u16Mask, uint8 u8Level);

/****************************************************************************
 *
 * NAME:       		vRGB_Init
 *
 * DESCRIPTION:		Initialises RGB PWM Timers
 *
 ****************************************************************************/
PUBLIC void DriverBulb_vInit(void)
{
	static bool_t bFirstCalled = TRUE;

	if (bFirstCalled)
	{
		bFirstCalled = FALSE;
#ifndef RTOS

		/* Ensure PWM and ADC settle timers have highest priority */
		vAHI_InterruptSetPriority( MICRO_ISR_MASK_ANPER , 15);
		vAHI_InterruptSetPriority( MICRO_ISR_MASK_TMR0,   14);
		vAHI_InterruptSetPriority( MICRO_ISR_MASK_TMR1,   14);
		vAHI_InterruptSetPriority( MICRO_ISR_MASK_TMR2,   14);
		vAHI_InterruptSetPriority( MICRO_ISR_MASK_TMR4,   15);

		/* Patch in the ISR before default handlers */
		MICRO_SET_INT_HANDLER(MICRO_ISR_NUM_TMR0 , vISR_PwmTimer);
		MICRO_SET_INT_HANDLER(MICRO_ISR_NUM_TMR2 , vISR_PwmTimer);
		MICRO_SET_INT_HANDLER(MICRO_ISR_NUM_TMR1 , vISR_PwmTimer);

		MICRO_SET_INT_HANDLER(MICRO_ISR_NUM_TMR4 , vISR_MeasureLedStringVoltage);

		MICRO_SET_INT_HANDLER(MICRO_ISR_NUM_ANPER, vISR_MeasureLedStringVoltage);

#endif

		/* Set up the ADC */
		vAHI_ApConfigure(E_AHI_AP_REGULATOR_ENABLE, E_AHI_AP_INT_ENABLE,E_AHI_AP_SAMPLE_2,E_AHI_AP_CLOCKDIV_500KHZ, E_AHI_AP_INTREF);
		while (!bAHI_APRegulatorEnabled());

		/* Ensure DIO8/9 freed up when timer 0 used */
	    vAHI_TimerFineGrainDIOControl(REG_SYS_TIM_IO_T0GDIS_MASK | REG_SYS_TIM_IO_T0CDIS_MASK );

		/* Set up the 300us Settle timer */
		vAHI_TimerEnable(E_AHI_TIMER_4, 4 , FALSE, TRUE, TRUE);
		psTimerS->u32Fall = SETTLE_TIME;
		psTimerS->u32Rise = SETTLE_TIME;

		vAHI_TimerEnable(E_AHI_TIMER_0, PRESCALE , TRUE, FALSE, TRUE);
		vAHI_TimerEnable(E_AHI_TIMER_1, PRESCALE , TRUE, FALSE, TRUE);
		vAHI_TimerEnable(E_AHI_TIMER_2, PRESCALE , TRUE, FALSE, TRUE);

		vAHI_TimerConfigure(E_AHI_TIMER_0,MK_USE_POSITIVE_PWM, TRUE);
		vAHI_TimerConfigure(E_AHI_TIMER_1,MK_USE_POSITIVE_PWM, TRUE);
		vAHI_TimerConfigure(E_AHI_TIMER_2,MK_USE_POSITIVE_PWM, TRUE);

		vAHI_TimerStartRepeat(E_AHI_TIMER_0,(MAX_VALUE * MAX_VALUE),(MAX_VALUE * MAX_VALUE)+500);
		vAHI_TimerStartRepeat(E_AHI_TIMER_1,(MAX_VALUE * MAX_VALUE),(MAX_VALUE * MAX_VALUE)+500);
		vAHI_TimerStartRepeat(E_AHI_TIMER_2,(MAX_VALUE * MAX_VALUE),(MAX_VALUE * MAX_VALUE)+500);

		vAHI_DioSetDirection(0,(TRACE_DIO_MASK | MEAS_DIO_MASK));
	}
}

PUBLIC void DriverBulb_vOn(void)
{
	DriverBulb_vSetOnOff(TRUE);
}

PUBLIC void DriverBulb_vOff(void)
{
	DriverBulb_vSetOnOff(FALSE);
}

PUBLIC bool_t DriverBulb_bOn(void)
{
	if(sDriver.u32OnOffMask==0){
		return FALSE;
	}
	else{
		return TRUE;
	}

}

PUBLIC void DriverBulb_vSetOnOff(bool_t bOn)
{
	if (bOn)
	{
		sDriver.u32OnOffMask = ON_MASK;
		vAHI_DioSetOutput(MEAS_DIO_MASK,0);
	}
	else
	{
		sDriver.u32OnOffMask = 0;
		vAHI_DioSetOutput(0,MEAS_DIO_MASK);
	}
}

PUBLIC void DriverBulb_vSetLevel(uint32 u32Level)
{
	sDriver.u32Level = u32Level;
}

PUBLIC void DriverBulb_vSetColour(uint32 u32Red, uint32 u32Green, uint32 u32Blue)
{
	sDriver.au32Colour[E_RED_PWM]   = u32Red;
	sDriver.au32Colour[E_GREEN_PWM] = u32Green;
	sDriver.au32Colour[E_BLUE_PWM]  = u32Blue;
}

/* Auxiliary interface for AAL to hook into. Up-scales to 16bits */
/* R G B already have level mixed in in 12bit integer math       */

PUBLIC void DriverBulb_vSet12BitColour(uint32 u32Red, uint32 u32Green, uint32 u32Blue)
{
	sDriver.u32Level = 16;
	DriverBulb_vSetColour(u32Red,u32Green,u32Blue);
}

PUBLIC uint16 DriverBulb_u16GetAdcValue(uint32 u32ChannelId)
{
	uint16 u16AdcValue = 0;

	if (u32ChannelId < ADC_CHANNELS)
	{
		u16AdcValue=sDriver.au16AdcVal[u32ChannelId];
	}
	return (u16AdcValue);
}

/* JenNet-IP MIB compatiblity functions */
PUBLIC void DriverBulb_vTick(void)
{
}

PUBLIC bool_t DriverBulb_bReady(void)
{
	return TRUE;
}

PUBLIC bool_t DriverBulb_bFailed(void)
{
	return FALSE;
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/* Interrupt funnel: Handles all PWM timer interrupts */

DRIVER_ISR(vISR_PwmTimer)
{
	/* are any PWM interrupt  at the the slowest frequency */
	if (sDriver.bSafeToSample == TRUE)
	{
#if (MK_USE_POSITIVE_PWM == TRUE)
		/* ..and are all the outputs low  */
		if ((u32REG_GpioRead(REG_GPIO_DIN) & TIMER_DIO_MASK) == 0)
#else
		/* or high if we're using neagtive sense PWM */
		if ((u32REG_GpioRead(REG_GPIO_DIN) & TIMER_DIO_MASK) == TIMER_DIO_MASK)
#endif
		{
			bool_t bInputRange = E_AHI_AP_INPUT_RANGE_2;
			psTimerS->u32Control = TIMER_START_SINGLE;
			/* Pipeline the overlap calculation with the settle time */
			TRACE_ADC_ON;
			if (sDriver.u32AdcChannel == E_AHI_ADC_SRC_TEMP)
			{
				bInputRange = E_AHI_AP_INPUT_RANGE_1;
			}
			vAHI_AdcEnable(E_AHI_ADC_SINGLE_SHOT,bInputRange,sDriver.u32AdcChannel);

			sDriver.bOverlapSufficient = TRUE;
			sDriver.bOverlapSufficient &= ((u32REG_Timer0Read(REG_TMR_LO)-u32REG_Timer0Read(REG_TMR_CTR)) > T_CONV);
			sDriver.bOverlapSufficient &= ((u32REG_Timer1Read(REG_TMR_LO)-u32REG_Timer1Read(REG_TMR_CTR)) > T_CONV);
			sDriver.bOverlapSufficient &= ((u32REG_Timer2Read(REG_TMR_LO)-u32REG_Timer2Read(REG_TMR_CTR)) > T_CONV);
		}
	}

	/* Red PWM channel Interrupt */
	if (u32REG_Timer0Read(REG_TMR_MINT) & REG_TMR_INT_L_EN_MASK)
	{
		vJitterPwmWaveform((tsTimer *)REG_TMR0_BASE,E_RED_PWM);
	}

	/* Green PWM channel Interrupt*/
	if (u32REG_Timer2Read(REG_TMR_MINT) & REG_TMR_INT_L_EN_MASK)
	{
		vJitterPwmWaveform((tsTimer *)REG_TMR2_BASE,E_GREEN_PWM);
	}

	/* Blue PWM channel Interrupt*/
	if (u32REG_Timer1Read(REG_TMR_MINT) & REG_TMR_INT_L_EN_MASK)
	{
		vJitterPwmWaveform((tsTimer *)REG_TMR1_BASE,E_BLUE_PWM);
	}
}

DRIVER_ISR (vISR_MeasureLedStringVoltage)
{
	/* Check if settle time has expired and initiate ADC conversion if all dio low */
	/* and when a falling edge PWM line was low we had sufficient porch overlap    */
	if (psTimerS->u32MaskedIntStatus & REG_TMR_INT_P_EN_MASK)
	{
		psTimerS->u32MaskedIntStatus = REG_TMR_INT_P_EN_MASK;

#if (MK_USE_POSITIVE_PWM == TRUE)
		/* ..and are all the outputs low  */
		if (((u32REG_GpioRead(REG_GPIO_DIN) & TIMER_DIO_MASK) == 0) && sDriver.bOverlapSufficient)
#else
		/* or high if we're using negative sense PWM */
		if (((u32REG_GpioRead(REG_GPIO_DIN) & TIMER_DIO_MASK) == TIMER_DIO_MASK) && sDriver.bOverlapSufficient)
#endif
		{
			vAHI_AdcStartSample();
			sDriver.bOverlapSufficient = FALSE;
		}
		else
		{
			TRACE_ADC_OFF;  /* Early abort if a race condition prevailed */
		}
	}

	/* Handle the conversion complete interrupt */
	if (u32REG_AnaRead(REG_ANPER_MIS) & REG_ANPER_INT_CAPT_MASK)
	{
		uint32 u32AdcReading;
        /* clear the ADC conversion complete interrupt */
		vREG_AnaWrite((REG_ANPER_MIS) ,REG_ANPER_INT_CAPT_MASK);

		/* Simple first order exponential filter on ADC readings */
		u32AdcReading = sDriver.au16AdcVal[sDriver.u32AdcChannel];
		u32AdcReading *=7;
		u32AdcReading += (u16AHI_AdcRead()+4); /* add 1/2 divisor to dividend to improve accuracy */
		sDriver.au16AdcVal[sDriver.u32AdcChannel] = u32AdcReading >> 3;

		TRACE_ADC_OFF;  /* Genuine successful termination of cycle */
		/* Next channel on round-robin basis */
		sDriver.u32AdcChannel = (sDriver.u32AdcChannel < E_AHI_ADC_SRC_VOLT) ? (sDriver.u32AdcChannel+1) : E_AHI_ADC_SRC_ADC_1;
	}
}

PRIVATE void vJitterPwmWaveform(tsTimer * psAnyTimer, uint32 u32LedStringId)
{
	static uint32 au32SawtoothWave[EMITTERS] = {0,PHASE_ADV120,PHASE_ADV240};
	static uint32 u32lfsr = 1UL;

	uint32 u32TriangleWave = 0;

    psAnyTimer->u32MaskedIntStatus = REG_TMR_INT_L_EN_MASK;

	uint32 u32LocalRise = sDriver.au32Colour[u32LedStringId] * sDriver.u32Level & sDriver.u32OnOffMask;
	uint32 u32LocalFall = (MAX_VALUE * MAX_VALUE)+500;

	/* Limit the maximum duty cycle to 90%: This allows a 'rear-porch'  */
	/* on PWM waveform of sufficient width to allow settling + adc reading */

	u32LocalRise = (58980 * u32LocalRise) >> 16;

    /* flip top half of sawtooth to give triangle waveform modulation index */
    if (au32SawtoothWave[u32LedStringId] & 0x8000)
    {
    	u32TriangleWave =  au32SawtoothWave[u32LedStringId] ^ 0x7fff;
    }
    else
    {
    	u32TriangleWave = au32SawtoothWave[u32LedStringId] | 0x8000;
    }

    /* Raise or lower the safe to sample flag */
    sDriver.bSafeToSample = (u32TriangleWave & OPTIMAL_MODULATION_INDEX_MASK) ? TRUE : FALSE;
    sDriver.bConversionPending = TRUE;

    /* 32-bit maximal period Galois LFSR polynomial: x^32 + x^31 + x^29 + x + 1 */
    u32lfsr = (u32lfsr >> 1) ^ (-(u32lfsr & 1UL) & 0xD0000001UL);

    /* add 32us of jitter */
    u32TriangleWave = u32TriangleWave ^ (u32lfsr & 0x1FF);

	/* Jitter (scale duty and period ) each cycle */
	psAnyTimer->u32Rise = (u32LocalRise * u32TriangleWave) >> 16;
	psAnyTimer->u32Fall = (u32LocalFall * u32TriangleWave) >> 16;

	/* phase-lock the 120Deg advance (green) and 240 degrees(Blue) offsets */
	if (au32SawtoothWave[E_RED_PWM] == 0)
	{
		au32SawtoothWave[E_GREEN_PWM] = PHASE_ADV120;
		au32SawtoothWave[E_BLUE_PWM]  = PHASE_ADV240;
	}

	/* Advance the PWM modulation index */
	au32SawtoothWave[u32LedStringId] += 0x200UL;
	au32SawtoothWave[u32LedStringId] &= 0xffffUL;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
