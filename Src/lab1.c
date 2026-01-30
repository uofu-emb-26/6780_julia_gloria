#include <stm32f0xx_hal.h>

int main(void) {
    HAL_Init();

    SystemClock_Config();

    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef initStr = {GPIO_PIN_8 | GPIO_PIN_9,
                                GPIO_MODE_OUTPUT_PP
                                GPIO_SPEED_FREQ_LOW,
                                GPIO_NOPULL};
    
    HAL_GPIO_Init(GPIOC, &initStr); // Initialize pins PC8 & PC9

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET); // Start PC8 high

    while (1) {
        HAL_Delay(200); // Delay 200 ms
        
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9);
    }
}
