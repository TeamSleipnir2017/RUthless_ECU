/*
 * global.h
 *
 * Created: 7.12.2016 12:39:30
 *  Author: jbbja
 */ 


#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <asf.h>

// DEBUG PARAMETERS
uint32_t DebugCounter;

/************************************************************************/
/* Global definitions:                                                  */
/************************************************************************/
#define ATSAM3X8E_MCK 84000000		// Master clock frequency 84 MHz
#define ON		1					// Define on state
#define OFF		0					// Define off state
#define HIGH	1					// Define high state
#define LOW		0					// Define low state
#define TRUE	1		
#define FALSE	0
/* TODO: Read from EEPROM number of cylinders                           */
#define NR_OF_CYL			4		// Number of cylinders
#define RPM_SCALER			100
#define TEMPERATURE_OFFSET	40		// IAT value from sensor - 40°C 
#define TRIGGER_WHEEL		24		// Number of tooths
#define CYL_DISPLACEMENT	600		// Cylinder displacement in cc
#define MILLI_SEC			1000	// Factor of milliseconds
// Define constants to calculate fuel constant
#define INJECTOR_FLOW_RATE	200		// Should be a variable [cc/min]
#define GASOLINE_DENSITY	740		// kg/m^3
#define GAS_CONSTANT		287		// J/(Kg K)
#define NR_OF_INJECTORS		4		// Should be a variable
#define FUEL_CONST (CYL_DISPLACEMENT * 60 * 1000)/GASOLINE_DENSITY * 1000 / GAS_CONSTANT / NR_OF_INJECTORS * 1000 / INJECTOR_FLOW_RATE
//#define AFTER_START_ENRICH_SCALER 10 // Scale up rounds after start for enrichment calculation

/************************************************************************/
/* Crank, Cam definition:												*/
/************************************************************************/


#define CRANK_TEETH					24		// The amount of teeth on the crank wheel including the missing teeth
#define CYLINDERS					4		// The amount of cylinders
#define TACH_EVENTS					2		// Tach event per cycle http://www.megamanual.com/ms2/wheel.htm
#define CRANK_DEGREE_INTERVAL		360/TACH_EVENTS	
#define DEGREE_TEST					80


volatile uint32_t CrankCurrCycleCounts;		// Current cycle counts of timer 2.2 (timer 9), for crankshaft sensor. Counts between the current tooth and the last tooth of the crank wheel
volatile uint32_t CrankPrevCycleCounts;		// Previous cycle counts of timer 2.2 (timer 9), for crankshaft sensor.
volatile uint32_t CamCurrCycleCounts;		// Current cycle counts of timer 2.2 (timer 9), for camshaft sensor.
volatile uint32_t CrankTimerCounts;			// Last counter value of timer 2.2 (timer 9), for crankshaft sensor
volatile uint32_t CamTimerCounts;			// Last counter value of timer 2.2 (timer 9), for camshaft sensor
volatile uint16_t CrankTooth;				// Variable storing current crank tooth
volatile uint8_t TachPulse;					// Indicates when to calculate new RPM value
volatile uint32_t CrankTachCycleCounts;		// Cumsum of CrankCurrCycleCounts for TachPulse
volatile uint8_t CrankSignalFlag;			// Flag indicating new counter value
volatile uint8_t CamSignalFlag;				// Flag indicating new counter value
volatile uint8_t CrankFirstTach;			
volatile uint8_t CrankSecondTach;
volatile uint32_t CrankFirstInterval;
volatile uint32_t CrankSecondInterval;
volatile uint8_t CamSignalFlag;					// 

/************************************************************************/
/* Timer definitions:                                                   */
/************************************************************************/

#define CYLINDER_1_TIMER 0
#define CYLINDER_2_TIMER 1
#define CYLINDER_3_TIMER 2
#define CYLINDER_4_TIMER 3
#define CYLINDER_5_TIMER 4
#define CYLINDER_6_TIMER 5
#define CYLINDER_7_TIMER 6
#define CYLINDER_8_TIMER 7
#define GLOBAL_TIMER 8

#define GLOBAL_TIMER_FREQ 2625000

uint16_t GlobalTimerFreqADCScaler;
uint16_t GlobalTimerFreqUARTScaler;
uint16_t GlobalTimerFreqTelemetryScaler;



