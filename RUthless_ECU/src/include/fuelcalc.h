/*
 * fuelcalc.h
 *
 * Created: 1.2.2017 17:51:09
 *  Author: jbbja
 */ 



#ifndef FUELCALC_H_
#define FUELCALC_H_

#include "global.h"

uint32_t fuelcalc_pulsewidth(void);
uint16_t fuelcalc_GammaEnrich(void);

uint32_t LastCrankingCycle;


#endif /* FUELCALC_H_ */