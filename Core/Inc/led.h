/**
 ******************************************************************************
 * @file    led.h
 * @brief   Module điều khiển LED (RLED, BLED) và Động cơ (Motor/Buzzer).
 *          Hiện thực mô hình Moore: ngõ ra ổn định, Blink non-blocking.
 ******************************************************************************
 */

#ifndef __LED_H
#define __LED_H

#include "main.h"

typedef enum {
    LED_MODE_OFF,
    LED_MODE_ON,
    LED_MODE_BLINK
} LedMode_t;

void LED_Init(void);
void LED_SetRedMode(LedMode_t mode);
void LED_SetBlueMode(LedMode_t mode);
void Motor_SetState(bool is_running);
void Buzzer_Beep(uint16_t duration_ms);

/* Hàm gọi định kỳ mỗi vòng lặp hoặc ngắt timer (non-blocking) */
void LED_Update(uint32_t delta_time_ms);

#endif /* __LED_H */
