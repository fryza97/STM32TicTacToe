/*
 * SPI.h
 *
 *  Created on: Apr 8, 2024
 *      Author: indie
 */

#ifndef MYLIBRARIES_MYSPI_SPI_H_
#define MYLIBRARIES_MYSPI_SPI_H_

//polling functions
void SPISendData(SPI_TypeDef *spi, uint8_t *data, uint32_t size);
void SPISendByte(SPI_TypeDef *spi, uint8_t data);
void SPIEnable(SPI_TypeDef *spi);
void SPIDisable(SPI_TypeDef *spi);
void SPISendInRow(SPI_TypeDef *spi, uint8_t data);
void SPISendDataInRow(SPI_TypeDef *spi, uint8_t *data, uint32_t size);

//interrupts functions
void SPIInterruptSend (SPI_TypeDef *spi, uint8_t data);
uint8_t SPIInterruptReceive (SPI_TypeDef *spi);

void SPIInterrruptEnable (SPI_TypeDef *spi);
void SPIInterrruptDisable (SPI_TypeDef *spi);

//dma functions
void SPISendDataDma(DMA_TypeDef* dma, uint32_t channel, SPI_TypeDef *spi, uint16_t *data, uint32_t size);
void SPIDisableDMA(DMA_TypeDef* dma, uint32_t channel, SPI_TypeDef *spi);

#endif /* MYLIBRARIES_MYSPI_SPI_H_ */
