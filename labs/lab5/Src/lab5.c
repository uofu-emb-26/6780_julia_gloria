#include "main.h"
#include "assert.h"
#include "hal_gpio.h"
#include "stm32f072xb.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_it.h"

volatile char rx_data = 0;
volatile uint8_t rx_flag = 0;

void SystemClock_Config(void);

// USART KEY
// Orange Wire - Orange (TX) to PA10 (RX)
// Yellow Wire - Yellow (RX) to PA9 (TX)

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

int main(void) {
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

  GPIO_InitTypeDef LEDs_config = {
    GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
    GPIO_MODE_OUTPUT_PP,
    GPIO_NOPULL,
    GPIO_SPEED_FREQ_LOW
  };

  My_HAL_GPIO_Init(GPIOA, &USARTconfig);
  My_HAL_GPIO_Init(GPIOB, &I2Cconfig);
  My_HAL_GPIO_Init(GPIOB, &B_outs);
  My_HAL_GPIO_Init(GPIOC, &C_outs);
  My_HAL_GPIO_Init(GPIOC, &LEDs_config);

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
  uint8_t CR1_ADDR = 0x20;
  uint8_t FIFO_CR_ADDR = 0x2E;

  uint8_t OUT_XL_ADDR = 0x28;
  uint8_t OUT_XH_ADDR = 0x29;
  uint8_t OUT_YL_ADDR = 0x2A;
  uint8_t OUT_YH_ADDR = 0x2B;

  uint8_t X_HL_ADDR = 0xA8;
  uint8_t Y_HL_ADDR = 0xAA;

  uint8_t data[10];
  char X_number[10];
  char Y_number[10];
  char number[10];

  char hexString[10];

  uint8_t CR1_CONFIG = 0x0B;
  uint8_t FIFO_CONFIG = 0x20;
  
  My_HAL_I2C_WriteToReg(I2C2, GYROSCOPE_ADDR, CR1_ADDR, 1, CR1_CONFIG);

  usart_print("Gyroscope Enabled\r\n");

  HAL_Delay(5000);

  My_HAL_I2C_WriteToReg(I2C2, GYROSCOPE_ADDR, FIFO_CR_ADDR, 1, FIFO_CONFIG);

  usart_print("FIFO Mode Enabled\r\n");

  HAL_Delay(5000);

  while(1) {
     
    // Lab Exercise 5.1 

    // My_HAL_I2C_ReadFromReg(I2C2, GYROSCOPE_ADDR, WHO_AM_I_ADDR, 1, data);

    // if (I2C2 -> RXDR == 0xD3) {
    //   usart_print ("The WHO AM I register is: 0x");
    //   usart_transmit(HexToChar(data[0] >> 4));
    //   usart_transmit(HexToChar(data[0]));
    //   usart_print("\r\n");
    // }

    // I2C2 -> CR2 |= (I2C_CR2_STOP);

    // Lab Exercise 5.2

    // My_HAL_I2C_ReadFromReg(I2C2, GYROSCOPE_ADDR, OUT_XL_ADDR, 4, data);

    I2C2->CR2 &= ~((0xFF << 16) | (0x7FF << 0));
    I2C2->CR2 |= ((1 << 16) | (0x69 << 1));
    I2C2->CR2 |= I2C_CR2_START;

    //verify no NACK and wait for TXIS
    while(!(I2C2->ISR & (I2C_ISR_TXIS | I2C_ISR_NACKF)));
    I2C2->TXDR = 0xA8;

    //wait for transfer complete
    while(!(I2C2->ISR & I2C_ISR_TC));

    //read 4 bytes of data for X and Y axes using auto-increment
    I2C2->CR2 &= ~((0xFF << 16) | (0x7FF << 0));
    I2C2->CR2 |= ((4 << 16) | (0x69 << 1) | I2C_CR2_RD_WRN);
    I2C2->CR2 |= I2C_CR2_START;

    //read 4 bytes OUT_X_L, OUT_X_H, OUT_Y_L, OUT_Y_H 
    uint8_t xl, xh, yl, yh;
    while(!(I2C2->ISR & I2C_ISR_RXNE));
    xl = I2C2->RXDR;
    
    while(!(I2C2->ISR & I2C_ISR_RXNE));
    xh = I2C2->RXDR;
    
    while(!(I2C2->ISR & I2C_ISR_RXNE));
    yl = I2C2->RXDR;

    while(!(I2C2->ISR & I2C_ISR_RXNE));
    yh = I2C2->RXDR;

    //wait for transfer complete
    while(!(I2C2->ISR & I2C_ISR_TC));
    I2C2->CR2 |= I2C_CR2_STOP;

    //wait for stop to complete
    while(!(I2C2->ISR & I2C_ISR_STOPF));
    I2C2->CR2 |= I2C_ICR_STOPCF;

    int16_t x = (int16_t)((xh << 8) | xl);
    int16_t y = (int16_t)((yh << 8) | yl);

    // My_HAL_I2C_ReadFromReg(I2C2, GYROSCOPE_ADDR, OUT_XL_ADDR, 1, data);

    // uint8_t xlow = data[0];

    // I2C2 -> CR2 |= (I2C_CR2_STOP);

    // My_HAL_I2C_ReadFromReg(I2C2, GYROSCOPE_ADDR, OUT_YH_ADDR, 1, data);

    // uint8_t yhigh = data[0];

    // I2C2 -> CR2 |= (I2C_CR2_STOP);

    // My_HAL_I2C_ReadFromReg(I2C2, GYROSCOPE_ADDR, OUT_YL_ADDR, 1, data);

    // uint8_t ylow = data[0];

    // I2C2 -> CR2 |= (I2C_CR2_STOP);

    // usart_print("Reading y-raw\r\n");

    // My_HAL_I2C_ReadFromReg(I2C2, GYROSCOPE_ADDR, Y_HL_ADDR, 2, data);

    // uint8_t yhigh = data[0];
    // uint8_t ylow = data[1];

    // I2C2 -> CR2 |= (I2C_CR2_STOP);

    // int16_t xValue = (xhigh << 8 | xlow);
    // int16_t yValue = (yhigh << 8 | ylow);

    snprintf(X_number, sizeof(X_number), "%d", x);
    usart_print("X-value: ");
    usart_print(X_number);
    usart_print("\r\n");

    snprintf(Y_number, sizeof(Y_number), "%d", y);
    usart_print("Y-value: ");
    usart_print(Y_number);
    usart_print("\r\n");

    if(x > 1000) {
      My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET); // Set Green
      My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET); // Reset Orange
    }
    else if(x < -1000) {
      My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);   // set orange
      My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET); // reset green
    }
    else {
      My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET); // reset orange
      My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET); // reset green
    }

    if(y > 1000) {
      My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET); // Set red
      My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET); // Reset blue
    }
    else if(y < -1000) {
      My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);   // set blue
      My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET); // reset red
    }
    else {
      My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET); // reset red
      My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET); // reset blue
    }

    HAL_Delay(100);
  }

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
