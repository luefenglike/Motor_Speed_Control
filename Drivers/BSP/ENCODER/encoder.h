#ifndef __ENCODER_H
#define __ENCODER_H

#include "stm32f1xx_hal.h"

// 初始化编码器 (TIM3, PA6, PA7)
void Encoder_Init(void);

// 获取当前计数值 (有符号，支持正反转)
int16_t Encoder_GetCount(void);

// 清零计数值
void Encoder_ClearCount(void);

// 获取电机实际转速 (RPM，可正可负)
float Encoder_GetSpeed_RPM(void);

#endif