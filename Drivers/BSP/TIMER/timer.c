#include "timer.h"

// 定时器句柄
static TIM_HandleTypeDef htim4;

// 10ms 中断标志（外部使用）
volatile uint8_t timer10ms_flag = 0;

/**
 * @brief 初始化 TIM4 为 10ms 周期中断
 * @note  时钟配置为 72MHz，APB1 总线频率 36MHz，定时器时钟 = 72MHz
 *        预分频器 (Prescaler) = 7200-1 → 72MHz / 7200 = 10kHz 计数时钟
 *        周期 (Period) = 100-1 → 10kHz / 100 = 100Hz → 周期 10ms
 */
void Timer_Init(void) {
    // 使能 TIM4 时钟
    __HAL_RCC_TIM4_CLK_ENABLE();

    htim4.Instance = TIM4;
    htim4.Init.Prescaler = 7200 - 1;        // 10kHz 计数频率
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 100 - 1;            // 计数值 100 → 10ms
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_Base_Init(&htim4);
}

/**
 * @brief 启动 TIM4 中断
 */
void Timer_Start_IT(void) {
    HAL_TIM_Base_Start_IT(&htim4);
    HAL_NVIC_SetPriority(TIM4_IRQn, 1, 0);   // 抢占优先级 1
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
}

/**
 * @brief TIM4 中断服务函数（需放在 stm32f1xx_it.c 中，或直接放在此文件）
 * @note  如果将此函数放在 timer.c 中，需要确保中断向量表正确链接。
 *        推荐放在 stm32f1xx_it.c 中，并在此文件中声明 extern 标志。
 */
void TIM4_IRQHandler(void) {
    if (__HAL_TIM_GET_FLAG(&htim4, TIM_FLAG_UPDATE) != RESET) {
        __HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_UPDATE);
        timer10ms_flag = 1;   // 置位标志，供主循环查询
    }
}