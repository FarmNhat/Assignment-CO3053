/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body for Washing Machine Control Unit
  *                   Target MCU : STM32F103C8T6
  *                   Simulation : Proteus 8 Professional (VSM Cortex-M3)
  *                   Course     : CO3053 - Embedded Systems (HCMUT)
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include "fsm.h"
#include "button.h"
#include "led.h"
#include <stdio.h>

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);

/**
  * @brief Retarget printf to USART1 for Proteus Virtual Terminal
  */
#ifdef __GNUC__
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}

#ifdef __GNUC__
int _write(int file, char *ptr, int len) {
    HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, 0xFFFF);
    return len;
}
#endif

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock (8MHz HSI or HSE for Proteus) */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_USART1_UART_Init();

    /* Initialize Application Modules */
    LED_Init();
    Button_Init();
    FSM_Init();

    printf("\r\n===================================================\r\n");
    printf("   HCMUT - CO3053 EMBEDDED SYSTEMS ASSIGNMENT 2    \r\n");
    printf("   WASHING MACHINE CONTROL UNIT (STM32F103C8)       \r\n");
    printf("===================================================\r\n");
    printf("Pinout Guide:\r\n");
    printf("  [PA0] STOP Button    | [PA3] +10c Coin   | [PB0] RLED\r\n");
    printf("  [PA1] RUN Button     | [PA4] +20c Coin   | [PB1] BLED\r\n");
    printf("  [PA2] PAUSE Button   | [PA5] +50c Coin   | [PB8] MOTOR\r\n");
    printf("  [PA6] ERROR Test BTN |                   | [PB9] BUZZER\r\n");
    printf("---------------------------------------------------\r\n");

    uint32_t last_tick = HAL_GetTick();

    /* Infinite loop (Super-Loop Architecture) */
    while (1)
    {
        uint32_t current_tick = HAL_GetTick();
        uint32_t delta_time = current_tick - last_tick;

        /* Chạy chu kỳ quét mỗi 10ms */
        if (delta_time >= 10) {
            last_tick = current_tick;

            /* 1. Quét trạng thái nút bấm (Chống dội) */
            Button_Update(delta_time);

            /* 2. Kiểm tra sự kiện nút bấm & Dispatch vào FSM */
            if (Button_WasPressed(BTN_ID_COIN10)) {
                FSM_DispatchEvent(EVT_COIN_10);
            }
            if (Button_WasPressed(BTN_ID_COIN20)) {
                FSM_DispatchEvent(EVT_COIN_20);
            }
            if (Button_WasPressed(BTN_ID_COIN50)) {
                FSM_DispatchEvent(EVT_COIN_50);
            }
            if (Button_WasPressed(BTN_ID_RUN)) {
                FSM_DispatchEvent(EVT_BTN_RUN);
            }
            if (Button_WasPressed(BTN_ID_PAUSE)) {
                FSM_DispatchEvent(EVT_BTN_PAUSE);
            }
            if (Button_WasPressed(BTN_ID_STOP)) {
                FSM_DispatchEvent(EVT_BTN_STOP);
            }
            if (Button_WasPressed(BTN_ID_ERROR)) {
                FSM_DispatchEvent(EVT_ERROR_TRIGGER);
            }

            /* 3. Cập nhật FSM & Nhấp nháy LED */
            FSM_Update(delta_time);
        }
    }
}

/**
  * @brief System Clock Configuration (8MHz chuẩn mô phỏng Proteus)
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_SYSCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief USART1 Initialization Function (9600 Baud, 8-N-1)
  */
static void MX_USART1_UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 9600;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief GPIO Initialization Function
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Bật Clock cho GPIOA, GPIOB */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* Cấu hình các chân Nút bấm (PA0 -> PA6): Input Pull-Up (nhấn = GND) */
    GPIO_InitStruct.Pin = BTN_STOP_PIN | BTN_RUN_PIN | BTN_PAUSE_PIN |
                          BTN_COIN10_PIN | BTN_COIN20_PIN | BTN_COIN50_PIN | BTN_ERR_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Cấu hình các chân Ngõ ra (PB0, PB1, PB8, PB9): Output Push-Pull */
    HAL_GPIO_WritePin(GPIOB, RLED_PIN | BLED_PIN | MOTOR_PIN | BUZZER_PIN, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = RLED_PIN | BLED_PIN | MOTOR_PIN | BUZZER_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
  * @brief  This function is executed in case of error occurrence.
  */
void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif
