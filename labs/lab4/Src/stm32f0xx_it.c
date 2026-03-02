#include "main.h"
#include <assert.h>
#include "hal_gpio.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_it.h"

extern volatile char rx_data;
extern volatile uint8_t rx_flag;

/******************************************************************************/
/*           Cortex-M0 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
   while (1) {
  }
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  while (1) {
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void) {
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void) {
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void) {
  HAL_IncTick();
}

/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f0xx.s).                    */
/******************************************************************************/

/*
void USART1_IRQHandler(void) {
  if (USART1 -> ISR & USART_ISR_RXNE) {
    rx_data = (char)USART1 -> RDR; 
    rx_flag = 1;
  }
}
*/