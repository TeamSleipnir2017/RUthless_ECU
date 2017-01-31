/*
 * math.h
 *
 * Created: 26.1.2017 21:10:34
 *  Author: jbbja
 */ 


#ifndef MATH_H_
#define MATH_H_

#include "global.h"
#include <asf.h>		// This is included in global.h but there is a compile error when this is not included here, 29.1.17 JBB

// Map value to a different range
uint32_t math_map_adc(uint16_t min, uint16_t max, uint16_t value);
uint32_t math_map(uint16_t min, uint16_t max, int16_t value, uint16_t div);
// Find median of vector
uint16_t math_find_median(uint16_t * Vector, uint16_t VectorLength);



#endif /* MATH_H_ */