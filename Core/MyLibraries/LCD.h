/*
 * LCD.h
 *
 *  Created on: May 13, 2024
 *      Author: indie
 */

#ifndef MYLIBRARIES_LCD_H_
#define MYLIBRARIES_LCD_H_

#pragma once

#include <stdint.h>
#include <stdbool.h>

#define LCD_WIDTH	160
#define LCD_HEIGHT	128

#define BLACK		0x0000
#define RED			0x00f8
#define GREEN		0xe007
#define BLUE		0x1f00
#define YELLOW		0xe0ff
#define MAGENTA		0x1ff8
#define CYAN		0xff07
#define WHITE		0xffff
#define ORANGE 		0xfbe0
#define GRAY 		0x7bfe

void LCDInit(void);
void LCDPutPixel(int x, int y, uint16_t color);
void LCDPutRect(int a_0, int b_0, int a, int b, uint16_t color);
void LCDPutColor(uint16_t color);
void LCDCopy(void);
void LCDTransferDone(DMA_TypeDef* dma, uint32_t channel, SPI_TypeDef *spi);
bool LCDIsBusy(SPI_TypeDef *spi);

#endif /* MYLIBRARIES_LCD_H_ */
