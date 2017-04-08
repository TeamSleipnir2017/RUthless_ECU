/*
 * math.h
 *
 * Created: 26.1.2017 21:10:34
 *  Author: jbbja
 */ 


#ifndef MATH_H_
#define MATH_H_

#define UINT32_T_MAX 4294967295

#include "global.h"

// Map value to a different range
uint32_t math_map_adc(uint16_t min, uint16_t max, uint16_t value);
uint32_t math_map(uint16_t min, uint16_t max, int16_t value, uint16_t div);
// Find median of vector
uint16_t math_find_median(uint16_t * Vector, uint16_t VectorLength);
// Calculate interpolated value for 3D tables
uint16_t math_interpolation_array(uint16_t Rpm, uint16_t Map, struct Table3D *Current, uint16_t Scaler);
// Calculate interpolated value for any vector
uint16_t math_interpolation_vector(uint8_t *LookUp, uint8_t *Calculate, uint16_t Value, uint16_t Scaler, uint8_t Len);
// Helper functions
void math_find_interpolation_index(uint8_t * Vector, uint16_t Value, uint8_t * Low, uint8_t * High, uint16_t Scaler, uint8_t Len);
uint8_t math_interpolation(uint16_t value, uint16_t x1, uint16_t x2);

// Function to add two variables together 32 bit, when there is a overflow it should return 2^32 - a
uint32_t math_sum_with_overflow_protection(uint32_t a, uint32_t b);
// Calculate teeth counts from degrees
uint8_t math_convert_degree_to_teeth_count(uint16_t degree);
// Calculate number of teeths according to RPM, PW is in hundreds of nanoseconds (1 = 0.1 µs)
uint8_t math_convert_pulsewidth_to_teeth_count(uint32_t PW);
// Convert pulsewidth (time) to timer counts, PW is in hundreds of nanoseconds (1 = 0.1 µs)
uint32_t math_convert_pulsewidth_to_timer_counts(uint32_t PW);
// Find the event tooth(initiate timer tooth) from current tooth 
uint32_t math_calculate_event_tooth_from_number_of_teeths(uint8_t CurrentCrankTooth, uint8_t NumberOfTeeths);

#endif /* MATH_H_ */