/*
 * decoders.h
 *
 * Created: 12.3.2017 18:25:03
 *  Author: rikardur
 */ 

#include <asf.h>
#include "global.h"
#include "uart.h"

#ifndef DECODERS_H_
#define DECODERS_H_

void decoders_crank_primary(void);
void interrupts_set_crank_signal_filter();
void decoders_tach_event(uint8_t CurrentCrankTooth, uint32_t CurrentCrankToothCounter);
// Calculate timing for injector or ignition coil
void decoders_set_inj_or_ign_event(uint8_t CurrentCrankTooth, uint32_t CurrentCrankToothCounter, struct cylinder_output_manager *Inj_or_Ign, uint32_t PulseWidth, uint16_t DegreeOff);
uint32_t decoders_find_event_tooth(uint32_t CrankDegree, uint32_t CurrentCrankToothCounter); // returns teeth numbers to next event



void decoders_toggle_ign1pin(void);
void decoders_toggle_ign2pin(void);
void decoders_toggle_ign3pin(void);
void decoders_toggle_ign4pin(void);

uint32_t decoders_tooth_degree_correction(void);

#endif /* DECODERS_H_ */