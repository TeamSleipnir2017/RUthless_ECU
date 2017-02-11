/*
 * math.h
 *
 * Created: 26.1.2017 21:10:34
 *  Author: jbbja
 */ 


#ifndef MATH_H_
#define MATH_H_

// MOVE WHEN ATMEL STUDIO IS NOT STUPID 5.2.17
#define THREE_D_TABLE_SIZE 16	// 3D table size definitions

#include <asf.h>		// This is included in global.h but there is a compile error when this is not included here, 29.1.17 JBB
#include "global.h"

// Map value to a different range
uint32_t math_map_adc(uint16_t min, uint16_t max, uint16_t value);
uint32_t math_map(uint16_t min, uint16_t max, int16_t value, uint16_t div);
// Find median of vector
uint16_t math_find_median(uint16_t * Vector, uint16_t VectorLength);
// Calculate interpolated value for 3D tables
uint16_t math_interpolation_array(uint16_t Rpm, uint16_t Map, uint8_t Table[THREE_D_TABLE_SIZE][THREE_D_TABLE_SIZE], uint8_t RpmBins[THREE_D_TABLE_SIZE], uint8_t MapBins[THREE_D_TABLE_SIZE]);
// Helper functions
void math_find_interpolation_index(uint8_t * Vector, uint16_t Value, uint8_t * Low, uint8_t * High, uint16_t Scaler, uint8_t Len);
uint8_t math_interpolation(uint16_t value, uint16_t x1, uint16_t x2);


#endif /* MATH_H_ */