/**
 ******************************************************************************
 * @file    fsm.c
 * @brief   Hiện thực Máy trạng thái máy giặt (Switch-case pattern từ Lecture 4).
 ******************************************************************************
 */

#include "fsm.h"
#include <stdio.h>

static WashingMachine_t s_machine;
static uint32_t s_one_second_accumulator = 0;

static void FSM_ApplyOutputs(MachineState_t state);

void FSM_Init(void) {
    s_machine.current_state = STATE_STANDBY;
    s_machine.state_before_stop_confirm = STATE_STANDBY;
    s_machine.money_cents = 0;
    s_machine.wash_timer_sec = 0;
    s_machine.stop_confirm_timer_ms = 0;
    s_machine.has_error = false;
    s_one_second_accumulator = 0;

    FSM_ApplyOutputs(s_machine.current_state);
    printf("\r\n==========================================\r\n");
    printf("[FSM INIT] Washing Machine Ready in STANDBY.\r\n");
    printf("Please insert at least 50 cents (10c, 20c, 50c).\r\n");
    printf("==========================================\r\n");
}

const WashingMachine_t* FSM_GetContext(void) {
    return &s_machine;
}

const char* FSM_GetStateName(MachineState_t state) {
    switch (state) {
        case STATE_STANDBY:      return "STANDBY";
        case STATE_READY:        return "READY";
        case STATE_RUNNING:      return "RUNNING";
        case STATE_PAUSED:       return "PAUSED";
        case STATE_STOP_CONFIRM: return "STOP_CONFIRM (Wait 2nd STOP)";
        case STATE_ERROR:        return "ERROR";
        default:                 return "UNKNOWN";
    }
}

/**
 * @brief Áp dụng các ngõ ra theo mô hình Moore (Outputs chỉ phụ thuộc State)
 */
static void FSM_ApplyOutputs(MachineState_t state) {
    switch (state) {
        case STATE_STANDBY:
            LED_SetRedMode(LED_MODE_ON);     /* RLED ON: sẵn sàng phục vụ */
            LED_SetBlueMode(LED_MODE_OFF);
            Motor_SetState(false);
            break;

        case STATE_READY:
            LED_SetRedMode(LED_MODE_OFF);
            LED_SetBlueMode(LED_MODE_ON);    /* BLED ON: đủ tiền, sẵn sàng chạy */
            Motor_SetState(false);
            break;

        case STATE_RUNNING:
            LED_SetRedMode(LED_MODE_OFF);
            LED_SetBlueMode(LED_MODE_BLINK); /* BLED BLINK: đang giặt */
            Motor_SetState(true);            /* Động cơ quay */
            break;

        case STATE_PAUSED:
            LED_SetRedMode(LED_MODE_OFF);
            LED_SetBlueMode(LED_MODE_OFF);   /* Tạm dừng: BLED tắt, động cơ dừng */
            Motor_SetState(false);
            break;

        case STATE_STOP_CONFIRM:
            /* Giữ nguyên hoặc tắt motor trong khi chờ xác nhận dừng */
            Motor_SetState(false);
            break;

        case STATE_ERROR:
            LED_SetRedMode(LED_MODE_BLINK);  /* RLED BLINK: máy có lỗi */
            LED_SetBlueMode(LED_MODE_OFF);
            Motor_SetState(false);
            break;
    }
}

static void ChangeState(MachineState_t new_state) {
    if (s_machine.current_state != new_state) {
        printf("[STATE TRANSITION] %s -> %s\r\n", 
               FSM_GetStateName(s_machine.current_state), 
               FSM_GetStateName(new_state));
        s_machine.current_state = new_state;
        FSM_ApplyOutputs(new_state);
    }
}

