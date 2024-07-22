#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>
#include "hk32f0301mxxc.h"
#include "wrapper.h"

extern void EXTI_IRQ_hook_rs(uint8_t line);

void EXTI0(void) {
	if (EXTI_GetFlagStatus(EXTI_Line0) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line0);
		EXTI_IRQ_hook_rs(0);
	}
}

void EXTI1(void) {
	if (EXTI_GetFlagStatus(EXTI_Line1) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line1);
		EXTI_IRQ_hook_rs(1);
	}
}

void EXTI2(void) {
	if (EXTI_GetFlagStatus(EXTI_Line2) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line2);
		EXTI_IRQ_hook_rs(2);
	}
}

void EXTI3(void) {
	if (EXTI_GetFlagStatus(EXTI_Line3) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line3);
		EXTI_IRQ_hook_rs(3);
	}
}

void EXTI4(void) {
	if (EXTI_GetFlagStatus(EXTI_Line4) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line4);
		EXTI_IRQ_hook_rs(4);
	}
}

void EXTI5(void) {
	if (EXTI_GetFlagStatus(EXTI_Line5) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line5);
		EXTI_IRQ_hook_rs(5);
	}
}

void EXTI6(void) {
	if (EXTI_GetFlagStatus(EXTI_Line6) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line6);
		EXTI_IRQ_hook_rs(6);
	}
}

void EXTI7(void) {
	if (EXTI_GetFlagStatus(EXTI_Line7) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line7);
		EXTI_IRQ_hook_rs(7);
	}
}
