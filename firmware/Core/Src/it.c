/*
 * it.c
 *
 *  Created on: Oct 28, 2026
 *      Author: farhaad-sheikh-mohammed
 */
#include "it.h"
extern TIM_HandleTypeDef TIMER10;


void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void TIM1_UP_TIM10_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIMER10);
}
