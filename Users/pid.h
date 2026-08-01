#ifndef __PID_H
#define __PID_H

#include "stm32f1xx_hal.h"

typedef struct {
    // PID 系数
    float Kp;
    float Ki;
    float Kd;

    // 目标值与反馈值
    float target;
    float feedback;
    float output;          // 当前输出

    // 输出限幅
    float out_max;
    float out_min;

    // 误差历史（用于增量式）
    float err;
    float err_last;
    float err_pre_last;

    // 积分分离阈值（误差绝对值小于此值时，积分项不累加）
    float integral_sep_th;

    // 死区钳位阈值（输出绝对值小于此值时，强制输出 0 并清空历史）
    float dead_zone_th;

    // 功能使能标志
    uint8_t enable_integral_sep;
    uint8_t enable_dead_zone;

} PID_TypeDef;

// 初始化 PID (增量式)
void PID_Init(PID_TypeDef *pid, float kp, float ki, float kd, float out_max, float out_min);

// 设置目标值
void PID_SetTarget(PID_TypeDef *pid, float target);

// 增量式 PID 计算
float PID_Compute(PID_TypeDef *pid, float feedback);

#endif