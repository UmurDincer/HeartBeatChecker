/*
 * it.c
 *
 *  Created on: Mar 21, 2022
 *      Author: Umur Edip Dinçer
 */

#include"main.h"


void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void EXTI0_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}
