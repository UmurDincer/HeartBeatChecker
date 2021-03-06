/*
 * msp.c
 *
 *  Created on: Mar 21, 2022
 *      Author: Umur Edip Dinçer
 */

#include"main.h"

extern TIM_HandleTypeDef tim2;

void HAL_MspInit(void)
{
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	SCB->SHCSR |= (0x7 << 16); // set USGFAULTENA, BUSFAULTENA, MEMFAULTENA bits from SHCSR

	HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
	HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
	HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
	__HAL_RCC_ADC1_CLK_ENABLE(); // enable ADC2 clock

}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef gpio_uart;

	__HAL_RCC_USART2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	gpio_uart.Mode = GPIO_MODE_AF_PP;
	gpio_uart.Pin = GPIO_PIN_2;
	gpio_uart.Pull =GPIO_PULLUP;
	gpio_uart.Alternate = GPIO_AF7_USART2; //uart2_tx

	HAL_GPIO_Init(GPIOA, &gpio_uart);

	gpio_uart.Pin = GPIO_PIN_3; //UART2_RX
	HAL_GPIO_Init(GPIOA,&gpio_uart);

	HAL_NVIC_EnableIRQ(USART2_IRQn);
	HAL_NVIC_SetPriority(USART2_IRQn, 15, 0);

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
		if(tim2.Instance == TIM2)
		{
		__HAL_RCC_TIM2_CLK_ENABLE(); // enable clock for TIM2

		}
}
