/**
 ******************************************************************************
 * @file    startup_stm32f103c8tx.c
 * @brief   Bảng vector ngắt và hàm khởi tạo Reset_Handler cho STM32F103C8T6.
 ******************************************************************************
 */

#include <stdint.h>

extern uint32_t _estack;
extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

extern int main(void);
extern void HAL_IncTick(void);

void Reset_Handler(void);
void Default_Handler(void);
void SysTick_Handler(void);

/* Định nghĩa bảng vector ngắt Cortex-M3 */
__attribute__((section(".isr_vector"), used))
void (* const g_pfnVectors[])(void) = {
    (void (*)(void))((uint32_t)&_estack), /* Initial Stack Pointer */
    Reset_Handler,                          /* Reset Handler */
    Default_Handler,                        /* NMI Handler */
    Default_Handler,                        /* HardFault Handler */
    Default_Handler,                        /* MPU Fault Handler */
    Default_Handler,                        /* Bus Fault Handler */
    Default_Handler,                        /* Usage Fault Handler */
    0, 0, 0, 0,                             /* Reserved */
    Default_Handler,                        /* SVCall Handler */
    Default_Handler,                        /* Debug Monitor Handler */
    0,                                      /* Reserved */
    Default_Handler,                        /* PendSV Handler */
    SysTick_Handler,                        /* SysTick Handler */
};

void Reset_Handler(void) {
    /* Copy dữ liệu .data từ Flash sang RAM */
    uint32_t *pSrc = &_sidata;
    uint32_t *pDst = &_sdata;
    while (pDst < &_edata) {
        *pDst++ = *pSrc++;
    }

    /* Xóa vùng .bss trong RAM về 0 */
    pDst = &_sbss;
    while (pDst < &_ebss) {
        *pDst++ = 0;
    }

    /* Nhảy vào hàm main */
    main();

    /* Vòng lặp vô tận đề phòng main return */
    while (1);
}

void SysTick_Handler(void) {
    HAL_IncTick();
}

void Default_Handler(void) {
    while (1);
}
