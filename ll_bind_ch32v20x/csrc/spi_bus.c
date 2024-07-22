#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include "ch32v20x.h"
#include "spi_bus.h"
#include "wrapper.h"

const SPI_TypeDef* SPI_LIST[] = { NULL, SPI1, SPI2 };

static SPI_TypeDef* get_SPIx(uint32_t bus)
{

	if(bus < sizeof(SPI_LIST)/sizeof(SPI_LIST[0])) {
		return (SPI_TypeDef*)SPI_LIST[bus];
	}
	return NULL;
}

int spi_bus_init(uint32_t bus, uint32_t mode, uint32_t flags, uint32_t baud_rate)
{
    SPI_InitTypeDef  SPI_InitStructure = {0};
    (void)mode;
    (void)baud_rate;

    SPI_TypeDef* spi = get_SPIx(bus);
    if(spi == NULL) {
        return -1;
    }

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;

    SPI_InitStructure.SPI_FirstBit = (flags & SPI_BUS_FLAG_LSB) ? SPI_FirstBit_LSB : SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(spi, &SPI_InitStructure);

    SPI_Cmd(spi, ENABLE);

    return 0;
}

static uint8_t SPIx_ReadWriteByte(SPI_TypeDef* spi, uint8_t TxData)
{
    uint8_t i = 0;

    while(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET)
    {
        i++;
        if(i > 200)
            return 0;
    }

    SPI_I2S_SendData(spi, TxData);
    i = 0;

    while(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_RXNE) == RESET)
    {
        i++;
        if(i > 200)
            return 0;
    }

    return SPI_I2S_ReceiveData(spi);
}

int spi_bus_blocking_transfer(uint32_t bus, uint8_t* p_wr, uint8_t* p_rd, uint32_t size)
{
    SPI_TypeDef* spi = get_SPIx(bus);
    if(spi == NULL) {
        return -1;
    }

    uint8_t wr_data = 0;
    uint8_t rd_data = 0;
    while(size--) {
        if(p_wr) {
            wr_data = *p_wr++;
        }
        rd_data = SPIx_ReadWriteByte(spi, wr_data);
        if(p_rd) {
            *p_rd++ = rd_data;
        }
    }

    return 0;
}