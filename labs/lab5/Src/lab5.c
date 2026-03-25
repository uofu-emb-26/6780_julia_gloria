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

char HexToChar(uint8_t hexadecimal) {
  
  hexadecimal &= 0xF;
  
  if (hexadecimal < 0xA) {
    return hexadecimal + '0';
  }
  else {
    return (hexadecimal - 10) + 'A';
  }
}

int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  /* Configure the system clock */
  SystemClock_Config();

  // Peripheral Clocks Enabled
  HAL_RCC_GPIO_CLK_ENABLE();
  HAL_RCC_USART_ENABLE();
  HAL_RCC_I2C_ENABLE();

  GPIO_InitTypeDef USARTconfig = {
    GPIO_PIN_10 | GPIO_PIN_9,
    GPIO_MODE_AF_PP,
    GPIO_NOPULL,
    GPIO_SPEED_FREQ_LOW
  };

  GPIO_InitTypeDef I2Cconfig = {
    GPIO_PIN_11 | GPIO_PIN_13,
    GPIO_MODE_AF_OD,
    GPIO_NOPULL,
    GPIO_SPEED_FREQ_LOW
  };

  GPIO_InitTypeDef B_outs = {
    GPIO_PIN_14,
    GPIO_MODE_OUTPUT_PP,
    GPIO_NOPULL,
    GPIO_SPEED_FREQ_LOW
  };

  GPIO_InitTypeDef C_outs = {
    GPIO_PIN_0,
    GPIO_MODE_OUTPUT_PP,
    GPIO_NOPULL,
    GPIO_SPEED_FREQ_LOW
  };  

  My_HAL_GPIO_Init(GPIOA, &USARTconfig);
  My_HAL_GPIO_Init(GPIOB, &I2Cconfig);
  My_HAL_GPIO_Init(GPIOB, &B_outs);
  My_HAL_GPIO_Init(GPIOC, &C_outs);

  // Sets USART as alternate function
  GPIOA -> AFR[1] |= ((1 << GPIO_AFRH_AFSEL9_Pos) | (1 << GPIO_AFRH_AFSEL10_Pos));

  // Sets I2C2 as alternate function
  GPIOB -> AFR[1] |= ((1 << GPIO_AFRH_AFSEL11_Pos) | (5U << GPIO_AFRH_AFSEL13_Pos));

  My_HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
  My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);

  My_HAL_USART_Init();
  My_HAL_I2C_Init();                  // The code is correct up to here.

  NVIC_EnableIRQ(USART1_IRQn);
  NVIC_SetPriority(USART1_IRQn, 1);

  uint8_t GYROSCOPE_ADDR = 0x69;
  uint8_t WHO_AM_I_ADDR = 0x0F;
  uint8_t CONTROL_ADDR = 0x20;
  char data[1];

  char hexString[10];

  while(1) {
    usart_print("Starting writing transaction...\r\n");

    // Set up writing transaction
    I2C2 -> CR2 &= ~(1 << 10);
    I2C2 -> CR2 |= (GYROSCOPE_ADDR << 1) | (1 << 16) | (1 << 13);

    usart_print("Waiting for flag to be set...\r\n");

    while (!(I2C2 -> ISR & I2C_ISR_TXIS) && !(I2C2 -> ISR & I2C_ISR_NACKF)) {
      // waits until one of these flags are set
    }

    if (I2C2 -> ISR & I2C_ISR_NACKF) {
      usart_print("NACKF is true, transmit doesn't work\r\n");
      break; 
    }

    usart_print ("Transmitting WHO_AM_I address to GYROSCOPE.\r\n");

    I2C2 -> TXDR = WHO_AM_I_ADDR;

    usart_print("Waiting for transfer flag to complete...\r\n");

    while (!(I2C2 -> ISR & I2C_ISR_TC)) {
      // waits until transfer complete flag is set
    }

    usart_print("Starting reading transaction...\r\n");

    // Set up reading transaction
    I2C2 -> CR2 = (GYROSCOPE_ADDR << 1) | (0x1 << 16) | (0x1 << 10) | (0x1 << 13);

    usart_print("Waiting for flag to be set...\r\n");

    while (!(I2C2 -> ISR & I2C_ISR_RXNE) && !(I2C2 -> ISR & I2C_ISR_NACKF)) {
      // waits until one of these flags are set
    }

    if (I2C2 -> ISR & I2C_ISR_NACKF) {
      usart_print("NACKF is true, transmit doesn't work\r\n");
      break; 
    }

    usart_print("Waiting for transfer flag to complete...\r\n");

    while (!(I2C2 -> ISR & I2C_ISR_TC)) {
      // waits until transfer complete flag is set
    }

    usart_print ("Reading WHO_AM_I register.\r\n");

    data[0] = I2C2 -> RXDR; 

    if (I2C2 -> RXDR == 0xD3) {
      usart_print ("The WHO AM I register is: 0x");
      usart_transmit(HexToChar(data[0]));
      usart_transmit(HexToChar(data[0] >> 4));
      usart_print("\r\n");
    }

    I2C2 -> CR2 |= (1 << 14);

    HAL_Delay(1000);
  }

  /* My_HAL_I2C_WriteToReg(I2C2, GYROSCOPE_ADDR, CONTROL_ADDR, 1, (0x1 | (0x1 << 1) | (0x1 << 3)));
  
  usart_print("Turned on Gyroscope.\r\n");

  while (1) {
    My_HAL_I2C_ReadFromReg(I2C2, GYROSCOPE_ADDR, WHO_AM_I_ADDR, 1, data);

    usart_print("Reading WHO AM I register: ");
    usart_print("0x");
    usart_print(data);
    //usart_transmit(HexToChar(data[0] & 0x0F));
    //usart_transmit(HexToChar((data[0] >> 4) & 0x0F));
    usart_print("\r\n");

    HAL_Delay(500);
  } */
  return -1;
}

void HAL_RCC_GPIO_CLK_ENABLE(void) {
  RCC -> AHBENR |= (RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOAEN);
}

void HAL_RCC_USART_ENABLE(void) {
  RCC -> APB2ENR |= RCC_APB2ENR_USART1EN;
}

void HAL_RCC_I2C_ENABLE(void) {
  RCC -> APB1ENR |= RCC_APB1ENR_I2C2EN;
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