/************************************************************************/
/* Nested vector interrupt priority definitions:                        */
/************************************************************************/
// Lower number means higher priority
#define PIOA_PRIORITY 0		// Crank-, camshaft input
#define TC0_PRIORITY 1		// Cylinder 1, Ignition and injection
#define TC1_PRIORITY 2		// Cylinder 2, Ignition and injection
#define TC2_PRIORITY 3		// Cylinder 3, Ignition and injection
#define TC3_PRIORITY 4		// Cylinder 4, Ignition and injection
#define TC4_PRIORITY 5		// Cylinder 5, Ignition and injection
#define TC5_PRIORITY 6		// Cylinder 6, Ignition and injection
#define TC6_PRIORITY 7		// Cylinder 7, Ignition and injection
#define TC7_PRIORITY 8		// Cylinder 8, Ignition and injection
#define TC8_PRIORITY 9		// General clock, not necessary, maybe put lower in the queue
#define ADC_PRIORITY 10		// Analog to digital conversion completed
#define PIOD_PRIORITY 11	// Speed sensor input
#define UART_PRIORITY 12	// UART communication for tuning enviroment
#define UOTGHS_PRIORITY 13	// USB communication
#define CAN0_PRIORITY 14	// Can bus communication
#define USART1_PRIORITY 15	// Telemetry communication
#define TWI1_PRIORITY 16	// I2C EEPROM communication

/************************************************************************/
/* Table definitions:                                                   */
/************************************************************************/
volatile struct Table3D VE, AFR, IGN;
#define THREE_D_TABLE_SIZE 16	// 3D table size definitions
#define WARMUP_ENRICH_SIZE 10

/************************************************************************/
/* Struct definitions:                                                  */
/************************************************************************/

// Engine status
#define ENGINE_RUNNING		(1 << 0)		// Engine running (above cranking rpm)
#define ENGINE_CRANKING		(1 << 1)		// Engine starting	
#define ENGINE_ASE			(1 << 2)		// Engine in afterstart enrichment
#define ENGINE_WARMING		(1 << 3)		// Engine warming up (CLT below specific value)	
#define ENGINE_TPS_ACC_E	(1 << 4)		// TPS acceleration enrichment enabled
#define ENGINE_TPS_DEC_E	(1 << 5)		// TODO: TPS deceleration enrichment enabled
#define ENGINE_MAP_ACC_E	(1 << 6)		// TODO: MAP acceleration enrichment enabled
#define ENGINE_MAP_DEC_E	(1 << 7)		// TODO: MAP deceleration enrichment enabled


volatile struct engine_config2_ engine_config2;
volatile struct engine_config4_ engine_config4;
volatile struct engine_config6_ engine_config6;
volatile struct engine_config7_ engine_config7;
volatile struct engine_config8_ engine_config8;
volatile struct engine_config9_ engine_config9;
volatile struct engine_realtime_ engine_realtime; 

struct cylinder_
{
	uint32_t IgnCntTimingOn;	// Ignition coil cylinder "x" counter ON value
	uint32_t IgnCntTimingOff;	// Ignition coil cylinder "x" counter OFF value
	uint32_t InjCntTimingOn;	// Injector cylinder "x" counter ON value
	uint32_t InjCntTimingOff;	// Injector cylinder "x" counter OFF value
	/* TODO: IF secondary injector                                      */
};
volatile struct cylinder_ cylinder[NR_OF_CYL]; // Create an instance of the struct defined above 

/************************************************************************/
/* Functions:                                                           */
/************************************************************************/
void global_init(void);
void cylinder_init(void);		// Initialize all variables to 0


/************************************************************************/
/* Hardware definitions:                                                */
/************************************************************************/

