/**
 ******************************************************************************
 * @file    button.c
 * @brief   Hiện thực đọc nút bấm chống dội (Debounce 20ms) và phát hiện cạnh xuống.
 ******************************************************************************
 */

#include "button.h"

typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
    bool current_state;      /* true nếu đang nhấn (mức 0) */
    bool last_raw_state;
    uint16_t debounce_timer; /* ms */
    bool was_pressed_flag;   /* Event cờ nhấn 1 lần */
} ButtonDev_t;

static ButtonDev_t s_buttons[BTN_ID_COUNT] = {
    { BTN_STOP_PORT,   BTN_STOP_PIN,   false, false, 0, false },
    { BTN_RUN_PORT,    BTN_RUN_PIN,    false, false, 0, false },
    { BTN_PAUSE_PORT,  BTN_PAUSE_PIN,  false, false, 0, false },
    { BTN_COIN10_PORT, BTN_COIN10_PIN, false, false, 0, false },
    { BTN_COIN20_PORT, BTN_COIN20_PIN, false, false, 0, false },
    { BTN_COIN50_PORT, BTN_COIN50_PIN, false, false, 0, false },
    { BTN_ERR_PORT,    BTN_ERR_PIN,    false, false, 0, false }
};

#define DEBOUNCE_TIME_MS 20

void Button_Init(void) {
    for (int i = 0; i < BTN_ID_COUNT; ++i) {
        s_buttons[i].current_state = false;
        s_buttons[i].last_raw_state = false;
        s_buttons[i].debounce_timer = 0;
        s_buttons[i].was_pressed_flag = false;
    }
}

void Button_Update(uint32_t delta_time_ms) {
    for (int i = 0; i < BTN_ID_COUNT; ++i) {
        /* Nút bấm kéo lên PULL-UP: nhấn = mức 0 (RESET), thả = mức 1 (SET) */
        bool raw_pressed = (HAL_GPIO_ReadPin(s_buttons[i].port, s_buttons[i].pin) == GPIO_PIN_RESET);

        if (raw_pressed != s_buttons[i].last_raw_state) {
            s_buttons[i].last_raw_state = raw_pressed;
            s_buttons[i].debounce_timer = 0;
        } else {
            if (s_buttons[i].debounce_timer < DEBOUNCE_TIME_MS) {
                s_buttons[i].debounce_timer += delta_time_ms;
                if (s_buttons[i].debounce_timer >= DEBOUNCE_TIME_MS) {
                    /* Trạng thái đã ổn định */
                    if (raw_pressed && !s_buttons[i].current_state) {
                        /* Cạnh xuống: phát hiện nhấn phím 1 lần */
                        s_buttons[i].was_pressed_flag = true;
                    }
                    s_buttons[i].current_state = raw_pressed;
                }
            }
        }
    }
}

bool Button_WasPressed(ButtonId_t id) {
    if (id >= BTN_ID_COUNT) return false;
    if (s_buttons[id].was_pressed_flag) {
        s_buttons[id].was_pressed_flag = false; /* Xóa cờ sau khi đọc */
        return true;
    }
    return false;
}
