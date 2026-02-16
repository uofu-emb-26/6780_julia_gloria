#pragma once
#include "stm32f072xb.h"
#include <stdint.h>

void SystemClock_Config(void);

#ifndef HAL_RCC_GPIO_CLK_EN
#define HAL_RCC_GPIO_CLK_EN

void HAL_RCC_GPIO_CLK_ENABLE(void);

#endif 

#ifndef HAL_RCC_SYSCFG_CLK_EN
#define HAL_RCC_SYSCFG_CLK_EN

void HAL_RCC_SYSCFG_CLK_ENABLE(void);

#endif 

int lab1_main(void);
int lab2_main(void);
int lab3_main(void);
int lab4_main(void);
int lab5_main(void);
int lab6_main(void);
int lab7_main(void);

