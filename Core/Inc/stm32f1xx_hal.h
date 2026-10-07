/**
 ******************************************************************************
 * @file    stm32f1xx_hal.h
 * @brief   Tương thích HAL driver cho STM32F103C8T6 (Cortex-M3)
 *          Hỗ trợ trực tiếp cho mô phỏng Proteus và nạp chip STM32F103
 ******************************************************************************
 */

#ifndef __STM32F1XX_HAL_H
#define __STM32F1XX_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* ====================================================================
 * ĐỊNH NGHĨA THANH GHI BỘ NHỚ NGOẠI VI STM32F103 (MEMORY MAP)
 * ==================================================================== */
#define PERIPH_BASE           (0x40000000UL)
#define APB1PERIPH_BASE       PERIPH_BASE
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x00010000UL)
#define AHBPERIPH_BASE        (PERIPH_BASE + 0x00020000UL)

/* Cổng GPIO */
#define GPIOA_BASE            (APB2PERIPH_BASE + 0x0800UL)
#define GPIOB_BASE            (APB2PERIPH_BASE + 0x0C00UL)
#define GPIOC_BASE            (APB2PERIPH_BASE + 0x1000UL)

/* Khối RCC */
#define RCC_BASE              (AHBPERIPH_BASE + 0x1000UL)

/* Khối USART1 */
#define USART1_BASE           (APB2PERIPH_BASE + 0x3800UL)

/* Khối Core SysTick (ARM Cortex-M3) */
#define SysTick_BASE          (0xE000E010UL)

/* ====================================================================
 * CẤU TRÚC THANH GHI (REGISTER STRUCTS)
 * ==================================================================== */
typedef struct {
    volatile uint32_t CRL;
    volatile uint32_t CRH;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t BRR;
    volatile uint32_t LCKR;
} GPIO_TypeDef;

typedef struct {
    volatile uint32_t CR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t APB2RSTR;
    volatile uint32_t APB1RSTR;
    volatile uint32_t AHBENR;
    volatile uint32_t APB2ENR;
    volatile uint32_t APB1ENR;
    volatile uint32_t BDCR;
    volatile uint32_t CSR;
} RCC_TypeDef;

typedef struct {
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t GTPR;
} USART_TypeDef;

typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
} SysTick_TypeDef;

#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB               ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC               ((GPIO_TypeDef *) GPIOC_BASE)
#define RCC                 ((RCC_TypeDef *) RCC_BASE)
#define USART1              ((USART_TypeDef *) USART1_BASE)
#define SysTick             ((SysTick_TypeDef *) SysTick_BASE)

/* ====================================================================
 * ĐỊNH NGHĨA CHÂN VÀ CHẾ ĐỘ GPIO
 * ==================================================================== */
#define GPIO_PIN_0          ((uint16_t)0x0001)
#define GPIO_PIN_1          ((uint16_t)0x0002)
#define GPIO_PIN_2          ((uint16_t)0x0004)
#define GPIO_PIN_3          ((uint16_t)0x0008)
#define GPIO_PIN_4          ((uint16_t)0x0010)
#define GPIO_PIN_5          ((uint16_t)0x0020)
#define GPIO_PIN_6          ((uint16_t)0x0040)
#define GPIO_PIN_7          ((uint16_t)0x0080)
#define GPIO_PIN_8          ((uint16_t)0x0100)
#define GPIO_PIN_9          ((uint16_t)0x0200)
#define GPIO_PIN_10         ((uint16_t)0x0400)
#define GPIO_PIN_11         ((uint16_t)0x0800)
#define GPIO_PIN_12         ((uint16_t)0x1000)
#define GPIO_PIN_13         ((uint16_t)0x2000)
#define GPIO_PIN_14         ((uint16_t)0x4000)
#define GPIO_PIN_15         ((uint16_t)0x8000)

typedef enum {
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET = 1
} GPIO_PinState;

#define GPIO_MODE_INPUT       0x00000000U
#define GPIO_MODE_OUTPUT_PP   0x00000001U
#define GPIO_NOPULL           0x00000000U
#define GPIO_PULLUP           0x00000001U
#define GPIO_SPEED_FREQ_LOW   0x00000002U

typedef struct {
    uint32_t Pin;
    uint32_t Mode;
    uint32_t Pull;
    uint32_t Speed;
} GPIO_InitTypeDef;

/* ====================================================================
 * RCC CLOCK MACROS
 * ==================================================================== */
#define __HAL_RCC_GPIOA_CLK_ENABLE()   (RCC->APB2ENR |= (1UL << 2))
#define __HAL_RCC_GPIOB_CLK_ENABLE()   (RCC->APB2ENR |= (1UL << 3))
#define __HAL_RCC_USART1_CLK_ENABLE()  (RCC->APB2ENR |= (1UL << 14))

typedef struct {
    uint32_t OscillatorType;
    uint32_t HSIState;
    uint32_t HSICalibrationValue;
    struct { uint32_t PLLState; } PLL;
} RCC_OscInitTypeDef;

typedef struct {
    uint32_t ClockType;
    uint32_t SYSCLKSource;
    uint32_t AHBCLKDivider;
    uint32_t APB1CLKDivider;
    uint32_t APB2CLKDivider;
} RCC_ClkInitTypeDef;

#define RCC_OSCILLATORTYPE_HSI 0
#define RCC_HSI_ON             1
#define RCC_HSICALIBRATION_DEFAULT 0x10
#define RCC_PLL_NONE           0
#define RCC_CLOCKTYPE_HCLK     1
#define RCC_CLOCKTYPE_SYSCLK   2
#define RCC_CLOCKTYPE_PCLK1    4
#define RCC_CLOCKTYPE_PCLK2    8
#define RCC_SYSCLKSOURCE_HSI   0
#define RCC_SYSCLK_DIV1        0
#define FLASH_LATENCY_0        0

/* ====================================================================
 * USART & HAL STATUS
 * ==================================================================== */
typedef enum {
    HAL_OK       = 0x00U,
    HAL_ERROR    = 0x01U,
    HAL_BUSY     = 0x02U,
    HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

typedef struct {
    uint32_t BaudRate;
    uint32_t WordLength;
    uint32_t StopBits;
    uint32_t Parity;
    uint32_t Mode;
    uint32_t HwFlowCtl;
    uint32_t OverSampling;
} UART_InitTypeDef;

typedef struct {
    USART_TypeDef* Instance;
    UART_InitTypeDef Init;
} UART_HandleTypeDef;

#define UART_WORDLENGTH_8B     0
#define UART_STOPBITS_1        0
#define UART_PARITY_NONE       0
#define UART_MODE_TX_RX        0
#define UART_HWCONTROL_NONE    0
#define UART_OVERSAMPLING_16   0

/* ====================================================================
 * HAL FUNCTIONS PROTOTYPES
 * ==================================================================== */
HAL_StatusTypeDef HAL_Init(void);
uint32_t HAL_GetTick(void);
void HAL_IncTick(void);

HAL_StatusTypeDef HAL_RCC_OscConfig(RCC_OscInitTypeDef *RCC_OscInitStruct);
HAL_StatusTypeDef HAL_RCC_ClockConfig(RCC_ClkInitTypeDef *RCC_ClkInitStruct, uint32_t FLatency);

void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init);
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);

HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size, uint32_t Timeout);

static inline void __disable_irq(void) {
    __asm volatile ("cpsid i" : : : "memory");
}

static inline void __enable_irq(void) {
    __asm volatile ("cpsie i" : : : "memory");
}

#ifdef __cplusplus
}
#endif

#endif /* __STM32F1XX_HAL_H */
