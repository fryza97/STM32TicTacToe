/*
 * SPI.c
 *
 *  Created on: Apr 8, 2024
 *      Author: indie
 */

#include "main.h"
#include "SPI.h"
#include "GPIO.h"

#define DUMMY_BYTE 0x2F

#define CS_PORT CS_GPIO_Port
#define CS_PIN CS_Pin

//polling functions
void SPISendData(SPI_TypeDef *spi, uint8_t *data, uint32_t size){
	uint32_t count = 0;

	LL_SPI_Enable(spi);
	GPIO_set(0, CS_PORT, CS_PIN);

	while(count < size)
	{
		while (!LL_SPI_IsActiveFlag_TXE(spi));
		LL_SPI_TransmitData8(spi, *(data+count));
		count++;
	}

	while (LL_SPI_GetTxFIFOLevel(spi) != LL_SPI_TX_FIFO_EMPTY);
	while (LL_SPI_IsActiveFlag_BSY(spi) != 0);

	LL_SPI_Disable(spi);
	GPIO_set(1, CS_PORT, CS_PIN);

	while (LL_SPI_GetRxFIFOLevel(spi) != LL_SPI_RX_FIFO_EMPTY)
	{
		LL_SPI_ReceiveData8(spi);
	}

	LL_SPI_ClearFlag_OVR(spi);
}

void SPISendByte(SPI_TypeDef *spi, uint8_t data){
	LL_SPI_Enable(spi);
	GPIO_set(0, CS_PORT, CS_PIN);

	while (!LL_SPI_IsActiveFlag_TXE(spi));
	LL_SPI_TransmitData8(spi, data);

	while (LL_SPI_GetTxFIFOLevel(spi) != LL_SPI_TX_FIFO_EMPTY);
	while (LL_SPI_IsActiveFlag_BSY(spi) != 0);

	LL_SPI_Disable(spi);
	GPIO_set(1, CS_PORT, CS_PIN);

	while (LL_SPI_GetRxFIFOLevel(spi) != LL_SPI_RX_FIFO_EMPTY)
	{
		LL_SPI_ReceiveData8(spi);
	}

	LL_SPI_ClearFlag_OVR(spi);
}

void SPIEnable(SPI_TypeDef *spi){
	LL_SPI_Enable(spi);
	GPIO_set(0, CS_PORT, CS_PIN);
}

void SPIDisable(SPI_TypeDef *spi){
	while (LL_SPI_GetTxFIFOLevel(spi) != LL_SPI_TX_FIFO_EMPTY);
	while (LL_SPI_IsActiveFlag_BSY(spi) != 0);

	LL_SPI_Disable(spi);
	GPIO_set(1, CS_PORT, CS_PIN);

	while (LL_SPI_GetRxFIFOLevel(spi) != LL_SPI_RX_FIFO_EMPTY)
	{
		LL_SPI_ReceiveData8(spi);
	}

	LL_SPI_ClearFlag_OVR(spi);
}

void SPISendInRow(SPI_TypeDef *spi, uint8_t data){
	while (!LL_SPI_IsActiveFlag_TXE(spi));
	LL_SPI_TransmitData8(spi, data);
}

void SPISendDataInRow(SPI_TypeDef *spi, uint8_t *data, uint32_t size){
	uint32_t count = 0;

	while(count < size){
		while (!LL_SPI_IsActiveFlag_TXE(spi));
		LL_SPI_TransmitData8(spi, *(data+count));
		count++;
	}
}

//interrupt functions
void SPIInterruptSend (SPI_TypeDef *spi, uint8_t data){
	LL_SPI_TransmitData8(spi, data);
}

uint8_t SPIInterruptReceive (SPI_TypeDef *spi){
	return LL_SPI_ReceiveData8(spi);
}

void SPIInterrruptEnable (SPI_TypeDef *spi){
	LL_SPI_Enable(spi);
	GPIO_set(0, CS_PORT, CS_PIN);
	//LL_SPI_EnableIT_TXE(spi);
}

void SPIInterrruptDisable (SPI_TypeDef *spi){
	LL_SPI_DisableIT_TXE(spi);
	LL_SPI_DisableIT_RXNE(spi);

	while (LL_SPI_GetRxFIFOLevel(spi) != LL_SPI_RX_FIFO_EMPTY);
	while (LL_SPI_IsActiveFlag_BSY(spi) != 0);

	LL_SPI_Disable(spi);

	while (LL_SPI_GetRxFIFOLevel(spi) != LL_SPI_RX_FIFO_EMPTY)
	{
		LL_SPI_ReceiveData8(spi);
	}

	LL_SPI_ClearFlag_OVR(spi);

	GPIO_set(1, CS_PORT, CS_PIN);
}

//DMA functions
void SPISendDataDma(DMA_TypeDef* dma, uint32_t channel, SPI_TypeDef *spi, uint16_t *data, uint32_t size)
{
	LL_DMA_ConfigAddresses(dma, channel, (uint32_t)data, LL_SPI_DMA_GetRegAddr(spi), LL_DMA_GetDataTransferDirection(dma, channel));
	LL_DMA_SetDataLength(dma, channel, size);

	LL_SPI_EnableDMAReq_TX(spi);
	LL_DMA_EnableChannel(dma, channel);
	LL_DMA_EnableIT_TC(dma, channel);
	GPIO_set(0, CS_PORT, CS_PIN);
	LL_SPI_Enable(spi);
}

void SPIDisableDMA(DMA_TypeDef* dma, uint32_t channel, SPI_TypeDef *spi)
{
	LL_SPI_DisableDMAReq_TX(spi);
	LL_DMA_DisableChannel(dma, channel);

	if(!LL_SPI_IsEnabledDMAReq_RX(spi))
	{
		while (LL_SPI_GetTxFIFOLevel(spi) != LL_SPI_TX_FIFO_EMPTY);

		while (LL_SPI_IsActiveFlag_BSY(spi) != 0);

		LL_SPI_Disable(spi);

		while (LL_SPI_GetRxFIFOLevel(spi) != LL_SPI_RX_FIFO_EMPTY)
		{
			LL_SPI_ReceiveData8(spi);
		}

		LL_SPI_ClearFlag_OVR(spi);

		GPIO_set(1, CS_PORT, CS_PIN);
	}
}


