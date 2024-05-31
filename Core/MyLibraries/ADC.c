/*
 * ADC.c
 *
 *  Created on: May 19, 2024
 *      Author: indie
 */

#include "main.h"
#include "ADC.h"
#include "USART.h"

#define ADC_VREF_MV		3300
#define ADC_MAX_VALUE	4096
#define CONVERT_ADC_TO_MV(x) ((ADC_VREF_MV * x) / (ADC_MAX_VALUE - 1))

#define NUMBER_OF_CONVERSION	2

void ADCStartConversion(void){
	LL_ADC_ClearFlag_ADRDY(ADC3);
	LL_ADC_Enable(ADC3);
	while (LL_ADC_IsActiveFlag_ADRDY(ADC3) == 0);
}

void ADCReadData(uint16_t* adc_data){
	LL_ADC_REG_StartConversion(ADC3);

	while (LL_ADC_IsActiveFlag_EOC(ADC3) == 0);
	adc_data[0] = LL_ADC_REG_ReadConversionData12(ADC3);

	while (LL_ADC_IsActiveFlag_EOC(ADC3) == 0);
	adc_data[1] = LL_ADC_REG_ReadConversionData12(ADC3);

	LL_ADC_ClearFlag_EOC(ADC3);
}
