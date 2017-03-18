/*
 * decoders.c
 *
 * Created: 12.3.2017 18:24:44
 *  Author: rikardur
 */ 


#include "decoders.h"


void decoders_crank_primary(void)
{
	if (CrankSignalFlag)
	{
		if(CrankCurrCycleCounts > (3 * (CrankPrevCycleCounts >> 1)))
		{
			CrankTooth = 0;
			CrankSecondTach = 11 - math_ign_time_teeth(DEGREE_TEST);
			CrankSecondInterval = math_ign_time_interval(DEGREE_TEST, CrankPrevCycleCounts);
		}
		else if (CrankTooth == 12)
		{
			CrankFirstTach = 23 - math_ign_time_teeth(DEGREE_TEST);
			CrankFirstInterval = math_ign_time_interval(DEGREE_TEST, CrankPrevCycleCounts);
		}
		CrankSignalFlag = FALSE;
		
	}
}




// Toggle ignition pin 1
void decoders_toggle_ign1pin(void)
{
	uint32_t status =	PIOC->PIO_ODSR;			// Store the status on pins in port C
	/*	Toggle output pin PC19	*/
	if (status & PIO_ODSR_P19)					// AND the status on pins in port C and the status of pin 19
	{
		PIOC->PIO_CODR	=	IGN1_OUT;			// If PC19 is high,	Clear Output Data Register	-	Sets pin PC19 to low
	}
	else
	{
		PIOC->PIO_SODR	=	IGN1_OUT;			// If PC19 is low,	Set Output Data Register	-	Sets pin PC19 to high
	}
}

// Toggle ignition pin 2
void decoders_toggle_ign2pin(void)
{
	uint32_t status =	PIOC->PIO_ODSR;			// Store the status on pins in port C
	/*	Toggle output pin PC19	*/
	if (status & PIO_ODSR_P23)					// AND the status on pins in port C and the status of pin 19
	{
		PIOC->PIO_CODR	=	IGN2_OUT;			// If PC19 is high,	Clear Output Data Register	-	Sets pin PC19 to low
	}
	else
	{
		PIOC->PIO_SODR	=	IGN2_OUT;			// If PC19 is low,	Set Output Data Register	-	Sets pin PC19 to high
	}
}

// Toggle ignition pin 3
void decoders_toggle_ign3pin(void)
{
	uint32_t status =	PIOC->PIO_ODSR;			// Store the status on pins in port C
	/*	Toggle output pin PC19	*/
	if (status & PIO_ODSR_P17)					// AND the status on pins in port C and the status of pin 19
	{
		PIOC->PIO_CODR	=	IGN3_OUT;			// If PC19 is high,	Clear Output Data Register	-	Sets pin PC19 to low
	}
	else
	{
		PIOC->PIO_SODR	=	IGN3_OUT;			// If PC19 is low,	Set Output Data Register	-	Sets pin PC19 to high
	}
}

// Toggle ignition pin 4
void decoders_toggle_ign4pin(void)
{
	uint32_t status =	PIOC->PIO_ODSR;			// Store the status on pins in port C
	/*	Toggle output pin PC19	*/
	if (status & PIO_ODSR_P25)					// AND the status on pins in port C and the status of pin 25
	{
		PIOC->PIO_CODR	=	IGN4_OUT;			// If PC25 is high,	Clear Output Data Register	-	Sets pin PC25 to low
	}
	else
	{
		PIOC->PIO_SODR	=	IGN4_OUT;			// If PC25 is low,	Set Output Data Register	-	Sets pin PC25 to high
	}
}