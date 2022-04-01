/*
 * it.c
 *
 *  Created on: Mar 21, 2022
 *      Author: ASUS
 */

#include"main.h"
extern ADC_HandleTypeDef adc;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef tim2;
/*
void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&tim2);
}
*/
void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void EXTI0_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}
