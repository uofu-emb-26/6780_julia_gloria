#include "main.h"
#include "assert.h"
#include "hal_gpio.h"
#include "stm32f072xb.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_it.h"

volatile char rx_data = 0;
volatile uint8_t rx_flag = 0;

void SystemClock_Config(void);

/**
  * @brief  The application entry point.
  * @retval int
*/

void usart_transmit(char c) {
  while(!(USART1 -> ISR & USART_ISR_TXE));
  USART1 -> TDR = c;
}

void usart_print(const char* str) {
  while(*str != '\0') {
    usart_transmit(*str++);
  }
}

int main(void) {
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  // Peripheral Clocks Enabled
  HAL_RCC_GPIO_CLK_ENABLE();
  HAL_RCC_USART_ENABLE();

  // USART initial configuration
  GPIO_InitTypeDef USARTconfig = {GPIO_PIN_10 | GPIO_PIN_9,
                                  GPIO_MODE_AF_PP,
                                  GPIO_NOPULL,
                                  GPIO_SPEED_FREQ_LOW};

  GPIO_InitTypeDef LEDs_config = {GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
                                  GPIO_MODE_OUTPUT_PP,
                                  GPIO_NOPULL,
                                  GPIO_SPEED_FREQ_LOW};

  My_HAL_GPIO_Init(GPIOA, &USARTconfig);
  My_HAL_GPIO_Init(GPIOC, &LEDs_config);

  GPIOA -> AFR[1] |= ((1 << GPIO_AFRH_AFSEL9_Pos) | (1 << GPIO_AFRH_AFSEL10_Pos));

  assert(GPIOA -> AFR[1] == 0x110);

  My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);

  My_HAL_USART_Init();

  // enable NVIC for interrupt handler
  // NVIC_EnableIRQ(USART1_IRQn);
  // NVIC_SetPriority(USART1_IRQn, 1);

  while (1) {
    while(!(USART1 -> ISR & USART_ISR_RXNE));
    char c = (char)USART1 -> RDR;

    usart_transmit(c);
    
    usart_print("\n\r");

    switch(c) {
      case 'r':
        My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
        break;
      case 'b':
        My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
        break;
      case 'o':
        My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
        break;     
      case 'g':
        My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
        break; 
      default:
        usart_print("error!!\n\r");
        break;                 
    }
  }
  return -1;
}

void HAL_RCC_GPIO_CLK_ENABLE(void) {
  RCC -> AHBENR |= (RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOAEN);
}

void HAL_RCC_USART_ENABLE(void) {
  RCC -> APB2ENR |= (RCC_APB2ENR_USART1EN);
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
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
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
  while (1) {
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
