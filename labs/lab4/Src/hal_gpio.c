#include <stdint.h>
#include <assert.h>
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
void My_HAL_GPIO_DeInit(GPIO_TypeDef  *GPIOx, uint32_t GPIO_Pin) {
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

void My_HAL_USART_Init(void) {
    USART1 -> BRR = HAL_RCC_GetHCLKFreq() / 115200; // Sets the divisor to a divisor for the desired baud rate.

    USART1 -> CR1 |= (USART_CR1_RE | USART_CR1_TE);
    USART1 -> CR1 |= USART_CR1_UE;

    USART1 -> CR1 |= USART_CR1_RXNEIE;
}

void My_HAL_TIMER_Init(void) {
    // For TIM2, 8 MHz divided by 4 Hz to get 2,000,000
    // Setting prescalar to (32 - 1), where 32 is the actual divisor.
    TIM2 -> PSC = 31;

    // Setting the auto-reload register divides 2,000,000 by 62500, allowing the prescalar to be 32.
    TIM2 -> ARR = 62500;

    // For TIM3, 8 MHz divided by 800 Hz to get 10,000
    // Setting prescalar to (100 - 1), where 100 is the actual divisor.
    TIM3 -> PSC = 99;

    // Setting the auto-reload register divides 100,000 by 100, allowing the prescalar to be 100.
    TIM3 -> ARR = 100;

    // Update interrupt enable for timer 2.
    TIM2 -> DIER |= TIM_DIER_UIE;

    NVIC_EnableIRQ(TIM2_IRQn);

    // Configuring the capture/compare mode register for channels 1 and 2 to an output
    TIM3 -> CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_CC2S);

    // Configuring OC1M to PWM Mode 2 and OC2M to PWM Mode 1
    TIM3 -> CCMR1 |= (TIM_CCMR1_OC1M | (TIM_CCMR1_OC2M & ~(TIM_CCMR1_OC2M_0)));
    assert((TIM3 -> CCMR1 && (TIM_CCMR1_OC1M | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2)) == 0x1);

    // Enabling output compare preload for both channels
    TIM3 -> CCMR1 |= (TIM_CCMR1_OC1PE | TIM_CCMR1_OC2PE);

    // Enabling capture/compare 1 and 2 output.
    TIM3 -> CCER |= (TIM_CCER_CC1E | TIM_CCER_CC2E);

    // Configuring capture/compare registers to 20% of ARR value.
    TIM3 -> CCR1 = 20;
    TIM3 -> CCR2 = 20;

    // Turning on auto-reload preload enabled and update generation turned on
    TIM3 -> CR1 |= TIM_CR1_ARPE;
    TIM3 -> EGR |= TIM_EGR_UG;

    // Enable timer
    TIM2 -> CR1 |= TIM_CR1_CEN;
    TIM3 -> CR1 |= TIM_CR1_CEN;
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

