#ifndef __GPIO_H__
#define __GPIO_H__

struct PortReg {
	/// Port input data register
	uint16_t* idr;
	/// Port output data register
	uint16_t* odr;
	/// Port bit set register
	uint16_t* bsr;
	/// Port bit reset register
	uint16_t* bcr;
};

int gpio_init(uint32_t port, uint32_t pin, uint32_t flags);
void gpio_set(uint32_t port, uint32_t pin, bool level);
bool gpio_get_input(uint32_t port, uint32_t pin);
bool gpio_get_output(uint32_t port, uint32_t pin);
int gpio_get_port_reg(uint32_t port, struct PortReg* p_port_reg);

#endif //__GPIO_H__
