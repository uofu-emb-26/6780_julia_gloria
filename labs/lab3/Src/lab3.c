#include "main.h"
#include "assert.h"
#include "hal_gpio.h"
#include "stm32f072xb.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_it.h"

void SystemClock_Config(void);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  // Reset of all peripherals, Initializes the Flash interface and the Systick.
  HAL_Init();

  // Configure the system clock
  SystemClock_Config();

  // Peripheral Clocks Enabled
  HAL_RCC_GPIO_CLK_ENABLE();
  // HAL_RCC_SYSCFG_CLK_ENABLE();
  HAL_RCC_TIMERS_CLK_ENABLE();

  GPIO_InitTypeDef initial = {GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
                              GPIO_MODE_OUTPUT_PP,
                              GPIO_NOPULL,
                              GPIO_SPEED_FREQ_LOW};

  My_HAL_GPIO_Init(GPIOC, &initial);

  My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);

  My_HAL_TIMER_Init();

  NVIC_SetPriority(TIM2_IRQn, 0);
  NVIC_SetPriority(SysTick_IRQn, 3);

  while (1) {
    HAL_Delay(500);                       // Delay 500 ms

    My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
  }

  return -1;
}

void HAL_RCC_GPIO_CLK_ENABLE(void) {
  RCC -> AHBENR |= (RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOAEN);
}

void HAL_RCC_SYSCFG_CLK_ENABLE(void) {
  RCC -> APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
}

void HAL_RCC_TIMERS_CLK_ENABLE(void) {
  // SYSCLK divided by 2 and HCLK divided by 16. 
  // This allows easier configuration of timer prescalars.
  //RCC -> CFGR &= ~(RCC_CFGR_PPRE_DIV16 | RCC_CFGR_HPRE_DIV2);
  
  // Enables timer 2 (TIM2)
  RCC -> APB1ENR |= RCC_APB1ENR_TIM2EN;
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
  /* User can add their own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line) {
  /* User can add their own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */
