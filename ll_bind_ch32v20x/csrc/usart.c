#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include "ch32v20x.h"
#include "usart.h"
#include "wrapper.h"

extern void USART2_rx_hook_rs(uint8_t data);

void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        uint8_t data = USART_ReceiveData(USART2);
        USART2_rx_hook_rs(data);
    }
}

const USART_TypeDef* USART_LIST[] = { NULL, USART1, USART2, USART3, UART4 };

static USART_TypeDef* get_USARTx(uint32_t usart)
{
	if(usart < sizeof(USART_LIST)/sizeof(USART_LIST[0])) {
		return (USART_TypeDef*)USART_LIST[usart];
	}
	return NULL;
}

int usart_init(uint32_t usart_id, uint32_t flags, uint32_t baud_rate)
{
	(void)flags;

	USART_TypeDef* usart = get_USARTx(usart_id);
    if(usart == NULL) {
        return -1;
    }

    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef  NVIC_InitStructure = {0};

	USART_InitStructure.USART_BaudRate = baud_rate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(usart, &USART_InitStructure);
    if(usart == USART2) {
        USART_ITConfig(usart, USART_IT_RXNE, ENABLE);

        NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
    }
    USART_Cmd(usart, ENABLE);

	return 0;
}

int usart_blocking_write(uint32_t usart_id, const uint8_t* p_buff, uint32_t size)
{
    USART_TypeDef* usart = get_USARTx(usart_id);
    if(usart == NULL) {
        return -1;
    }

    while(size--) {
        while(USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET);
        USART_SendData(usart, *p_buff++);
    }

    return 0;
}

// extern void delay_ns(uint32_t ns);
// int usart_blocking_read(uint32_t usart_id, uint8_t* p_buff, uint32_t size, uint32_t timeout)
// {
//     USART_TypeDef* usart = get_USARTx(usart_id);
//     if(usart == NULL) {
//         return -1;
//     }

//     timeout *= 1000; //ms -> us
//     uint32_t rx_idx = 0;
//     while(1) {
//         while(USART_GetFlagStatus(usart, USART_FLAG_RXNE) == RESET)
//         {
//             if(timeout > 0) {
//                 delay_ns(1000);//1us
//                 timeout--;
//                 if(timeout == 0) {
//                     return rx_idx;
//                 }
//             }
//         }
//         p_buff[rx_idx++] = (USART_ReceiveData(usart));
//     }

//     return rx_idx;
// }