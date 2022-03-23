/*
 * it.c
 *
 *  Created on: Mar 21, 2022
 *      Author: ASUS
 */

#include"main.h"
extern ADC_HandleTypeDef adc;
extern UART_HandleTypeDef huart2;

void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

