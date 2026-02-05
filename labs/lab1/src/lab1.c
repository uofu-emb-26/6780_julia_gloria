#include "main.h"
#include "assert.h"
#include "stm32f072xb.h"
#include "stm32f0xx_hal.h"

void SystemClock_Config(void);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
  // Initializes the HAL library
  HAL_Init();

  assert(GPIOC -> MODER == (0x00000000));           // Checks GPIOC MODER registers are cleared
  assert(GPIOC -> OTYPER == (0x0000));              // Checks GPIOC OTYPER registers are cleared
  assert(GPIOC -> OSPEEDR == (0x00000000));         // Checks GPIOC OSPEEDR registers are cleared
  assert(GPIOC -> PUPDR == (0x00000000));           // Checks GPIOC PUPDR registers are cleared
  assert(GPIOC -> ODR == (0x0000));                 // Checks GPIOC ODR registers are cleared
  
  // Initializes the system clock
  SystemClock_Config();     

  //RCC enables system clock to be used for GPIO clock
  HAL_RCC_GPIOC_CLK_ENABLE();
  
  // Instead of using the HAL library, we can program the direct memory address for each peripheral register.
  // The memory address for GPIOC is 0x4800 0800.
  // The offset for input mode is 0x00, so no offset address is calculated.
  // ST provides header files for each offset (The following shows this shortcut).

  GPIOC -> MODER |= 0x50000;               // GPIOC MODER9 and MODER8 set to 01 and 01 for general purpose output mode

  // The offset for the output type is 0x04, so this memory address is 0x4800 0804.
  // &= used to clear by bitwise-AND operation on 0s.

  GPIOC -> OTYPER &= ~(0x300);            // GPIOC OT9 and OT8 cleared to 0 for push-pull output type.

  // The offset for the output speed is 0x08, so this memory address is 0x4800 0808.

  GPIOC -> OSPEEDR &= ~(0x50000);         // GPIOC OSPEEDR9 and OSPEEDR8 cleared to x0 for low-speed output configuration.

  // The offset for the pull-up pull-down resistors is 0x0C, so this memory address is 0x4800 080C.

  GPIOC -> PUPDR &= ~(0xF0000);           // GPIOC PUPDR9 and PUPDR8 cleared to 00 for no pull-up, no pull-down configuration.

  // Initializes the specific GPIO peripheral of desired pins
  /*GPIO_InitTypeDef initStr = {GPIO_PIN_8 | GPIO_PIN_9,
                              GPIO_MODE_OUTPUT_PP,
                              GPIO_SPEED_FREQ_LOW,
                              GPIO_NOPULL};*/
  
  // The offset for the output data register is 0x14, so this memory address is 0x4800 0814.
  
  GPIOC -> ODR |= 0x100;                  // GPIOC ODR8 set to 1, output of VDD (Logical 1).

  //HAL_GPIO_Init(GPIOC, &initStr);       // Initialize pins PC8 & PC9
  /*HAL_GPIO_WritePin(GPIOC, 
                    GPIO_PIN_8, 
                    GPIO_PIN_SET);*/      // Start PC8 high

  while (1) {
    HAL_Delay(200);                       // Delay 200 ms
    
    // Toggle on the clock cycle of the GPIO both pins 8 and 9.
    //HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9);

    GPIOC -> ODR ^= 0x300;                // GPIOC ODR9 and ODR8 inverted.
  }
}

void HAL_RCC_GPIOC_CLK_ENABLE(void) {
    RCC -> AHBENR |= RCC_AHBENR_GPIOCEN;
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
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
void Error_Handler(void)
{
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add their own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */
