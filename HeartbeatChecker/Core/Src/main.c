/*
 * main.c
 *
 *  Created on: Mar 21, 2022
 *      Author: ASUS
 */

#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include<stdarg.h>

void ConfigureSystemClock(void);
void ConfigureButtonandSensor(void);
void ADC_Init(void);
void ConfigureUART(void);
void Error_Handler(void);

ADC_HandleTypeDef adc;
UART_HandleTypeDef huart2;

char rate[150];


void printmsg(char *format, ...)
{
	char str[80];

	va_list args;
	va_start(args, format);
	vsprintf(str, format, args);
	HAL_UART_Transmit(&huart2, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
	va_end(args);
}


int main()
{
	HAL_Init();
	ConfigureSystemClock();
	ConfigureUART();
	ADC_Init();
	ConfigureButtonandSensor();
	ConfigureUART();

	while(1);

}


void ADC_Init(void)
{
	ADC_HandleTypeDef adc;
	ADC_ChannelConfTypeDef ch_adc;

	adc.Instance = ADC2;
	adc.Init.Resolution = ADC_RESOLUTION_12B;
	adc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	adc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adc.Init.ClockPrescaler =ADC_CLOCK_SYNC_PCLK_DIV8;
	adc.Init.ScanConvMode = DISABLE;
	adc.Init.ContinuousConvMode = ENABLE;
	adc.Init.NbrOfConversion = 1;

	HAL_ADC_Init(&adc);

	ch_adc.Channel = ADC_CHANNEL_1;
	ch_adc.Rank = 1;
	ch_adc.SamplingTime = ADC_SAMPLETIME_144CYCLES;

	HAL_ADC_ConfigChannel(&adc, &ch_adc);

}
void ConfigureSystemClock(void)
{
	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;

	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	osc_init.HSEState = RCC_HSE_ON;
	osc_init.PLL.PLLState = RCC_PLL_ON;
	osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	osc_init.PLL.PLLM = 8;
	osc_init.PLL.PLLN = 336;
	osc_init.PLL.PLLP = 2;

	if(HAL_RCC_OscConfig(&osc_init))
	{
		Error_Handler();
	}


	clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
	clk_init.APB1CLKDivider = RCC_HCLK_DIV4;
	clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
	clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;

	if(HAL_RCC_ClockConfig(&clk_init, FLASH_LATENCY_5))  // 168 Mhz FLatency
	{
		Error_Handler();
	}

}

void ConfigureUART(void)
{
	UART_HandleTypeDef huart2;

	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	if(HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
}

void ConfigureButtonandSensor(void)
{
	GPIO_InitTypeDef button;
	GPIO_InitTypeDef sensor;

	//init button
	button.Mode = GPIO_MODE_IT_FALLING;
	button.Pin = GPIO_PIN_0;
	button.Pull = GPIO_NOPULL;
	button.Speed = GPIO_SPEED_FAST;

	HAL_GPIO_Init(GPIOA, &button);

	//init sensor
	button.Mode = GPIO_MODE_ANALOG;
	button.Pin = GPIO_PIN_1;
	button.Pull = GPIO_NOPULL;

	HAL_GPIO_Init(GPIOC, &sensor);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint16_t rate;
	rate = HAL_ADC_GetValue(&adc);
	printmsg("%d\n", rate);
}

void Error_Handler(void)
{
	while(1);
}