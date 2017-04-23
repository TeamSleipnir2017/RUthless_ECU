/*
 * interrupts.h
 *
 * Created: 7.12.2016 14:32:06
 *  Author: jbbja
 */ 


#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

#include "global.h"
#include "decoders.h"

#define INTERRUPT_PIN_CHANGE_MODE		0
#define INTERRUPT_RISING_EDGE_MODE		1
#define INTERRUPT_FALLING_EDGE_MODE		2



/************************************************************************/
/* INTERRUPT PRIORITY QUEUE
	0. PIOA_Handler: Crank-, camshaft input
	1. TC0_Handler: Cylinder 1, Ign and Inj
	2. TC1_Handler: Cylinder 2, Ign and Inj
	3. TC2_Handler: Cylinder 3, Ign and Inj
	4. TC3_Handler: Cylinder 4, Ign and Inj
	5. TC4_Handler: Cylinder 5, Ign and Inj
	6. TC5_Handler: Cylinder 6, Ign and Inj
	7. TC6_Handler: Cylinder 7, Ign and Inj
	8. TC7_Handler: Cylinder 8, Ign and Inj
	9. TC8_Handler: Crank signal, cam signal, almenn klukka
	10. ADC_Handler: Analog to digital
	11. PIOD_Handler: Speed sensor input
	12. UOTGHS_Handler: USB comm.
	13. CAN0_Handler: Can bus comm.
	14. USART1_Handler: Telemetry
	15. TWI0_Handler: I2C
                                                                        */
/************************************************************************/

void interrupts_enable_interrupt_vector(uint32_t IRQN, uint32_t Priority);
void interrupts_enable_pio(uint32_t PeripheralID, uint32_t Pin, uint32_t Priority, uint8_t Mode);
void interrupts_check_timer_for_inj_or_ign(struct cylinder_output_manager *Inj_or_Ign, struct cylinder_ *Cyl);

#endif /* INTERRUPTS_H_ */