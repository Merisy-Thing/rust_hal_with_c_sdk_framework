#ifndef __I2C_H__
#define __I2C_H__


int i2c_init(uint32_t bus_id, uint32_t clk_hz, uint32_t flags);
int i2c_read(uint32_t bus_id, uint8_t addr_bits, uint16_t dev_addr, uint8_t *p_read, uint32_t size);
int i2c_write(uint32_t bus_id, uint8_t addr_bits, uint16_t dev_addr, const uint8_t *p_write, uint32_t size);
int i2c_write_read(uint32_t bus_id, uint8_t addr_bits, uint16_t dev_addr, const uint8_t *p_write, uint32_t wr_size, uint8_t *p_read, uint32_t rd_size);

#endif //__I2C_H__
