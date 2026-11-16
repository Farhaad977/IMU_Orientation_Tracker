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
void I2C1_Init(void);
void GPIO_LEDInit(void);	//To initialize the PC13 which is the Blue LED

void TIM10Init(void);

TIM_HandleTypeDef TIMER10;
UART_HandleTypeDef HUART2;
I2C_HandleTypeDef I2CDEV;

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

uint8_t data;
uint8_t ret = 0; uint8_t retm = 0;
uint8_t reg_val;
uint8_t err;
char data_buf[40];
char err_buf[20];
char reg_buf[40];
char ret_buf[40];
int main()
{

	//HAL_Delay(200);
	HAL_Init();
	SystemClockConfig();
	UART_Init();
	I2C1_Init();

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

	//---------------------------------------------------------------------------------------------------------------------------//
	//AD0 Pin is set as LOW (MPU-6050 Address 0x68)
	ret = HAL_I2C_IsDeviceReady(&I2CDEV, (0x68<<1), 5, 3);
	if(ret == HAL_OK)
	{
		snprintf((char*)data_buf,sizeof(data_buf),"MPU-6050 Address: = 0x68\r\n");
		while(HAL_UART_Transmit_IT(&HUART2, (uint8_t*)data_buf, strlen(data_buf))!= HAL_OK);
		retm = HAL_I2C_Mem_Read(&I2CDEV, (0x68 <<1), 0x75, I2C_MEMADD_SIZE_8BIT, &reg_val, 1, 1000);
		if(retm == HAL_OK)
		{
			if(reg_val == 0x68)
			{
				snprintf((char*)reg_buf,sizeof(reg_buf),"WHO_AM_I Register Value = 0x%X(PASS)\r\n",reg_val);
				while(HAL_UART_Transmit_IT(&HUART2, (uint8_t*)reg_buf, strlen(reg_buf))!= HAL_OK);
			}
			else
			{
				snprintf((char*)reg_buf,sizeof(reg_buf),"WHO_AM_I Register Value = 0x%X(FAIL)\r\n",reg_val);
				while(HAL_UART_Transmit_IT(&HUART2, (uint8_t*)reg_buf, strlen(reg_buf))!= HAL_OK);
			}
		}
		else
		{
			snprintf((char*)reg_buf,sizeof(reg_buf),"WHO_AM_I Register Read Failure 0x%X\r\n",retm);
			while(HAL_UART_Transmit_IT(&HUART2, (uint8_t*)reg_buf, strlen(reg_buf))!= HAL_OK);
		}
	}
	else
	{
		snprintf((char*)ret_buf,sizeof(ret_buf),"I2C Device Ready Status = 0x%X\r\n",ret);
		while((HAL_UART_Transmit_IT(&HUART2, (uint8_t*)ret_buf, strlen(ret_buf))!= HAL_OK));
		ret = HAL_I2C_GetError(&I2CDEV);
		snprintf((char*)err_buf,sizeof(err_buf),"I2C Error = 0x%X\r\n",ret);
		while((HAL_UART_Transmit_IT(&HUART2, (uint8_t*)err_buf, strlen(err_buf))!= HAL_OK));
	}
	//---------------------------------------------------------------------------------------------------------------------------//

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

void I2C1_Init(void)
{
	I2CDEV.Instance = I2C1;
	I2CDEV.Init.ClockSpeed = 100000;
	I2CDEV.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	I2CDEV.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2CDEV.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	I2CDEV.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if(HAL_I2C_Init(&I2CDEV) != HAL_OK)
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
