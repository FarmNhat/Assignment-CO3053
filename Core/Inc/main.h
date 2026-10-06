/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  *                   CO3053 - Embedded Systems Assignment 2
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <stdbool.h>

/* ====================================================================
 * CẤU HÌNH HỆ THỐNG
 * ==================================================================== */
/**
 * Chế độ mô phỏng nhanh:
 * - 0: Thời gian thực (30 phút = 1800 giây).
 * - 1: Chế độ test mô phỏng Proteus (30 giây chu trình giặt, mỗi giây = 1 phút)
 *      để giảng viên / người test không phải chờ 30 phút trong Proteus.
 */
#define SIMULATION_FAST_MODE        1

#if SIMULATION_FAST_MODE
    #define WASH_DURATION_SECONDS   30      /* 30 giây để test nhanh */
#else
    #define WASH_DURATION_SECONDS   (30 * 60) /* 1800 giây chuẩn 30 phút */
#endif

#define MIN_EXECUTE_COINS           50      /* Cần tối thiểu 50 cents để giặt */
#define STOP_DOUBLE_CLICK_TIMEOUT   2000    /* 2 giây chờ lần bấm STOP thứ 2 (ms) */
#define LED_BLINK_PERIOD_MS         500     /* 500ms chu kỳ nhấp nháy LED */

/* ====================================================================
 * ĐỊNH NGHĨA CHÂN GPIO (STM32F103C8T6 Pinout)
 * ==================================================================== */
/* Nút bấm điều khiển (Nhấn tích cực LOW, kích hoạt internal Pull-Up) */
#define BTN_STOP_PIN                GPIO_PIN_0
#define BTN_STOP_PORT               GPIOA

#define BTN_RUN_PIN                 GPIO_PIN_1
#define BTN_RUN_PORT                GPIOA

#define BTN_PAUSE_PIN               GPIO_PIN_2
#define BTN_PAUSE_PORT              GPIOA

/* Nút nạp tiền (Coins) */
#define BTN_COIN10_PIN              GPIO_PIN_3
#define BTN_COIN10_PORT             GPIOA

#define BTN_COIN20_PIN              GPIO_PIN_4
#define BTN_COIN20_PORT             GPIOA

#define BTN_COIN50_PIN              GPIO_PIN_5
#define BTN_COIN50_PORT             GPIOA

/* Nút mô phỏng lỗi (Error trigger / Toggle) */
#define BTN_ERR_PIN                 GPIO_PIN_6
#define BTN_ERR_PORT                GPIOA

/* Ngõ ra (LEDs & Actuators) */
#define RLED_PIN                    GPIO_PIN_0
#define RLED_PORT                   GPIOB

#define BLED_PIN                    GPIO_PIN_1
#define BLED_PORT                   GPIOB

#define MOTOR_PIN                   GPIO_PIN_8
#define MOTOR_PORT                  GPIOB

#define BUZZER_PIN                  GPIO_PIN_9
#define BUZZER_PORT                 GPIOB

/* Exported functions */
void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
