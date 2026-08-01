#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f1xx_hal.h"

// 初始化 TIM4 为基础定时器 (10ms 周期)
void Timer_Init(void);

// 启动 TIM4 中断
void Timer_Start_IT(void);

// 获取 10ms 标志（在中断中置1，主循环中清零）
extern volatile uint8_t timer10ms_flag;

#endif