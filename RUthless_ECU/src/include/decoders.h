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

void decoders_toggle_ign1pin(void);
void decoders_toggle_ign2pin(void);
void decoders_toggle_ign3pin(void);
void decoders_toggle_ign4pin(void);

#endif /* DECODERS_H_ */