/*
 * sensors.c
 *
 * Created: 26.1.2017 20:44:56
 *  Author: jbbja
 */ 

 #include "sensors.h"

void sensors_init(void)
{
	// Initialize sensor look up vector/table
	for (uint16_t i = 0; i < LUT_SIZE; i++)
	{
		CltAdc_LUT[i]		= eeprom_read_byte(EEPROM_CLT_ADC_INDEX + i);
		IatAdc_LUT[i]		= eeprom_read_byte(EEPROM_IAT_ADC_INDEX + i);
		AfrAdc_LUT[i]		= eeprom_read_byte(EEPROM_AFR_ADC_INDEX + i);
	}
}

void sensors_read_adc(void)
{
	// TODO: TURN GLOBAL INTERRUPTS OFF
	engine_realtime.Clt = CltAdc_LUT[math_find_median(AdcData[ADC_CLT_CH], ADC_MEDIAN_FILTER_LENGTH)];
	engine_realtime.Iat = IatAdc_LUT[math_find_median(AdcData[ADC_IAT_CH], ADC_MEDIAN_FILTER_LENGTH)];
	engine_realtime.Afr = AfrAdc_LUT[math_find_median(AdcData[ADC_AFR_CH], ADC_MEDIAN_FILTER_LENGTH)];
	engine_realtime.Map = math_map_adc(engine_config2.MapMin, engine_config2.MapMax, math_find_median(AdcData[ADC_MAP_CH], ADC_MEDIAN_FILTER_LENGTH));
	engine_realtime.TpsAdc = math_find_median(AdcData[ADC_TPS_CH], ADC_MEDIAN_FILTER_LENGTH) >> 2; // Change to 8 bit 
	engine_realtime.Tps = math_map(0, 100, engine_realtime.TpsAdc - engine_config2.TpsMin, engine_config2.TpsMax - engine_config2.TpsMin);
	engine_realtime.BattVolt = math_map_adc(0, 150, math_find_median(AdcData[ADC_BATT_CH], ADC_MEDIAN_FILTER_LENGTH));
}