void FSM_DispatchEvent(MachineEvent_t event) {
    if (event == EVT_NONE) return;

    /* Xử lý sự kiện khẩn cấp: Lỗi hệ thống */
    if (event == EVT_ERROR_TRIGGER && s_machine.current_state != STATE_ERROR) {
        s_machine.has_error = true;
        printf("\r\n[ALERT] Error detected! System entering ERROR state.\r\n");
        ChangeState(STATE_ERROR);
        return;
    }

    switch (s_machine.current_state) {
        /* -----------------------------------------------------------
         * STATE: STANDBY (Chờ nạp đủ 50 cents)
         * ----------------------------------------------------------- */
        case STATE_STANDBY:
            if (event == EVT_COIN_10 || event == EVT_COIN_20 || event == EVT_COIN_50) {
                uint16_t add = (event == EVT_COIN_10) ? 10 : ((event == EVT_COIN_20) ? 20 : 50);
                s_machine.money_cents += add;
                printf("[COIN INSERTED] +%d cents. Total: %d cents\r\n", add, s_machine.money_cents);

                /* Guard condition: money >= 50 */
                if (s_machine.money_cents >= MIN_EXECUTE_COINS) {
                    printf("[INFO] Enough money! Ready to execute. Press RUN to start.\r\n");
                    ChangeState(STATE_READY);
                }
            }
            break;

        /* -----------------------------------------------------------
         * STATE: READY (Đã đủ >= 50 cents, chờ bấm RUN)
         * ----------------------------------------------------------- */
        case STATE_READY:
            if (event == EVT_COIN_10 || event == EVT_COIN_20 || event == EVT_COIN_50) {
                uint16_t add = (event == EVT_COIN_10) ? 10 : ((event == EVT_COIN_20) ? 20 : 50);
                s_machine.money_cents += add;
                printf("[COIN INSERTED] +%d cents. Total: %d cents (No refunds)\r\n", add, s_machine.money_cents);
            }
            else if (event == EVT_BTN_RUN) {
                /* Đề bài: "Then, a 30-mins timer will be activated and the money will be clear without returning the redundancies" */
                printf("[ACTION] RUN pressed! Starting wash cycle (%d seconds). Money cleared: %d cents.\r\n", 
                       WASH_DURATION_SECONDS, s_machine.money_cents);
                s_machine.money_cents = 0;
                s_machine.wash_timer_sec = WASH_DURATION_SECONDS;
                ChangeState(STATE_RUNNING);
            }
            else if (event == EVT_BTN_STOP) {
                /* Người dùng hủy trước khi giặt */
                printf("[ACTION] STOP pressed in READY. Resetting to STANDBY.\r\n");
                s_machine.money_cents = 0;
                ChangeState(STATE_STANDBY);
            }
            break;

        /* -----------------------------------------------------------
         * STATE: RUNNING (Máy đang giặt, motor chạy, timer đếm lùi)
         * ----------------------------------------------------------- */
        case STATE_RUNNING:
            if (event == EVT_BTN_PAUSE) {
                printf("[ACTION] PAUSE pressed! Washing paused. Timer is STILL counting down.\r\n");
                ChangeState(STATE_PAUSED);
            }
            else if (event == EVT_BTN_STOP) {
                /* Bấm STOP lần 1: Chuyển sang chờ xác nhận lần 2 */
                printf("[WARNING] STOP pressed 1st time! Press STOP again within %d ms to Force Stop.\r\n", 
                       STOP_DOUBLE_CLICK_TIMEOUT);
                s_machine.state_before_stop_confirm = STATE_RUNNING;
                s_machine.stop_confirm_timer_ms = STOP_DOUBLE_CLICK_TIMEOUT;
                ChangeState(STATE_STOP_CONFIRM);
            }
            else if (event == EVT_TIMER_1S_TICK) {
                if (s_machine.wash_timer_sec > 0) {
                    s_machine.wash_timer_sec--;
                    printf("[WASHING] Time remaining: %02d:%02d\r\n", 
                           s_machine.wash_timer_sec / 60, s_machine.wash_timer_sec % 60);

                    if (s_machine.wash_timer_sec == 0) {
                        /* Đề bài: "The executing machine will be automatically terminated after 30 minutes" */
                        printf("\r\n[COMPLETED] Washing cycle completed! Buzzer on. Terminating.\r\n");
                        Buzzer_Beep(1000);
                        ChangeState(STATE_STANDBY);
                    }
                }
            }
            break;

        /* -----------------------------------------------------------
         * STATE: PAUSED (Tạm dừng nhưng timer vẫn đếm lùi)
         * ----------------------------------------------------------- */
        case STATE_PAUSED:
            if (event == EVT_BTN_RUN) {
                /* Đề bài: "The machine will re-execute when the RUN button is pressed again" */
                printf("[ACTION] RUN pressed! Re-executing wash cycle.\r\n");
                ChangeState(STATE_RUNNING);
            }
            else if (event == EVT_BTN_STOP) {
                printf("[WARNING] STOP pressed 1st time in PAUSE! Press STOP again within %d ms to Force Stop.\r\n", 
                       STOP_DOUBLE_CLICK_TIMEOUT);
                s_machine.state_before_stop_confirm = STATE_PAUSED;
                s_machine.stop_confirm_timer_ms = STOP_DOUBLE_CLICK_TIMEOUT;
                ChangeState(STATE_STOP_CONFIRM);
            }
            else if (event == EVT_TIMER_1S_TICK) {
                /* Đề bài: "paused as the PAUSE button is pressed but the timer is still counting down" */
                if (s_machine.wash_timer_sec > 0) {
                    s_machine.wash_timer_sec--;
                    printf("[PAUSED] Time remaining: %02d:%02d (Timer still running!)\r\n", 
                           s_machine.wash_timer_sec / 60, s_machine.wash_timer_sec % 60);

                    if (s_machine.wash_timer_sec == 0) {
                        printf("\r\n[TIMEOUT] Timer reached 0 while in PAUSED! Terminating.\r\n");
                        Buzzer_Beep(1000);
                        ChangeState(STATE_STANDBY);
                    }
                }
            }
            break;

        /* -----------------------------------------------------------
         * STATE: STOP_CONFIRM (Chờ lần bấm STOP thứ 2 để force stop)
         * ----------------------------------------------------------- */
        case STATE_STOP_CONFIRM:
            if (event == EVT_BTN_STOP) {
                /* Đề bài: "or when the STOP button is pressed twice (force to stop)" */
                printf("\r\n[FORCE STOP] STOP pressed 2nd time! Forcing machine to terminate!\r\n");
                s_machine.wash_timer_sec = 0;
                s_machine.stop_confirm_timer_ms = 0;
                Buzzer_Beep(500);
                ChangeState(STATE_STANDBY);
            }
            else if (event == EVT_TIMER_1S_TICK) {
                /* Vẫn đếm lùi thời gian giặt nếu chưa hết */
                if (s_machine.wash_timer_sec > 0) {
                    s_machine.wash_timer_sec--;
                }
            }
            break;

        /* -----------------------------------------------------------
         * STATE: ERROR (Có lỗi, RLED nhấp nháy, chờ reset)
         * ----------------------------------------------------------- */
        case STATE_ERROR:
            if (event == EVT_ERROR_TRIGGER || event == EVT_BTN_STOP) {
                printf("[ACTION] Error cleared / Reset pressed. Returning to STANDBY.\r\n");
                s_machine.has_error = false;
                s_machine.wash_timer_sec = 0;
                ChangeState(STATE_STANDBY);
            }
            break;
    }
}

void FSM_Update(uint32_t delta_time_ms) {
    /* Cập nhật nhấp nháy LED & còi Buzzer */
    LED_Update(delta_time_ms);

    /* Cập nhật đếm ngược xác nhận double STOP */
    if (s_machine.current_state == STATE_STOP_CONFIRM) {
        if (s_machine.stop_confirm_timer_ms <= delta_time_ms) {
            s_machine.stop_confirm_timer_ms = 0;
            printf("[TIMEOUT] No 2nd STOP pressed. Resuming %s.\r\n", 
                   FSM_GetStateName(s_machine.state_before_stop_confirm));
            ChangeState(s_machine.state_before_stop_confirm);
        } else {
            s_machine.stop_confirm_timer_ms -= delta_time_ms;
        }
    }

    /* Tạo nhịp đếm 1 giây (1000ms) độc lập, không block CPU */
    s_one_second_accumulator += delta_time_ms;
    if (s_one_second_accumulator >= 1000) {
        s_one_second_accumulator = 0;
        FSM_DispatchEvent(EVT_TIMER_1S_TICK);
    }
}
