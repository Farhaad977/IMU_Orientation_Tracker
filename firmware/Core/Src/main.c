/*
 * main.c
 *
 *  Created on: Oct 28, 2026
 *      Author: farhaad-sheikh-mohammed
 */
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"

void SystemClockConfig(void);
void Error_Handler(void);
void UART_Init(void);
void GPIO_LEDInit(void);	//To initialize the PC13 which is the Blue LED

void TIM10Init(void);

TIM_HandleTypeDef TIMER10;
UART_HandleTypeDef HUART2;

uint8_t COUNT_VAL = 0;
uint8_t SYSCLK = 0;
uint8_t HCLK = 0;
uint8_t PCLK1 = 0;
uint8_t PCLK2 = 0;
uint32_t COUNT = 0;

volatile uint8_t CNT_FLAG = FALSE;
float IMU_DATA = 0.03245;

const char boot_banner[] = "=== IMU Orientation Tracker ===\r\n"; //Boot Banner
char CLK_FRQ [70];
char CNT_IMU [50];
int main()
{

	//HAL_Delay(200);
	HAL_Init();
	SystemClockConfig();
	UART_Init();

	while(HAL_UART_Transmit_IT(&HUART2, (uint8_t*)boot_banner, strlen(boot_banner))!= HAL_OK); //Program displays Boot Banner First

	SYSCLK = HAL_RCC_GetSysClockFreq()/1000000;
	HCLK = HAL_RCC_GetHCLKFreq()/1000000;
	PCLK1 = HAL_RCC_GetPCLK1Freq()/1000000;
	PCLK2 = HAL_RCC_GetPCLK2Freq()/1000000;

	snprintf((char*)CLK_FRQ,sizeof(CLK_FRQ),"SYSCLK = %dMHz HCLK = %dMHz PCLK1 = %dMHz PCLK2 = %dMHz\r\n",SYSCLK,HCLK,PCLK1,PCLK2); //Printing Clock Settings

	while(HAL_UART_Transmit_IT(&HUART2, (uint8_t*)CLK_FRQ, strlen((char*)CLK_FRQ)) != HAL_OK);

	GPIO_LEDInit();
	TIM10Init();

	//Starting Timer in Interrupt Mode
	HAL_TIM_Base_Start_IT(&TIMER10);
	HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_PLLCLK, RCC_MCODIV_1); //Outputting PLLCLK on MCU Clock Output PA8 Pin
	while(1)
	{
		//This executes every 2nd time Timer 10 Interrupt occurs(0.5*2=1s) displaying IMU Data and Counter Value.
		if(CNT_FLAG)
		{
			COUNT++;
			snprintf((char*)CNT_IMU,sizeof(CNT_IMU),"COUNT = %ld\r\nIMU Reading = %f\r\n",COUNT,IMU_DATA);
			if(HAL_UART_Transmit_IT(&HUART2, (uint8_t*)CNT_IMU, strlen(CNT_IMU))!= HAL_OK)
			{
				Error_Handler();
			}
			CNT_FLAG = FALSE;
		}
	}
	return 0;
}

void SystemClockConfig(void)
{
	RCC_OscInitTypeDef OSC_INIT = {0};
	RCC_ClkInitTypeDef CLK_INIT = {0};

	//Enable the Clock for Power Controller
	__HAL_RCC_PWR_CLK_ENABLE();
	//Setting Voltage Scale Mode 1 100MHz
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	OSC_INIT.OscillatorType = RCC_OSCILLATORTYPE_HSE; //The Main Clock Source is HSE.
	OSC_INIT.HSEState = RCC_HSE_ON;	//From Schematic, the 25 MHz oscillator is present and hence its set as Crystal Resonator

	//SYSCLK will be PLLCLK based on HSE
	OSC_INIT.PLL.PLLState = RCC_PLL_ON;
	OSC_INIT.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	OSC_INIT.PLL.PLLM = 25;
	OSC_INIT.PLL.PLLN = 200;
	OSC_INIT.PLL.PLLP = 2;
	OSC_INIT.PLL.PLLQ = 4;
	if(HAL_RCC_OscConfig(&OSC_INIT) != HAL_OK)
	{
		Error_Handler();
	}


	CLK_INIT.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	//SYSCLK will be PLLCLK based on HSE
	CLK_INIT.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	CLK_INIT.AHBCLKDivider = RCC_SYSCLK_DIV1;
	CLK_INIT.APB1CLKDivider = RCC_HCLK_DIV2;
	CLK_INIT.APB2CLKDivider = RCC_HCLK_DIV1;
	if(HAL_RCC_ClockConfig(&CLK_INIT, FLASH_ACR_LATENCY_3WS) != HAL_OK) //3WS Flash Latency due to our Freq being 100MHz and Supply Voltage between 2.7V - 3.6V
	{
		Error_Handler();
	}
	// HSI Clock not required after enable HSE and use PLLCLK
	__HAL_RCC_HSI_DISABLE();

}

void UART_Init(void)
{
	HUART2.Instance = USART2;
	HUART2.Init.BaudRate = 115200;
	HUART2.Init.Mode = UART_MODE_TX;
	HUART2.Init.Parity = UART_PARITY_NONE;
	HUART2.Init.StopBits = UART_STOPBITS_1;
	HUART2.Init.WordLength = UART_WORDLENGTH_8B;
	HUART2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	if(HAL_UART_Init(&HUART2)!= HAL_OK)
	{
		Error_Handler();
	}
}

void GPIO_LEDInit(void)
{
	//Enable the Clock for GPIOC Peripheral
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef LEDGPIO = {0};
	LEDGPIO.Mode = GPIO_MODE_OUTPUT_PP;
	LEDGPIO.Pin = GPIO_PIN_13;
	LEDGPIO.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &LEDGPIO);

}

void Error_Handler(void)
{
	//Forever While Loop
	while(1);
}

void TIM10Init(void)
{
	TIMER10.Instance = TIM10;
	TIMER10.Init.Prescaler = 1999; //Reducing TIMER10 Clock from 100MHz to 50kHz
	TIMER10.Init.Period = 25000-1; //This creates a 0.5s Timer UpdateEvent
	if(HAL_TIM_Base_Init(&TIMER10) != HAL_OK)
	{
		Error_Handler();
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	COUNT_VAL++;
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	if(COUNT_VAL == 2)
	{
		COUNT_VAL = 0;
		CNT_FLAG = TRUE; //Flag used to collect data every 1s and transmit it over UART.
	}
}


