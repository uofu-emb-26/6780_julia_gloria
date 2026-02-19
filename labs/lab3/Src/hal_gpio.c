#include <stdint.h>
#include <stdio.h>
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>

    // The following is a built-in HAL struct GPIO_InitTypeDef that describes variables necessary for
    // initializing the GPIO of a given register.
    // { uint32_t Pin, uint32_t Mode, uint32_t Pull, uint32_t Speed, uint32_t Alternate }

void My_HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init) {
    
    uint32_t position = 0;
    uint32_t pin;
    uint32_t pinMask = 0;

    for(pin = 0; pin < 16; pin ++) {
        // A mask with a bit at a given pin.
        pinMask = (1 << pin);

        // Checks if the pinMask actually has anything happening to it.
        // GPIO_Init is a struct which contains a Pin variable.
        if ((GPIO_Init->Pin & pinMask) == 0) {
            continue;                           // skips interation
        }
        
        position = pin * 2;                     // position of 32-bit registers

        // Sets GPIO pins to digital input mode
        if (GPIO_Init -> Mode == GPIO_MODE_INPUT) {
            GPIOx -> MODER &= ~((1 << (position + 1)) | (1 << position));
        }

        // Sets GPIO pins to digital output push-pull mode
        else if (GPIO_Init -> Mode == GPIO_MODE_OUTPUT_PP) {
            GPIOx -> MODER &= ~(1 << (position + 1));
            GPIOx -> MODER |= (1 << position);

            GPIOx -> OTYPER &= ~(1 << pin);
        }

        // Sets GPIO pins to digital output open-drain mode
        else if (GPIO_Init -> Mode == GPIO_MODE_OUTPUT_OD) {
            GPIOx -> MODER &= ~(1 << (position + 1));
            GPIOx -> MODER |= (1 << position);

            GPIOx -> OTYPER |= (1 << pin);
        }

        // Sets GPIO pins to alternate function push-pull mode
        else if (GPIO_Init -> Mode == GPIO_MODE_AF_PP) {
            GPIOx -> MODER |= (1 << (position + 1));
            GPIOx -> MODER &= ~(1 << position);

            GPIOx -> OTYPER &= ~(1 << pin);
        }

        // Sets GPIO pins to alternate function open-drain mode
        else if (GPIO_Init -> Mode == GPIO_MODE_AF_OD) {
            GPIOx -> MODER |= (1 << (position + 1));
            GPIOx -> MODER &= ~(1 << position);

            GPIOx -> OTYPER |= (1 << pin);
        }

        // Sets GPIO pins to digital input analog mode.
        else if (GPIO_Init -> Mode == GPIO_MODE_ANALOG) {
            GPIOx -> MODER |= ((1 << (position + 1)) | (1 << position));
        }

        // Error message if given incorrect parameter.
        else {
            fprintf(stderr, "GPIO_InitTypeDef struct operation mode (GPIO_MODE_x defined HAL) is not set properly.\n");
        }

        // Sets GPIO pins speed to low frequency.
        if (GPIO_Init -> Speed == GPIO_SPEED_FREQ_LOW) {
            GPIOx -> OSPEEDR &= ~(1 << position);
        }
        
        // Sets GPIO pins speed to medium frequency.
        else if (GPIO_Init -> Speed == GPIO_SPEED_FREQ_MEDIUM) {
            GPIOx -> OSPEEDR &= ~(1 << (position + 1));
            GPIOx -> OSPEEDR |= (1 << position);
        }

        // Sets GPIO pins speed to high frequency
        else if (GPIO_Init -> Speed == GPIO_SPEED_FREQ_HIGH) {
            GPIOx -> OSPEEDR |= ((1 << (position + 1)) | (1 << position));
        }

        // Error message if given incorrect parameter.
        else {
            fprintf(stderr, "GPIO_InitTypeDef struct speed (GPIO_SPEED_FREQ_x defined HAL) is not set properly.\n");
        }

        // Sets GPIO pins to no pull-up or pull-down resistors.
        if (GPIO_Init -> Pull == GPIO_NOPULL) {
            GPIOx -> PUPDR &= ~((1 << (position + 1)) | (1 << position));
        }

        // Sets GPIO pins to have a pull-up resistor.
        else if (GPIO_Init -> Pull == GPIO_PULLUP) {
            GPIOx -> PUPDR &= ~(1 << (position + 1));
            GPIOx -> PUPDR |= (1 << position);
        }

        // Sets GPIO pins to have a pull-down resistor.
        else if (GPIO_Init -> Pull == GPIO_PULLDOWN) {
            GPIOx -> PUPDR &= ~(1 << position);
            GPIOx -> PUPDR |= (1 << (position + 1));
        }

        // Error message if given incorrect parameter.
        else {
            fprintf(stderr, "GPIO_InitTypeDef struct pull resistors (GPIO_PULLx defined HAL) is not set properly.\n");
        }
    }
}


/*
void My_HAL_GPIO_DeInit(GPIO_TypeDef  *GPIOx, uint32_t GPIO_Pin)
{
}
*/

GPIO_PinState My_HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    return ((GPIOx -> IDR & GPIO_Pin) && GPIO_Pin);
}

void My_HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState) {
    if (PinState == GPIO_PIN_SET) {
        GPIOx -> ODR |= GPIO_Pin;
    }

    else if (PinState == GPIO_PIN_RESET) {
        GPIOx -> ODR &= ~GPIO_Pin;
    }

    else {
        fprintf(stderr, "PinState (GPIO_PIN_SET or GPIO_PIN_RESET) not set properly.\n");
    }
}

void My_HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    GPIOx -> ODR ^= GPIO_Pin;
}

void My_HAL_TIMER_Init(void) {
    // Clear TIM2 registers
    // TIM2 -> PSC &= ~(0xFFFF);
    // TIM2 -> ARR &= ~(0xFFFF);

    // 8 MHz clock needs to be divided by 2000000 to get 4Hz.
    // Setting prescalar to 32
    TIM2 -> PSC = 32;

    // Setting the auto-reload register to 62500
    TIM2 -> ARR = 62500;

    // Setting count to 0
    // TIM2 -> CNT = 0x0;

    // Generate an update event
    // TIM2 -> EGR |= TIM_EGR_UG; 

    // Update interrupt enable
    TIM2 -> DIER |= TIM_DIER_UIE;

    NVIC_EnableIRQ(TIM2_IRQn);

    // Enable timer
    TIM2 -> CR1 |= TIM_CR1_CEN;
}

void My_HAL_EXTI0_ENABLE(void) {
    // Enable the EXTI0
    EXTI -> IMR |= 0x1;
    // Enabling rising trigger detection
    EXTI -> RTSR |= 0x1;
    // Disabling falling trigger detection
    EXTI -> FTSR &= ~(0x1);
}

void PA0_EXTI0(void) {
    SYSCFG -> EXTICR[0] &= ~(0x7);
}

