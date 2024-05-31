/*
 * ADC.h
 *
 *  Created on: May 19, 2024
 *      Author: indie
 */

#ifndef MYLIBRARIES_ADC_H_
#define MYLIBRARIES_ADC_H_

#pragma once

#include "main.h"
//polling
void ADCStartConversion(void);
void ADCReadData(uint16_t* adc_data);

#endif /* MYLIBRARIES_ADC_H_ */
