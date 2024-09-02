#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include "ch32v20x.h"
#include "i2c.h"
#include "wrapper.h"

extern void ffi_i2c_init();
extern int ffi_i2c_read(uint16_t dev_addr, uint8_t *p_read, uint32_t size);
extern int ffi_i2c_write(uint16_t dev_addr, const uint8_t *p_write, uint32_t size);
extern int ffi_i2c_write_read(uint16_t dev_addr, const uint8_t *p_write, uint32_t wr_size, uint8_t *p_read, uint32_t rd_size);

int i2c_init(uint32_t bus_id, uint32_t clk_hz, uint32_t flags)
{
	(void)bus_id;
	(void)clk_hz;
	(void)flags;

	ffi_i2c_init();

	return 0;
}

int i2c_read(uint32_t bus_id, uint8_t addr_bits, uint16_t dev_addr, uint8_t *p_read, uint32_t size)
{
	(void)bus_id;
	(void)addr_bits;

	return ffi_i2c_read(dev_addr, p_read, size);
}

int i2c_write(uint32_t bus_id, uint8_t addr_bits, uint16_t dev_addr, const uint8_t *p_write, uint32_t size)
{
	(void)bus_id;
	(void)addr_bits;

	return ffi_i2c_write(dev_addr, p_write, size);
}

int i2c_write_read(uint32_t bus_id, uint8_t addr_bits, uint16_t dev_addr, const uint8_t *p_write, uint32_t wr_size, uint8_t *p_read, uint32_t rd_size)
{
	(void)bus_id;
	(void)addr_bits;

	return ffi_i2c_write_read(dev_addr, p_write, wr_size, p_read, rd_size);
}


__attribute__((weak)) void ffi_i2c_init()
{
}

__attribute__((weak)) int ffi_i2c_read(uint16_t dev_addr, uint8_t *p_read, uint32_t size)
{
	(void)dev_addr;
	(void)p_read;
	(void)size;

	return -1;
}

__attribute__((weak)) int ffi_i2c_write(uint16_t dev_addr, const uint8_t *p_write, uint32_t size)
{
	(void)dev_addr;
	(void)p_write;
	(void)size;

	return -1;
}

__attribute__((weak)) int ffi_i2c_write_read(uint16_t dev_addr, const uint8_t *p_write, uint32_t wr_size, uint8_t *p_read, uint32_t rd_size)
{
	(void)dev_addr;
	(void)p_write;
	(void)p_read;
	(void)wr_size;
	(void)rd_size;

	return -1;
}
