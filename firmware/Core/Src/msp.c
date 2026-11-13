/*
 * msp.c
 *
 *  Created on: Oct 28, 2026
 *      Author: farhaad-sheikh-mohammed
 */
#ifndef SRC_MSP_C_
#define SRC_MSP_C_
#include "stm32f4xx_hal.h"

void HAL_MspInit(void)
{
	//Here we will do Low Level Processor Specific Initialization
	//1. Set up the Priority Grouping of the ARM Cortex Mx Processor.
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	//2. Enable the required System Exceptions of the ARM Cortex Mx Processor
	SCB->SHCSR |= (7 << 16); //Enabled UsageFault, MemManageFault, Bus Fault System Exceptions

	//3. Configure Priority of System Exceptions
	HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
	HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
	HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //Here we are going to do the Low Level Initialization of USART2 Peripheral.
	GPIO_InitTypeDef GPIO_UART = {0};
	//1. Enable the Clock for USART2 Peripheral
	__HAL_RCC_USART2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	//2.Pin MUXing Configuration
	GPIO_UART.Pin = GPIO_PIN_2;
	GPIO_UART.Mode = GPIO_MODE_AF_PP;
	GPIO_UART.Pull = GPIO_PULLUP;
	GPIO_UART.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_UART.Alternate = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOA, &GPIO_UART);

	GPIO_UART.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOA, &GPIO_UART);
	//3.Enable the Interrupt and set up the Priority(NVIC Settings)
	HAL_NVIC_SetPriority(USART2_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(USART2_IRQn);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
  //1. TIMER10 Clock Enabled
  __HAL_RCC_TIM10_CLK_ENABLE();
  //2. Enable the IRQ of TIMER10
  HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
  //3. Configure the Priority for TIM1_UP_TIM10_IRQn
  HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 15, 0);
}
#endif