/* ANALOG INPUTS */
#define ADC_MAP			PIO_PA16		// DUE A0, Channel 7
#define ADC_MAP_CH		ADC_CH7
#define ADC_IAT			PIO_PA24		// DUE A1, Channel 6
#define ADC_IAT_CH		ADC_CH6
#define ADC_AFR			PIO_PA23		// DUE A2, Channel 5
#define ADC_AFR_CH		ADC_CH5
#define ADC_CLT			PIO_PA22		// DUE A3, Channel 4
#define ADC_CLT_CH		ADC_CH4
#define ADC_BARO		PIO_PA6			// DUE A4, Channel 3
#define ADC_BARO_CH		ADC_CH3
//#define ADC_TPS2		PIO_PA4			// DUE A5, Channel 2
//#define ADC_TPS2		ADC_CH2
#define ADC_TPS			PIO_PA2			// DUE A7, Channel 0
#define ADC_TPS_CH		ADC_CH0
#define ADC_A8_AUX		PIO_PB17		// DUE A9, Channel 10
#define ADC_A8_AUX_CH	ADC_CH10	
#define ADC_A9_AUX		PIO_PB18		// DUE A9, Channel 11
#define ADC_A9_AUX_CH	ADC_CH11	
#define ADC_BATT		PIO_PB19		// DUE A10, Channel 12
#define ADC_BATT_CH		ADC_CH12
#define ADC_A11_AUX		PIO_PB20		// DUE A11, Channel 13
#define ADC_A11_AUX_CH	ADC_CH13

/* DIGITAL INPUTS */
#define CRANK_SIGNAL PIO_PA7	// DUE D34
#define CAM_SIGNAL PIO_PA15		// DUE D26
#define SPEED_SENSOR_SIGNAL	PIO_PD2		// DUE D27
#define LAUNCH_CONTROL_SIGNAL PIO_PD6	// DUE D29
#define NEUTRAL_SWITCH_SIGNAL PIO_PA21	// UNKNOWN
#define REVERSE_SWITCH_SIGNAL PIO_PA19	// DUE D42

/* DIGITAL OUTPUTS */
// Injectors
#define INJ1_OUT PIO_PC1		// DUE Core pin 55  - D33
#define INJ2_OUT PIO_PD0		// DUE Core pin 13  - D25
#define INJ3_OUT PIO_PC3		// DUE Core pin 60  - D35
#define INJ4_OUT PIO_PC5		// DUE Core pin 63  - D37
#define INJ5_OUT PIO_PC13		// DUE Core pin 95  - D50
#define INJ6_OUT PIO_PC7		// DUE Core pin 65  - D39
#define INJ7_OUT PIO_PC15		// DUE Core pin 97  - D48
#define INJ8_OUT PIO_PC9		// DUE Core pin 67  - D41
#define INJ_AUX_OUT PIO_PC11	// DUE Core pin 93  - Unknown
// Ignition coils
#define IGN1_OUT PIO_PC19		// DUE Core pin 108 - D46
#define IGN2_OUT PIO_PC23		// DUE Core pin 114 - D7
#define IGN3_OUT PIO_PC17		// DUE Core pin 110 - D44
#define IGN4_OUT PIO_PC25		// DUE Core pin 44  - D3
#define IGN5_OUT PIO_PC30		// DUE Core pin 116 - D5
#define IGN6_OUT PIO_PC29		// DUE Core pin 42  - Unknown
#define IGN7_OUT PIO_PC28		// DUE Core pin 41  - D10
#define IGN8_OUT PIO_PC27		// DUE Core pin 43  - Unknown
#define IGN_AUX_OUT PIO_PC21	// DUE Core pin 112 - D9

/* COMMUNICATION */
// Serial peripheral interface (SPI) (Future purposes logging to an sd card)
// TODO: Create a variable to define logging delay
#define SPI_MOSI_PIN PIO_PA26	// UNKNOWN
#define SPI_MISO_PIN PIO_PA25	// UNKNOWN
#define SPI_SCLK_PIN PIO_PA27	// UNKNOWN
#define SPI_CHIP_SELECT PIO_PA28// UNKNOWN

// Inter-integrated circuit (IIC) (Write and read from EEPROM)
#define  IIC_SDA PIO_PB12		// UNKNOWN
#define  IIC_SCL PIO_PB13		// UNKNOWN

// UART 1
#define UART_RX1 PIO_PA10		// DUE D19
#define UART_TX1 PIO_PA11		// DUE D18

#include "sensors.h"
#include "math.h"
#include "decoders.h"
#include "interrupts.h"
#include "uart.h"
#include "timers.h"
#include "ADC.h"
#include "tunerstudiocomm.h"
#include "tables.h"
#include "eeprom.h"
#include "fuelcalc.h"
#include "storage.h"


#endif /* GLOBAL_H_ */