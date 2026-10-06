/**
 ******************************************************************************
 * @file    led.c
 * @brief   Hiện thực module LED và Motor (Moore output, non-blocking toggle).
 ******************************************************************************
 */

#include "led.h"

static LedMode_t s_rled_mode = LED_MODE_OFF;
static LedMode_t s_bled_mode = LED_MODE_OFF;

static uint32_t s_rled_timer = 0;
static uint32_t s_bled_timer = 0;
static bool s_rled_state = false;
static bool s_bled_state = false;
static uint16_t s_buzzer_timer = 0;

void LED_Init(void) {
    s_rled_mode = LED_MODE_OFF;
    s_bled_mode = LED_MODE_OFF;
    s_rled_timer = 0;
    s_bled_timer = 0;
    s_rled_state = false;
    s_bled_state = false;
    s_buzzer_timer = 0;

    HAL_GPIO_WritePin(RLED_PORT, RLED_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BLED_PORT, BLED_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);
}

void LED_SetRedMode(LedMode_t mode) {
    if (s_rled_mode != mode) {
        s_rled_mode = mode;
        s_rled_timer = 0;
        if (mode == LED_MODE_ON) {
            s_rled_state = true;
            HAL_GPIO_WritePin(RLED_PORT, RLED_PIN, GPIO_PIN_SET);
        } else if (mode == LED_MODE_OFF) {
            s_rled_state = false;
            HAL_GPIO_WritePin(RLED_PORT, RLED_PIN, GPIO_PIN_RESET);
        } else { // BLINK
            s_rled_state = true;
            HAL_GPIO_WritePin(RLED_PORT, RLED_PIN, GPIO_PIN_SET);
        }
    }
}

void LED_SetBlueMode(LedMode_t mode) {
    if (s_bled_mode != mode) {
        s_bled_mode = mode;
        s_bled_timer = 0;
        if (mode == LED_MODE_ON) {
            s_bled_state = true;
            HAL_GPIO_WritePin(BLED_PORT, BLED_PIN, GPIO_PIN_SET);
        } else if (mode == LED_MODE_OFF) {
            s_bled_state = false;
            HAL_GPIO_WritePin(BLED_PORT, BLED_PIN, GPIO_PIN_RESET);
        } else { // BLINK
            s_bled_state = true;
            HAL_GPIO_WritePin(BLED_PORT, BLED_PIN, GPIO_PIN_SET);
        }
    }
}

void Motor_SetState(bool is_running) {
    HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_PIN, is_running ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void Buzzer_Beep(uint16_t duration_ms) {
    s_buzzer_timer = duration_ms;
    HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET);
}

void LED_Update(uint32_t delta_time_ms) {
    /* Cập nhật RLED */
    if (s_rled_mode == LED_MODE_BLINK) {
        s_rled_timer += delta_time_ms;
        if (s_rled_timer >= LED_BLINK_PERIOD_MS) {
            s_rled_timer = 0;
            s_rled_state = !s_rled_state;
            HAL_GPIO_WritePin(RLED_PORT, RLED_PIN, s_rled_state ? GPIO_PIN_SET : GPIO_PIN_RESET);
        }
    }

    /* Cập nhật BLED */
    if (s_bled_mode == LED_MODE_BLINK) {
        s_bled_timer += delta_time_ms;
        if (s_bled_timer >= LED_BLINK_PERIOD_MS) {
            s_bled_timer = 0;
            s_bled_state = !s_bled_state;
            HAL_GPIO_WritePin(BLED_PORT, BLED_PIN, s_bled_state ? GPIO_PIN_SET : GPIO_PIN_RESET);
        }
    }

    /* Cập nhật Buzzer */
    if (s_buzzer_timer > 0) {
        if (s_buzzer_timer <= delta_time_ms) {
            s_buzzer_timer = 0;
            HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);
        } else {
            s_buzzer_timer -= delta_time_ms;
        }
    }
}
