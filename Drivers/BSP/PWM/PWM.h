#ifndef __PWM_H
#define __PWM_H

#include "stm32f1xx_hal.h"

// 初始化 PWM (TIM2_CH1, PA0)
void PWM_Init(void);

// 设置占空比 (0 ~ 1000, 对应 0% ~ 100%)
void PWM_SetDuty(uint16_t duty);

#endif