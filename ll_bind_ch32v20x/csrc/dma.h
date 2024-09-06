#ifndef __DMA_H__
#define __DMA_H__

int dma_init(uint32_t dma_ch, uint32_t src_addr, uint32_t src_buff_size, uint32_t dst_addr, uint32_t dst_buff_size, uint32_t flags, uint32_t extra_flags);
int dma_ctrl(uint32_t dma_ch, uint32_t work);

#endif //__DMA_H__
