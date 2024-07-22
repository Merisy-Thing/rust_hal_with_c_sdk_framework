#ifndef __USART_H__
#define __USART_H__

int usart_init(uint32_t usart_id, uint32_t flags, uint32_t baud_rate);
int usart_blocking_write(uint32_t usart_id, const uint8_t* p_buff, uint32_t size);

#endif //__USART_H__
