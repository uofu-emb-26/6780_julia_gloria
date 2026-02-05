#include "main.h"
#include "stm32f072xb.h"
#include "stm32f0xx_hal.h"

int main(void) {                  
  
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
    
  }
}