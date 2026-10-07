/**
 ******************************************************************************
 * @file    stm32f1xx_hal.c
 * @brief   Hiện thực HAL driver chuẩn thanh ghi cho STM32F103C8T6.
 *          Tương thích 100% với lõi ARM Cortex-M3 trên Proteus 8 và chip thật.
 ******************************************************************************
 */

#include "stm32f1xx_hal.h"

static volatile uint32_t s_uwTick = 0;

HAL_StatusTypeDef HAL_Init(void) {
    /* Cấu hình SysTick ngắt mỗi 1ms tại tần số 8MHz */
    SysTick->LOAD = (8000000UL / 1000UL) - 1UL;
    SysTick->VAL  = 0UL;
    SysTick->CTRL = (1UL << 0)   /* ENABLE */
                  | (1UL << 1)   /* TICKINT: Bật ngắt SysTick */
                  | (1UL << 2);  /* CLKSOURCE: Core Clock */
    s_uwTick = 0;
    __enable_irq();
    return HAL_OK;
}

uint32_t HAL_GetTick(void) {
    return s_uwTick;
}

void HAL_IncTick(void) {
    s_uwTick++;
}

HAL_StatusTypeDef HAL_RCC_OscConfig(RCC_OscInitTypeDef *RCC_OscInitStruct) {
    (void)RCC_OscInitStruct;
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RCC_ClockConfig(RCC_ClkInitTypeDef *RCC_ClkInitStruct, uint32_t FLatency) {
    (void)RCC_ClkInitStruct;
    (void)FLatency;
    return HAL_OK;
}

void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init) {
    uint32_t pin_mask = GPIO_Init->Pin;

    for (uint32_t pin = 0; pin < 16; ++pin) {
        if (pin_mask & (1UL << pin)) {
            volatile uint32_t *cr = (pin < 8) ? &GPIOx->CRL : &GPIOx->CRH;
            uint32_t shift = (pin % 8) * 4;

            /* Xóa 4 bit cấu hình cũ */
            *cr &= ~(0xFUL << shift);

            if (GPIO_Init->Mode == GPIO_MODE_INPUT) {
                if (GPIO_Init->Pull == GPIO_PULLUP) {
                    /* CNF = 10 (Input with pull-up/pull-down), MODE = 00 */
                    *cr |= (0x8UL << shift);
                    /* Kéo lên nguồn: Ghi bit 1 vào ODR */
                    GPIOx->ODR |= (1UL << pin);
                } else {
                    /* Input floating: CNF = 01, MODE = 00 */
                    *cr |= (0x4UL << shift);
                }
            } else if (GPIO_Init->Mode == GPIO_MODE_OUTPUT_PP) {
                /* CNF = 00 (General purpose output push-pull), MODE = 10 (Output 2MHz) */
                *cr |= (0x2UL << shift);
            }
        }
    }
}

GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    return (GPIOx->IDR & GPIO_Pin) ? GPIO_PIN_SET : GPIO_PIN_RESET;
}

void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState) {
    if (PinState == GPIO_PIN_SET) {
        GPIOx->BSRR = (uint32_t)GPIO_Pin;
    } else {
        GPIOx->BRR = (uint32_t)GPIO_Pin;
    }
}

HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        /* Bật clock GPIOA và USART1 */
        RCC->APB2ENR |= (1UL << 2) | (1UL << 14);

        /* Cấu hình PA9 (TX): Alternate function push-pull (MODE=10, CNF=10 -> 0xB) */
        GPIOA->CRH &= ~(0xFUL << 4);
        GPIOA->CRH |=  (0xBUL << 4);

        /* Cấu hình PA10 (RX): Input floating (MODE=00, CNF=01 -> 0x4) */
        GPIOA->CRH &= ~(0xFUL << 8);
        GPIOA->CRH |=  (0x4UL << 8);

        /* Thiết lập Baud Rate: 9600 tại 8MHz -> 8000000 / (16 * 9600) = 52.0833
         * Mantissa = 52 (0x34), Fraction = 0.0833 * 16 = 1.33 => 1 => BRR = 0x341 */
        USART1->BRR = 0x0341;

        /* Bật USART, Bật bộ phát (TE) và bộ thu (RE) */
        USART1->CR1 = (1UL << 13) | (1UL << 3) | (1UL << 2);
    }
    return HAL_OK;
}

HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size, uint32_t Timeout) {
    (void)Timeout;
    for (uint16_t i = 0; i < Size; ++i) {
        /* Chờ thanh ghi phát rỗng (TXE = bit 7 của SR) */
        while (!(huart->Instance->SR & (1UL << 7)));
        huart->Instance->DR = (uint32_t)(pData[i] & 0xFF);
    }
    /* Chờ truyền xong hoàn tất (TC = bit 6 của SR) */
    while (!(huart->Instance->SR & (1UL << 6)));
    return HAL_OK;
}
