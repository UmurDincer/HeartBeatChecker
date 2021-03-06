/*
 * main.c
 *
 *  Created on: Mar 21, 2022
 *      Author: Umur Edip Dinçer
 */

#include "main.h"
#include <string.h>
#include <stdio.h>



char *user_data = "Put your finger on the sensor. Then, press the button and wait 10 seconds.\r\n";
char data_buffer[50];
uint16_t rawValue;
uint8_t flag = 0;
float Vadc;

void ConfigureSystemClock(void);
void ConfigureButtonandSensor(void);
void ADC_Init(void);
void ConfigureUART(void);
void TIM_Init(void);
void Error_Handler(void);
void delay_us(uint16_t us);
void delay_ms(uint16_t us);

ADC_HandleTypeDef adc;
UART_HandleTypeDef huart2;
TIM_HandleTypeDef tim2;

int main()
{	HAL_Init();
	ConfigureSystemClock();
	ConfigureUART();
	ADC_Init();
	ConfigureButtonandSensor();
	ConfigureUART();
	TIM_Init();

	if(HAL_UART_Transmit(&huart2, (uint8_t*)user_data, (uint16_t)strlen(user_data), HAL_MAX_DELAY) != HAL_OK)
	 {
		 Error_Handler();
	 }

	while(1);
}

void TIM_Init(void)
{
	tim2.Instance = TIM2;
	tim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	tim2.Init.Prescaler = 84 - 1; // 1 us
	tim2.Init.Period = 0xffff;
	tim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	tim2.Init.CounterMode = TIM_COUNTERMODE_UP;

	if(HAL_TIM_Base_Init(&tim2) != HAL_OK)
	{
		Error_Handler();
	}
}

void ADC_Init(void)
{
	ADC_ChannelConfTypeDef ch_adc;

	adc.Instance = ADC1;
	adc.Init.Resolution = ADC_RESOLUTION_12B;
	adc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	adc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adc.Init.ClockPrescaler =ADC_CLOCK_SYNC_PCLK_DIV4;
	adc.Init.ScanConvMode = ENABLE;
	adc.Init.ContinuousConvMode = ENABLE;
	adc.Init.NbrOfConversion = 1;


	HAL_ADC_Init(&adc);

	ch_adc.Channel = ADC_CHANNEL_8;
	ch_adc.Rank = 1;
	ch_adc.SamplingTime = ADC_SAMPLETIME_480CYCLES;

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
	__HAL_RCC_GPIOA_CLK_ENABLE();
	button.Mode = GPIO_MODE_IT_FALLING;
	button.Pin = GPIO_PIN_0;
	button.Pull = GPIO_NOPULL;
	button.Speed = GPIO_SPEED_FAST;

	HAL_GPIO_Init(GPIOA, &button);

	HAL_NVIC_SetPriority(EXTI0_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	//init sensor
	sensor.Mode = GPIO_MODE_ANALOG;
	sensor.Pin = GPIO_PIN_0;
	sensor.Pull = GPIO_NOPULL;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	HAL_GPIO_Init(GPIOB, &sensor);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint16_t counter = 0;
	uint16_t pulse = 0;

	HAL_TIM_Base_Start(&tim2);

	while(counter < 1000)
	{
		counter++;
		HAL_ADC_Start(&adc);
		delay_ms(10);
		if(HAL_ADC_PollForConversion(&adc, HAL_MAX_DELAY) == HAL_OK){

			rawValue = HAL_ADC_GetValue(&adc);
			Vadc = 5 * ((float)rawValue / 4095);

			if(Vadc >= 3 && flag == 0){
				pulse++;
				flag = 1;
			}
			else if(Vadc < 3){
				flag = 0;
			}
		}

		HAL_ADC_Stop(&adc);
	}

		HAL_TIM_Base_Stop(&tim2);
		pulse= pulse * 6;
		sprintf(data_buffer, "\n\nBeat: %d\r\n\n\n", pulse );

		if(HAL_UART_Transmit(&huart2,(uint8_t*)data_buffer,strlen(data_buffer), HAL_MAX_DELAY) != HAL_OK)
		{
			Error_Handler();
		}

		if(HAL_UART_Transmit(&huart2, (uint8_t*)user_data, (uint16_t)strlen(user_data), HAL_MAX_DELAY) != HAL_OK)
		{
			 Error_Handler();
		}

}

void delay_ms(uint16_t ms)
{
	for(uint16_t i = 0; i < ms; ++i){
		delay_us(1000);
	}
}

void delay_us(uint16_t us)
{
	__HAL_TIM_GET_COUNTER(&tim2) = 0;
	while(__HAL_TIM_GET_COUNTER(&tim2) <  us);
}

void Error_Handler(void)
{
	while(1);
}
