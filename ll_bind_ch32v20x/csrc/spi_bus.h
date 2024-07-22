#ifndef __SPI_BUS_H__
#define __SPI_BUS_H__

int spi_bus_init(uint32_t bus, uint32_t mode, uint32_t flags, uint32_t baud_rate);
int spi_bus_blocking_transfer(uint32_t bus, uint8_t* p_wr, uint8_t* p_rd, uint32_t size);

#endif //__SPI_BUS_H__
