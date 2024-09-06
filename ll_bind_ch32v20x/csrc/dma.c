#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include "ch32v20x.h"
#include "wrapper.h"
#include "dma.h"
#include "print.h"

#define DMA_CHX_TO_FLAG_TCX(ch)		DMA1_FLAG_TC1 << 4 * ((ch) - DMA1_Channel1)/(DMA1_Channel2 - DMA1_Channel1)

struct DmaInfo {
	DMA_Channel_TypeDef * p_ch;
	uint32_t TC_mask;
};

const struct DmaInfo DMA_list[] = { 
	{DMA1_Channel1, DMA1_FLAG_TC1},
	{DMA1_Channel2, DMA1_FLAG_TC2},
	{DMA1_Channel3, DMA1_FLAG_TC3},
	{DMA1_Channel4, DMA1_FLAG_TC4},
	{DMA1_Channel5, DMA1_FLAG_TC5},
	{DMA1_Channel6, DMA1_FLAG_TC6},
	{DMA1_Channel7, DMA1_FLAG_TC7},
	{DMA1_Channel8, DMA1_FLAG_TC8},
	{DMA1_Channel8, DMA1_FLAG_TC8},
};

const struct DmaInfo DmaInfoNull = { NULL, 0 };

static struct DmaInfo get_DMA(uint32_t ch)
{
	if(ch < sizeof(DMA_list)/sizeof(DMA_list[0])) {
		return DMA_list[ch];
	}

	return (struct DmaInfo)DmaInfoNull;
}

