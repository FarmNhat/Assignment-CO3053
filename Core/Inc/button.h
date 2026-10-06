/**
 ******************************************************************************
 * @file    button.h
 * @brief   Module xử lý nút bấm (Debounce, Edge Detection non-blocking).
 ******************************************************************************
 */

#ifndef __BUTTON_H
#define __BUTTON_H

#include "main.h"

typedef enum {
    BTN_ID_STOP = 0,
    BTN_ID_RUN,
    BTN_ID_PAUSE,
    BTN_ID_COIN10,
    BTN_ID_COIN20,
    BTN_ID_COIN50,
    BTN_ID_ERROR,
    BTN_ID_COUNT
} ButtonId_t;

void Button_Init(void);
void Button_Update(uint32_t delta_time_ms);
bool Button_WasPressed(ButtonId_t id);

#endif /* __BUTTON_H */
