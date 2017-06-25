/*
 * timers.h
 *
 * Created: 7.12.2016 18:27:52
 *  Author: jbbja
 */ 


#ifndef TIMERS_H_
#define TIMERS_H_

#include "global.h"

void timer_init(uint32_t TimerChannel, uint32_t TimerMode, uint32_t InterruptMode, uint8_t TimerInterruptPriority);
void timer_do_cylinder(uint8_t CylinderNr);
void timer_do_inj_or_ign(struct cylinder_output_manager *Inj_or_Ign, struct cylinder_ *Cyl, struct debug_cylinder_output_ *Debug_Output);
void timers_init1(void);

uint8_t TC8_Overflow;
uint32_t millis;


#endif /* TIMERS_H_ */