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