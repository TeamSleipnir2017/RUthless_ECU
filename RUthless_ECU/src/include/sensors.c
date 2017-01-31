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
	// Initialize config parameters
	engine_config.MapLow	= eeprom_read_byte(EEPROM_MAP_LOW_INDEX);
	engine_config.MapHigh	= eeprom_read_int(EEPROM_MAP_HIGH_INDEX);
	engine_config.TpsLow	= eeprom_read_byte(EEPROM_TPS_LOW_INDEX);
	engine_config.TpsHigh	= eeprom_read_byte(EEPROM_TPS_HIGH_INDEX);
}

void sensors_read_adc(void)
{
	// TODO: TURN GLOBAL INTERRUPTS OFF
	engine.Clt = CltAdc_LUT[math_find_median(AdcData[ADC_CLT_CH], ADC_MEDIAN_FILTER_LENGTH)];
	engine.Iat = IatAdc_LUT[math_find_median(AdcData[ADC_IAT_CH], ADC_MEDIAN_FILTER_LENGTH)];
	engine.Afr = AfrAdc_LUT[math_find_median(AdcData[ADC_AFR_CH], ADC_MEDIAN_FILTER_LENGTH)];
	engine.Map = math_map_adc(engine_config.MapLow, engine_config.MapHigh, math_find_median(AdcData[ADC_MAP_CH], ADC_MEDIAN_FILTER_LENGTH));
	engine.TpsAdc = math_find_median(AdcData[ADC_TPS_CH], ADC_MEDIAN_FILTER_LENGTH) >> 2; // Change to 8 bit
	engine.Tps = math_map(0, 100, engine.TpsAdc - engine_config.TpsLow, engine_config.TpsHigh - engine_config.TpsLow);
	engine.Batt = math_map_adc(0, 150, math_find_median(AdcData[ADC_BATT_CH], ADC_MEDIAN_FILTER_LENGTH));
}