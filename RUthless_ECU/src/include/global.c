/*
 * global.c
 *
 * Created: 13.12.2016 15:02:04
 *  Author: jbbja
 */ 

#include "global.h"

void global_init(void)
{
	// DEBUG
	DebugCounter = 0;
	isDebug = 0;
	// TODO: Read from EEPROM

	// Frequency scalers for ADC, TODO: Finish telemetry
	GlobalTimerFreqADCScaler = 50;
	GlobalTimerFreqUARTScaler = 1;
	GlobalTimerFreqTelemetryScaler = 1;

	// Timer Variables
	CrankCurrCycleCounts = 0;
	CrankPrevCycleCounts = 0;
	CamCurrCycleCounts = 0;
	CrankTimerCounts = 0;
	CamTimerCounts = 0;
	CrankTooth = 0;
	CrankToothCounter = 0;
	CrankSignalFlag = FALSE;
	CamSignalFlag = FALSE;
	CrankRevCounts = 0;
	LastCrankRevCounts = 0;

	storage_struct_read_eeprom_init(&engine_config2, sizeof(engine_config2), EEPROM_CONFIG2_INDEX);
	storage_struct_read_eeprom_init(&engine_config4, sizeof(engine_config4), EEPROM_CONFIG4_INDEX);
	storage_struct_read_eeprom_init(&engine_config6, sizeof(engine_config6), EEPROM_CONFIG6_INDEX);
	storage_struct_read_eeprom_init(&engine_config7, sizeof(engine_config7), EEPROM_CONFIG7_INDEX);
	storage_struct_read_eeprom_init(&engine_config8, sizeof(engine_config8), EEPROM_CONFIG8_INDEX);
	storage_struct_read_eeprom_init(&engine_config9, sizeof(engine_config9), EEPROM_CONFIG9_INDEX);

	TachEvents = engine_config2.NrCylinders / 2; // TODO: should be divided by one for two stroke
	TachPulse = engine_config4.TriggerTeethCount/TachEvents; // Calculate new RPM every half of the trigger wheel (24 tooths)
	TachEventDelayTeeths = (engine_config4.TriggerTeethCount * engine_config4.TriggerAngle) / (360);
	TachCrankDegreeInterval = 360 / TachEvents;
	CrankToothDegreeInterval = CRANK_DEGREE_RESOLUTION / engine_config4.TriggerTeethCount;
	InjectorOpenTime = engine_config2.injOpen * 1000; // injOpen is in 100 of �s, convert it to 100 of ns


	// TODO: NEED TO MAKE CONFIGURABLE IN TUNERSTUDIO
	engine_config2.FiringOrder[0] = 1 - 1;
	engine_config2.FiringOrder[1] = 2 - 1;
	engine_config2.FiringOrder[2] = 4 - 1;
	engine_config2.FiringOrder[3] = 3 - 1;


	storage_init_struct_to_zero(&engine_realtime, sizeof(engine_realtime));

	table_init();
	uint32_t *debug_cylinder_pointer = &debug_cylinder; 
	for (uint32_t i = 0; i < sizeof(debug_cylinder); i++)
		*(debug_cylinder_pointer + i) = 0;
	cylinder_init();
}

void cylinder_init(void)
{
	for (uint8_t i = 0; i < engine_config2.NrCylinders; i++)
	{
		cylinder[i].IgnCntTimingOn = 0;
		cylinder[i].IgnCntTimingOff = 0;
		cylinder[i].IgnToothOn = 0;
		cylinder[i].IgnToothOff = 0;
		cylinder[i].IgnEventPending = FALSE;
		cylinder[i].IgnEventOnSameTooth = FALSE;
		cylinder[i].InjCntTimingOn = 0;
		cylinder[i].InjCntTimingOff = 0;
		cylinder[i].InjToothOn = 0;
		cylinder[i].InjToothOff = 0;
		cylinder[i].InjEventPending = FALSE;
		cylinder[i].InjEventOnSameTooth = FALSE;
		cylinder[i].Ign_pio = PIOC;
		cylinder[i].Inj_pio = PIOC;
		
		Tc *Timer;
		// Initialize timer
		if (i < 3) // TC0
			Timer = TC0;
		else if (i < 6) // TC1
			Timer = TC1;
		else // TC2
			Timer = TC2;
		cylinder[i].Tc_channel = &Timer->TC_CHANNEL[i%3];
		
		timer_init(CYLINDER_1_TIMER + i, TC_CMR_WAVE | TC_CMR_TCCLKS_TIMER_CLOCK3 | TC_CMR_EEVT_XC0, TC_IER_COVFS | TC_IER_CPAS | TC_IER_CPBS | TC_IER_CPCS, TC0_PRIORITY + i);
		
		debug_cylinder[i].RealTimeCylInstance = i + 1;
	}
	cylinder[1].Inj_pio = PIOD; // Because the board developer did not thought it through :)
	cylinder[0].IgnOutputPin = IGN1_OUT;
	cylinder[1].IgnOutputPin = IGN2_OUT;
	cylinder[2].IgnOutputPin = IGN3_OUT;
	cylinder[3].IgnOutputPin = IGN4_OUT;
	cylinder[4].IgnOutputPin = IGN5_OUT;
	cylinder[5].IgnOutputPin = IGN6_OUT;
	cylinder[6].IgnOutputPin = IGN7_OUT;
	cylinder[7].IgnOutputPin = IGN8_OUT;
	cylinder[0].InjOutputPin = INJ1_OUT;
	cylinder[1].InjOutputPin = INJ2_OUT;
	cylinder[2].InjOutputPin = INJ3_OUT;
	cylinder[3].InjOutputPin = INJ4_OUT;
	cylinder[4].InjOutputPin = INJ5_OUT;
	cylinder[5].InjOutputPin = INJ6_OUT;
	cylinder[6].InjOutputPin = INJ7_OUT;
	cylinder[7].InjOutputPin = INJ8_OUT;
	for (uint8_t i = 0; i < engine_config2.NrCylinders; i++)
	{
		pio_set_output(cylinder[i].Ign_pio, cylinder[i].IgnOutputPin, LOW, FALSE, FALSE);
		pio_set_output(cylinder[i].Inj_pio, cylinder[i].InjOutputPin, LOW, FALSE, FALSE);
	}
}

void global_toggle_pin(Pio *PioInterface, uint32_t Pin)
{
	uint32_t Status = PioInterface->PIO_ODSR; // Should return 0 or 
	if (Status & Pin)
		PioInterface->PIO_CODR = Pin;
	else
		PioInterface->PIO_SODR = Pin;
}