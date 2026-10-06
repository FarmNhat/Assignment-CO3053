/**
 ******************************************************************************
 * @file    fsm.h
 * @brief   Định nghĩa Máy trạng thái (Finite State Machine) cho máy giặt.
 *          Tuân thủ nguyên tắc thiết kế FSM từ Lecture 4:
 *          - Extended State Machine (biến money, timer, stop_count)
 *          - Moore Machine cho ngõ ra (RLED, BLED, Motor)
 *          - Tránh missing transitions và unreachable states
 ******************************************************************************
 */

#ifndef __FSM_H
#define __FSM_H

#include "main.h"
#include "led.h"

/* Các trạng thái của máy giặt */
typedef enum {
    STATE_STANDBY = 0,      /* Chờ, RLED = ON, BLED = OFF */
    STATE_READY,            /* Đủ tiền >= 50c, BLED = ON, RLED = OFF */
    STATE_RUNNING,          /* Đang giặt, BLED = BLINK, Motor = ON, Timer đếm lùi */
    STATE_PAUSED,           /* Tạm dừng, Motor = OFF, Timer vẫn đếm lùi */
    STATE_STOP_CONFIRM,     /* Bấm STOP lần 1, chờ lần 2 (trong vòng 2s) */
    STATE_ERROR             /* Lỗi sự cố, RLED = BLINK, BLED = OFF, Motor = OFF */
} MachineState_t;

/* Các sự kiện kích hoạt (Events) */
typedef enum {
    EVT_NONE = 0,
    EVT_COIN_10,
    EVT_COIN_20,
    EVT_COIN_50,
    EVT_BTN_RUN,
    EVT_BTN_PAUSE,
    EVT_BTN_STOP,
    EVT_TIMER_1S_TICK,
    EVT_ERROR_TRIGGER
} MachineEvent_t;

/* Cấu trúc ngữ cảnh mở rộng (Extended Context) */
typedef struct {
    MachineState_t current_state;
    MachineState_t state_before_stop_confirm; /* Để khôi phục nếu timeout lần bấm STOP 2 */
    uint16_t money_cents;                     /* Số cent hiện tại */
    uint16_t wash_timer_sec;                  /* Thời gian giặt còn lại (giây) */
    uint16_t stop_confirm_timer_ms;           /* Đếm lùi thời gian chờ nút STOP lần 2 */
    bool has_error;                           /* Cờ ghi nhận lỗi */
} WashingMachine_t;

/* Khởi tạo FSM */
void FSM_Init(void);

/* Xử lý sự kiện đưa vào FSM */
void FSM_DispatchEvent(MachineEvent_t event);

/* Cập nhật định kỳ (gọi mỗi vòng lặp 10ms) */
void FSM_Update(uint32_t delta_time_ms);

/* Lấy thông tin trạng thái hiện tại (phục vụ hiển thị / UART) */
const WashingMachine_t* FSM_GetContext(void);
const char* FSM_GetStateName(MachineState_t state);

#endif /* __FSM_H */
