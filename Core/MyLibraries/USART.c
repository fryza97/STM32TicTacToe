/*
 * USART.c
 *
 *  Created on: Apr 8, 2024
 *      Author: indie
 */

#include "main.h"
#include "USART.h"
#include <stdio.h>

#define PRINTF_USART USART2

void UsartSendByte(USART_TypeDef* UART_PORT, uint8_t Value){
	while(!LL_USART_IsActiveFlag_TXE(UART_PORT));
	LL_USART_TransmitData8(UART_PORT, (uint8_t)Value);
}

void UsartSendString(USART_TypeDef* UART_PORT, uint8_t* Value){
	while(*Value != 0x00){
		if(*Value == '\n'){
			UsartSendByte(UART_PORT, '\r');
		}
		UsartSendByte(UART_PORT, *Value);
		Value++;
	}
}

void UsartNewLine(USART_TypeDef* UART_PORT){
	UsartSendByte(USART2, '\n');
	UsartSendByte(USART2, '\r');
}

int _write(int file, uint8_t *buf, int nbytes){
	uint8_t num_of_byte = 0;

	while (num_of_byte <= nbytes - 1){
		while (!LL_USART_IsActiveFlag_TXE(PRINTF_USART));
		LL_USART_TransmitData8(PRINTF_USART, buf[num_of_byte++]);
	}

	while (!LL_USART_IsActiveFlag_TC(PRINTF_USART));
	return nbytes;
}