int dma_init(uint32_t dma_ch, uint32_t src_addr, uint32_t src_buff_size, 
								uint32_t dst_addr, uint32_t dst_buff_size, 
								uint32_t flags, uint32_t extra_flags)
{
	(void)extra_flags;
    DMA_InitTypeDef DMA_InitStructure = {0};
	uint32_t src_size = flags & DMA_FLAG_SRC_MASK;
	uint32_t dst_size = flags & DMA_FLAG_DST_MASK;
	uint32_t x2x = flags & DMA_FLAG_X_TO_X_MASK;
	uint32_t inc = flags & DMA_FLAG_INC_MASK;
    uint32_t *p_SrcDataSize = NULL;
    uint32_t *p_DstDataSize = NULL;
	uint32_t *p_SrcInc = NULL;
    uint32_t *p_DstInc = NULL;

	struct DmaInfo dma_info = get_DMA(dma_ch);

	if(dma_info.p_ch == NULL) {
		return -1;
	}
	println("# dma_ch %d", dma_ch);
	println("# src_addr 0x%X, buff_size: %d", src_addr, src_buff_size);
	println("# dst_addr 0x%X, buff_size: %d", dst_addr, dst_buff_size);
	println("# flags 0x%X, extra_flags 0x%X", flags, extra_flags);


    DMA_StructInit(&DMA_InitStructure);
	if((x2x == DMA_FLAG_PERIPH_TO_MEM) || (x2x == DMA_FLAG_MEM_TO_MEM)){
		if(x2x == DMA_FLAG_MEM_TO_MEM){
    		DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
		} else {
    		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		}
		
    	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_PeripheralBaseAddr = src_addr;
		DMA_InitStructure.DMA_MemoryBaseAddr = dst_addr;
		p_SrcDataSize = &DMA_InitStructure.DMA_PeripheralDataSize;
		p_DstDataSize = &DMA_InitStructure.DMA_MemoryDataSize;
		p_SrcInc = &DMA_InitStructure.DMA_PeripheralInc;
		p_DstInc = &DMA_InitStructure.DMA_MemoryInc;
	} else if(x2x == DMA_FLAG_MEM_TO_PERIPH) {
    	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
		DMA_InitStructure.DMA_MemoryBaseAddr = src_addr;
		DMA_InitStructure.DMA_PeripheralBaseAddr = dst_addr;
		p_SrcDataSize = &DMA_InitStructure.DMA_MemoryDataSize;
		p_DstDataSize = &DMA_InitStructure.DMA_PeripheralDataSize;
		p_SrcInc = &DMA_InitStructure.DMA_MemoryInc;
		p_DstInc = &DMA_InitStructure.DMA_PeripheralInc;
	} else {
		return -2;
	}

	if(src_size == DMA_FLAG_SRC_BYTE) {
		*p_SrcDataSize = (x2x != DMA_FLAG_MEM_TO_PERIPH) ? DMA_PeripheralDataSize_Byte : DMA_MemoryDataSize_Byte;
	} else if(src_size == DMA_FLAG_SRC_HALFWORD) {
		//src_buff_size *= 2;
		*p_SrcDataSize = (x2x != DMA_FLAG_MEM_TO_PERIPH) ? DMA_PeripheralDataSize_HalfWord : DMA_MemoryDataSize_HalfWord;
	} else if(src_size == DMA_FLAG_SRC_WORD) {
		//src_buff_size *= 4;
		*p_SrcDataSize = (x2x != DMA_FLAG_MEM_TO_PERIPH) ? DMA_PeripheralDataSize_Word : DMA_MemoryDataSize_Word;
	} else {
		return -3;
	}

	if(dst_size == DMA_FLAG_DST_BYTE) {
		*p_DstDataSize = (x2x == DMA_FLAG_MEM_TO_PERIPH) ? DMA_PeripheralDataSize_Byte : DMA_MemoryDataSize_Byte;
	} else if(dst_size == DMA_FLAG_DST_HALFWORD) {
		//dst_buff_size *= 2;
		*p_DstDataSize = (x2x == DMA_FLAG_MEM_TO_PERIPH) ? DMA_PeripheralDataSize_HalfWord : DMA_MemoryDataSize_HalfWord;
	} else if(dst_size == DMA_FLAG_DST_WORD) {
		//dst_buff_size *= 4;
		*p_DstDataSize = (x2x == DMA_FLAG_MEM_TO_PERIPH) ? DMA_PeripheralDataSize_Word : DMA_MemoryDataSize_Word;
	} else {
		return -4;
	}

	if((src_buff_size == 0) || (dst_buff_size == 0)) {
		if(src_buff_size == 0) {
			DMA_InitStructure.DMA_BufferSize = dst_buff_size;
		} else if(dst_buff_size == 0){
			DMA_InitStructure.DMA_BufferSize = src_buff_size;
		} else {
			return -5;
		}
	} else {
		if(src_buff_size < dst_buff_size) {
			DMA_InitStructure.DMA_BufferSize = src_buff_size;
		} else {
			DMA_InitStructure.DMA_BufferSize = dst_buff_size;
		}
	}

    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	if(inc == DMA_FLAG_SRC_INC) {
		*p_DstInc = DMA_MemoryInc_Disable;
	} else if(inc == DMA_FLAG_DST_INC) {
		*p_SrcInc = DMA_MemoryInc_Disable;
	} else if(inc == DMA_FLAG_NONE_INC) {
		*p_SrcInc = DMA_MemoryInc_Disable;
		*p_DstInc = DMA_MemoryInc_Disable;
	}

	if(flags & DMA_FLAG_CIRCULAR_ON) {
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	} else {
    	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	}

    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;

	println("@DMA_PeripheralBaseAddr = 0x%X", DMA_InitStructure.DMA_PeripheralBaseAddr);
	println("@    DMA_MemoryBaseAddr = 0x%X", DMA_InitStructure.DMA_MemoryBaseAddr);
	println("@               DMA_DIR = 0x%X", DMA_InitStructure.DMA_DIR);
	println("@        DMA_BufferSize = 0x%X", DMA_InitStructure.DMA_BufferSize);
	println("@     DMA_PeripheralInc = 0x%X", DMA_InitStructure.DMA_PeripheralInc);
	println("@         DMA_MemoryInc = 0x%X", DMA_InitStructure.DMA_MemoryInc);
	println("@DMA_PeripheralDataSize = 0x%X", DMA_InitStructure.DMA_PeripheralDataSize);
	println("@    DMA_MemoryDataSize = 0x%X", DMA_InitStructure.DMA_MemoryDataSize);
	println("@              DMA_Mode = 0x%X", DMA_InitStructure.DMA_Mode);
	println("@          DMA_Priority = 0x%X", DMA_InitStructure.DMA_Priority);
	println("@               DMA_M2M = 0x%X", DMA_InitStructure.DMA_M2M);

    DMA_Init(dma_info.p_ch, &DMA_InitStructure);

    DMA_ClearFlag(dma_info.TC_mask);

	return 0;
}

int dma_ctrl(uint32_t dma_ch, uint32_t work)
{
	struct DmaInfo dma_info = get_DMA(dma_ch);

	if(dma_info.p_ch == NULL) {
		return -1;
	}

	switch(work) {
	case DMA_CTRL_START:
		DMA_Cmd(dma_info.p_ch, ENABLE);
	break;
	case DMA_CTRL_STOP:
		DMA_Cmd(dma_info.p_ch, DISABLE);
	break;
	case DMA_CTRL_WAIT:
		if(dma_info.p_ch->CFGR & DMA_CFGR1_EN)
		{
			while(DMA_GetFlagStatus(dma_info.TC_mask) == RESET);
		}
	break;
	default:
		return -2;
	}

	return 0;
}
