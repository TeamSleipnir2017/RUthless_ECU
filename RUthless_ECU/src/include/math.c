/*
 * math.c
 *
 * Created: 26.1.2017 21:10:18
 *  Author: jbbja
 */ 

#include "math.h"

uint32_t math_map_adc(uint16_t min, uint16_t max, uint16_t value)
{
	return ((value * max) >> ADC_RESOLUTION) + min;
}
uint32_t math_map(uint16_t min, uint16_t max, int16_t value, uint16_t div)
{
	if (value < 0)
		return 0;
	return ((value * max) / div) + min;
}

// ATTENTION this will not work if the vector is not 16 bit, not sure how to make it global size since it is a pointer
uint16_t math_find_median(uint16_t * Vector, uint16_t VectorLength)
{
	uint16_t TempVector[VectorLength];
	// Copy vector so we won't change him (copy constructor)
	for (uint16_t i = 0; i < VectorLength; i++)
	{
		TempVector[i] = Vector[i];
		//uart_print_string("her "); uart_print_int(Vector[i]); uart_new_line();
	}
	// Insertion sort
	for (uint16_t i = 1; i < VectorLength; i++)
	{
		uint16_t j = i;
		while (j > 0 && TempVector[j] < TempVector[j-1])
		{
			uint16_t temp = TempVector[j];
			TempVector[j] = TempVector[j-1];
			TempVector[j-1] = temp;
			j--;
		}
	}
	// Return median
	return TempVector[(uint16_t)((VectorLength * 1.0 + 0.5) / 2.0)];
}

uint16_t math_interpolation_array(uint16_t Rpm, uint16_t Map, struct Table3D *Current, uint16_t Scaler)
{
	uint8_t RpmIndexLow = 0, MapIndexLow = 0;
	uint8_t RpmIndexHigh = THREE_D_TABLE_SIZE - 1, MapIndexHigh = THREE_D_TABLE_SIZE - 1;
	// Find where the engine is positioned in the 3D tables
	
	math_find_interpolation_index(Current->Xbin, Rpm, &RpmIndexLow, &RpmIndexHigh, RPM_SCALER, THREE_D_TABLE_SIZE);
	math_find_interpolation_index(Current->Ybin, Map, &MapIndexLow, &MapIndexHigh, 1, THREE_D_TABLE_SIZE);

	// Use the indexes to calculate a interpolated value
	// Weighted average is first used
	uint8_t RpmWeight = math_interpolation(Rpm, Current->Xbin[RpmIndexLow] * RPM_SCALER, Current->Xbin[RpmIndexHigh] * RPM_SCALER);
// 	if (DebugCounter>= 100)
// 	{
// 		uart_print_string("Vec 0 "); uart_print_int(Current->Xbin[0]); uart_new_line();
// 		uart_print_string("Rpm "); uart_print_int(Rpm); uart_new_line();
// 		uart_print_string("RpmL "); uart_print_int(Current->Xbin[RpmIndexLow] * RPM_SCALER); uart_new_line();
// 		uart_print_string("RpmH "); uart_print_int(Current->Xbin[RpmIndexHigh] * RPM_SCALER); uart_new_line();
// 		uart_print_string("RpmIndexLow "); uart_print_int(RpmIndexLow); uart_new_line();
// 		uart_print_string("RpmIndexHigh "); uart_print_int(RpmIndexHigh); uart_new_line();
// 		uart_print_string("Weight "); uart_print_int(RpmWeight); uart_new_line();
// 		DebugCounter = 0;
// 	}
// 	DebugCounter = DebugCounter + 1;
	
	uint8_t MapWeight = math_interpolation(Map, Current->Ybin[MapIndexLow], Current->Ybin[MapIndexHigh]);
	// Calculate interpolated value of the four values
	/************************************************************************/
	/* 
	MAP	
	x	calc3	calc4
	x	calc1	calc2
	RPM	x		x                                                           */
	/************************************************************************/
	uint32_t calc1 = (uint32_t)Current->Table[MapIndexLow][RpmIndexLow] * (100 - MapWeight) * (100 - RpmWeight);
	uint32_t calc2 = (uint32_t)Current->Table[MapIndexLow][RpmIndexHigh] * (100 - MapWeight) * (RpmWeight);
	uint32_t calc3 = (uint32_t)Current->Table[MapIndexHigh][RpmIndexLow] * (MapWeight) * (100 - RpmWeight);
	uint32_t calc4 = (uint32_t)Current->Table[MapIndexHigh][RpmIndexHigh] * (MapWeight) * (RpmWeight);
	uint16_t tempCalc = 1000 / Scaler;
	return (calc1 + calc2 + calc3 + calc4) / tempCalc;
}

uint16_t math_interpolation_vector(uint8_t *LookUp, uint8_t *Calculate, uint16_t Value, uint16_t Scaler, uint8_t Len)
{
	uint8_t IndexLow = 0;
	uint8_t IndexHigh = Len - 1;
	math_find_interpolation_index(LookUp, Value, &IndexLow, &IndexHigh, Scaler, Len);
	uint8_t Weight = math_interpolation(Value, LookUp[IndexLow] * Scaler, LookUp[IndexHigh] * Scaler);
	uint16_t calc1 = (uint32_t)Calculate[IndexLow] * (100 - Weight);
	uint16_t calc2 = (uint32_t)Calculate[IndexHigh] * (Weight);
	return (calc1 + calc2) / 100;
}

void math_find_interpolation_index(uint8_t * Vector, uint16_t Value, uint8_t * Low, uint8_t * High, uint16_t Scaler, uint8_t Len)
{
	for (uint8_t Index = 0; Index < Len; Index++)
	{	
// 		if (DebugCounter>= 100)
// 		{
// 			uart_print_string("Vec "); uart_print_int(Vector[Index]); uart_new_line();
// 		}
		uint16_t Temp = Vector[Index] * Scaler;
		
		if (Value > Temp) {
			*Low = Index;
		} 
		else if (Value < Temp) {
			*High = Index;
			break;
		} 
		else if (Value == Temp) {
			*Low = Index;
			*High = Index;
			break;
		}
	}
}


// Calculate the weight of the value compared to indexes x1 and x2
uint8_t math_interpolation(uint16_t value, uint16_t x1, uint16_t x2)
{
	if (value <= x1) // below or equal to lower bound
		return 0;

	if (value >= x2) // above or equal to upper bounds
		return 100;

	return ((uint32_t)(value - x1) * 100)/((x2 - x1));
}

uint32_t math_sum_with_overflow_protection(uint32_t a, uint32_t b)
{
	uint32_t sum = a + b;
	if (sum < a)
		return UINT32_T_MAX - a;
	else
		return sum;
	// return ((a+b) < a) ? (UINT_MAX - a) : (a+b);
}