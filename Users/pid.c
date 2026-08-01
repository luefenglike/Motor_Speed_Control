#include "pid.h"
#include <math.h>

/**
 * @brief 初始化 PID 结构体
 * @param pid      PID 结构体指针
 * @param kp       比例系数
 * @param ki       积分系数
 * @param kd       微分系数
 * @param out_max  输出最大值
 * @param out_min  输出最小值
 */
void PID_Init(PID_TypeDef *pid, float kp, float ki, float kd, float out_max, float out_min) {
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
    pid->out_max = out_max;
    pid->out_min = out_min;

    pid->target = 0.0f;
    pid->feedback = 0.0f;
    pid->output = 0.0f;

    pid->err = 0.0f;
    pid->err_last = 0.0f;
    pid->err_pre_last = 0.0f;

    // 默认阈值（可根据实际电机调整）
    pid->integral_sep_th = 30.0f;
    pid->dead_zone_th = 25.0f;

    // 默认使能功能
    pid->enable_integral_sep = 1;
    pid->enable_dead_zone = 1;
}

/**
 * @brief 设置 PID 目标值
 * @param pid    PID 结构体指针
 * @param target 目标值
 */
void PID_SetTarget(PID_TypeDef *pid, float target) {
    pid->target = target;
}

/**
 * @brief 增量式 PID 计算
 * @param pid      PID 结构体指针
 * @param feedback 当前反馈值（实际转速）
 * @return         输出控制量（PWM 占空比等）
 *
 * 公式：Δu(k) = Kp * (e(k)-e(k-1)) + Ki * e(k) + Kd * (e(k)-2e(k-1)+e(k-2))
 *       u(k) = u(k-1) + Δu(k)
 */
float PID_Compute(PID_TypeDef *pid, float feedback) {
	
    pid->feedback = feedback;
    float err_new = pid->target - feedback;   // 当前误差

    // 积分分离：误差较小时，积分项不起作用
    float ki_effective = pid->Ki;
    if (pid->enable_integral_sep && fabs(err_new) < pid->integral_sep_th) {
        ki_effective = 0.0f;
    }

    // 增量式计算
    float delta = pid->Kp * (err_new - pid->err_last)
                + ki_effective * err_new
                + pid->Kd * (err_new - 2.0f * pid->err_last + pid->err_pre_last);

    pid->output += delta;

    // 死区钳位：输出很小时强制为 0，并清空历史误差避免退出死区时突变
    if (pid->enable_dead_zone && fabs(pid->output) < pid->dead_zone_th) {
        pid->output = 0.0f;
        pid->err = 0.0f;
        pid->err_last = 0.0f;
        pid->err_pre_last = 0.0f;
    }

    // 输出限幅
    if (pid->output > pid->out_max) pid->output = pid->out_max;
    if (pid->output < pid->out_min) pid->output = pid->out_min;

    // 更新误差历史
    pid->err_pre_last = pid->err_last;
    pid->err_last = err_new;
    pid->err = err_new;

    return pid->output;
}