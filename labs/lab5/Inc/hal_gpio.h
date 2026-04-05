#include <stdint.h>
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>

void My_HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init);
void My_HAL_GPIO_DeInit(GPIO_TypeDef  *GPIOx, uint32_t GPIO_Pin);
GPIO_PinState My_HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void My_HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
void My_HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void My_HAL_I2C_Init(void);
void My_HAL_USART_Init(void);
void My_HAL_TIMER_Init(void);
void My_HAL_EXTI0_ENABLE(void);
void PA0_EXTI0(void);

void My_HAL_I2C_WriteToReg(I2C_TypeDef* I2C, uint8_t device_address, uint8_t register_address, uint8_t nbytes, uint8_t data);
uint8_t My_HAL_I2C_ReadFromReg(I2C_TypeDef* I2C, uint8_t device_address, uint8_t register_address, uint8_t nbytes, uint8_t data[]);