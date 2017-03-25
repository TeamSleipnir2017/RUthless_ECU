/*
 * igncalc.h
 *
 * Created: 22.3.2017 15:24:20
 *  Author: rikardur
 */ 

#include "global.h"


#ifndef IGNCALC_H_
#define IGNCALC_H_
// Calculate ignition timing
uint16_t igncalc_ign_time_teeth(uint8_t ign_degree);
uint32_t igncalc_ign_time_interval(uint8_t ign_degree);

uint32_t igncalc_dwell_time(void);
uint8_t igncalc_dwell_correction(void);



#endif /* IGNCALC_H_ */