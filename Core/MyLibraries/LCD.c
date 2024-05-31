/*
 * LCD.c
 *
 *  Created on: May 13, 2024
 *      Author: indie
 */

#include "main.h"
#include "LCD.h"
#include "SPI.h"
#include "GPIO.h"

#include <stdbool.h>

#define ST7735S_SLPOUT			0x11
#define ST7735S_DISPOFF			0x28
#define ST7735S_DISPON			0x29
#define ST7735S_CASET			0x2a
#define ST7735S_RASET			0x2b
#define ST7735S_RAMWR			0x2c
#define ST7735S_MADCTL			0x36
#define ST7735S_COLMOD			0x3a
#define ST7735S_FRMCTR1			0xb1
#define ST7735S_FRMCTR2			0xb2
#define ST7735S_FRMCTR3			0xb3
#define ST7735S_INVCTR			0xb4
#define ST7735S_PWCTR1			0xc0
#define ST7735S_PWCTR2			0xc1
#define ST7735S_PWCTR3			0xc2
#define ST7735S_PWCTR4			0xc3
#define ST7735S_PWCTR5			0xc4
#define ST7735S_VMCTR1			0xc5
#define ST7735S_GAMCTRP1		0xe0
#define ST7735S_GAMCTRN1		0xe1

#define CMD(x)	((x) | 0x100)

#define LCD_OFFSET_X  1
#define LCD_OFFSET_Y  2

static const uint16_t init_table[] = {
  CMD(ST7735S_FRMCTR1), 0x01, 0x2c, 0x2d,
  CMD(ST7735S_FRMCTR2), 0x01, 0x2c, 0x2d,
  CMD(ST7735S_FRMCTR3), 0x01, 0x2c, 0x2d, 0x01, 0x2c, 0x2d,
  CMD(ST7735S_INVCTR), 0x07,
  CMD(ST7735S_PWCTR1), 0xa2, 0x02, 0x84,
  CMD(ST7735S_PWCTR2), 0xc5,
  CMD(ST7735S_PWCTR3), 0x0a, 0x00,
  CMD(ST7735S_PWCTR4), 0x8a, 0x2a,
  CMD(ST7735S_PWCTR5), 0x8a, 0xee,
  CMD(ST7735S_VMCTR1), 0x0e,
  CMD(ST7735S_GAMCTRP1), 0x0f, 0x1a, 0x0f, 0x18, 0x2f, 0x28, 0x20, 0x22,
                         0x1f, 0x1b, 0x23, 0x37, 0x00, 0x07, 0x02, 0x10,
  CMD(ST7735S_GAMCTRN1), 0x0f, 0x1b, 0x0f, 0x17, 0x33, 0x2c, 0x29, 0x2e,
                         0x30, 0x30, 0x39, 0x3f, 0x00, 0x07, 0x03, 0x10,
  CMD(0xf0), 0x01,
  CMD(0xf6), 0x00,
  CMD(ST7735S_COLMOD), 0x05,
  CMD(ST7735S_MADCTL), 0xa0,
};

static uint16_t frame_buffer[LCD_WIDTH * LCD_HEIGHT];
static bool buffor_changed = false;

static void LCDCmd(uint8_t cmd)
{
	GPIO_set(0, DC_GPIO_Port, DC_Pin);
	SPISendInRow(SPI2, cmd);
}

static void LCDData(uint8_t data)
{
	GPIO_set(1, DC_GPIO_Port, DC_Pin);
	SPISendInRow(SPI2, data);
}

static void LCDData16(uint16_t value)
{
	LCDData(value >> 8);
	LCDData(value);
}

static void LCDSend(uint16_t value)
{
	if (value & 0x100) {
		LCDCmd(value);
	} else {
		LCDData(value);
	}
}

static void LCDSetWindow(int x, int y, int width, int height)
{
	LCDCmd(ST7735S_CASET);
	LCDData16(LCD_OFFSET_X + x);
	LCDData16(LCD_OFFSET_X + x + width - 1);

	LCDCmd(ST7735S_RASET);
	LCDData16(LCD_OFFSET_Y + y);
	LCDData16(LCD_OFFSET_Y + y + height- 1);
}

void LCDInit(void)
{
  int i;

  GPIO_set(0, RST_GPIO_Port, RST_Pin);
  LL_mDelay(100);
  GPIO_set(1, RST_GPIO_Port, RST_Pin);
  LL_mDelay(100);

  SPIEnable(SPI2);

  for (i = 0; i < sizeof(init_table) / sizeof(uint16_t); i++) {
    LCDSend(init_table[i]);
  }

  LL_mDelay(200);

  LCDCmd(ST7735S_SLPOUT);
  LL_mDelay(120);

  LCDCmd(ST7735S_DISPON);

  SPIDisable(SPI2);
}

void LCDPutPixel(int x, int y, uint16_t color)
{
	while(!LCDIsBusy(SPI2));
	frame_buffer[x + y * LCD_WIDTH] = color;
	buffor_changed = true;
}

void LCDPutRect(int a_0, int b_0, int a, int b, uint16_t color)
{
	while(!LCDIsBusy(SPI2));
	for (int x = 0; x < a; x++) {
		for (int y = 0; y < b; y++) {
			LCDPutPixel(x+a_0, y+b_0, color);
		}
	}
}

void LCDPutColor(uint16_t color){
	while(!LCDIsBusy(SPI2));
	for (int x = 0; x < LCD_WIDTH; x++) {
		for (int y = 0; y < LCD_HEIGHT; y++) {
			LCDPutPixel(x, y, color);
		}
	}
}

void LCDCopy(void)
{
	if(buffor_changed){
		buffor_changed = false;
		while(!LCDIsBusy(SPI2));
		SPIEnable(SPI2);

		LCDSetWindow(0, 0, LCD_WIDTH, LCD_HEIGHT);
		LCDCmd(ST7735S_RAMWR);

		SPIDisable(SPI2);

		GPIO_set(1, DC_GPIO_Port, DC_Pin);
		SPISendDataDma(DMA1, LL_DMA_CHANNEL_5, SPI2, frame_buffer, sizeof(frame_buffer));
	}
}

void LCDTransferDone(DMA_TypeDef* dma, uint32_t channel, SPI_TypeDef *spi)
{
	LL_DMA_ClearFlag_TC5(dma);
	SPIDisableDMA(dma, channel, spi);
}

bool LCDIsBusy(SPI_TypeDef *spi)
{
	if(LL_DMA_IsActiveFlag_TC5(DMA1)){
		return false;
	}
	else
		return true;
}


