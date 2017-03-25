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
uint32_t timer_read_status(Tc *p_tc, uint32_t ul_channel, uint32_t *CounterValue);

void timers_init1(void);

uint8_t TC8_Overflow;
uint32_t millis;


#endif /* TIMERS_H_ */