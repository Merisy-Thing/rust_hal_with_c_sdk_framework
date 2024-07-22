#include <stdint.h>
#include <stdbool.h>
enum {
	USART_0 = 0x00,
	USART_1 = 0x01,
	USART_2 = 0x02,
	USART_3 = 0x03,
	USART_4 = 0x04,
	USART_5 = 0x05,
	USART_6 = 0x06,
	USART_7 = 0x07,
	USART_MAX,

	USART_STOP_BIT_0_5	= 0x00,
	USART_STOP_BIT_1	= 0x01,
	USART_STOP_BIT_1_5	= 0x02,
	USART_STOP_BIT_2	= 0x03,
	USART_STOP_BIT_MASK = 0x03,

	USART_PARITY_NO     = 0x00 << 2,
	USART_PARITY_EVEN   = 0x01 << 2,
	USART_PARITY_ODD    = 0x02 << 2,
	USART_PARITY_MASK   = 0x03 << 2,

	USART_MODE_LB	    = 0x00 << 4,//loopback
	USART_MODE_RX       = 0x01 << 4,
	USART_MODE_TX       = 0x02 << 4,
	USART_MODE_MASK     = 0x03 << 4,

	USART_HF_NONE       = 0x00 << 6,   
	USART_HF_RTS        = 0x01 << 6,   
	USART_HF_CTS        = 0x02 << 6,   
	USART_HF_RTS_CTS    = 0x03 << 6,
	USART_HF_MASK       = 0x03 << 6,

	USART_DATA_BITS8    = 0x00 << 8,
	USART_DATA_BITS9    = 0x01 << 8,
};

enum {
	SPI_BUS0 = 0x00,
	SPI_BUS1 = 0x01,
	SPI_BUS2 = 0x02,
	SPI_BUS3 = 0x03,
	SPI_BUS4 = 0x04,
	SPI_BUS_MAX,

	SPI_BUS_FLAG_DATA_SIZE_8  = 0x00,
	SPI_BUS_FLAG_DATA_SIZE_16 = 0x01,
	SPI_BUS_FLAG_MSB = 0x00,
	SPI_BUS_FLAG_LSB = 0x02,

	SPI_BUS_MODE0 = 0,
	SPI_BUS_MODE1 = 1,
	SPI_BUS_MODE2 = 2,
	SPI_BUS_MODE3 = 3,
};

enum {
	PA = 0x00,
	PB = 0x01,
	PC = 0x02,
	PD = 0x03,
	PE = 0x04,
	PF = 0x05,
	PG = 0x06,
	PH = 0x07,

	GPIO_FLAG_OUT_PP       = 0x01,
	GPIO_FLAG_OUT_OD       = 0x02,
	GPIO_FLAG_OUT_MASK     = 0x03,

	GPIO_FLAG_AIN          = 0x00 << 2,
	GPIO_FLAG_IN_FLOATING  = 0x01 << 2,
	GPIO_FLAG_IN_PU        = 0x02 << 2,
	GPIO_FLAG_IN_PD        = 0x03 << 2,
	GPIO_FLAG_IN_MASK      = 0x03 << 2,

	GPIO_FLAG_AF_0         = 0x01 << 4,
	GPIO_FLAG_AF_1         = 0x02 << 4,
	GPIO_FLAG_AF_2         = 0x03 << 4,
	GPIO_FLAG_AF_3         = 0x04 << 4,
	GPIO_FLAG_AF_4         = 0x05 << 4,
	GPIO_FLAG_AF_5         = 0x06 << 4,
	GPIO_FLAG_AF_6         = 0x07 << 4,
	GPIO_FLAG_AF_7         = 0x08 << 4,
	GPIO_FLAG_AF_OD        = 0x09 << 4,
	GPIO_FLAG_AF_PP        = 0x0A << 4,
	GPIO_FLAG_AF_MASK      = 0x0F << 4,

	GPIO_FLAG_EXTI_RISING        = 0x01,
	GPIO_FLAG_EXTI_FALLING       = 0x02,
	GPIO_FLAG_EXTI_RISINGFALLING = 0x03,
	GPIO_FLAG_EXTI_ENABLE        = 0x04,
	GPIO_FLAG_EXTI_DISABLE       = 0x05,
};

enum {
    PWM_CH0,
    PWM_CH1,
    PWM_CH2,
    PWM_CH3,
    PWM_CH4,
    PWM_CH5,
    PWM_CH6,
    PWM_CH7,
    PWM_CH_MAX,

	PWM_CTRL_ON  = 0,
	PWM_CTRL_OFF = 1,
	PWM_CTRL_SET_DUTY = 2,
	PWM_CTRL_GET_DUTY = 3,
	PWM_CTRL_GET_MAXDUTY = 4,
	PWM_CTRL_SET_PERIOD = 5,
	PWM_CTRL_GET_PERIOD = 6,

    PWM_CTRL_ACTIVE_HIGH = 10,
    PWM_CTRL_ACTIVE_LOW  = 11,
};

enum INVOKE {
	ID_SYSTEM_INIT = 100,

	ID_LL_DRV_INIT = 200,
	ID_DELAY_NANO  = 201,
	ID_LOG_PUTS    = 202,

	ID_GPIO_INIT = 300,
	ID_GPIO_SET,
	ID_GPIO_GET_INPUT,
	ID_GPIO_GET_OUTPUT,
	ID_GPIO_GET_PORT_REG,
	ID_GPIO_EXTI,

	ID_SPI_BUS_INIT = 400,
	ID_SPI_BUS_DEINIT,
	ID_SPI_BLOCKING_RW,

	ID_USART_INIT  = 500,
	ID_USART_DEINIT,
	ID_USART_WRITE,

	ID_PWM_INIT  = 600,
	ID_PWM_DEINIT,
	ID_PWM_CTRL,
};

int ll_invoke(enum INVOKE invoke_id, ...);
