#include "pwm.h"

static TIM_HandleTypeDef htim2_pwm;

/**
 * @brief 初始化 TIM2 通道1 为 PWM 输出模式 (PA0)
 * @note  PWM 频率 = 72MHz / (Prescaler+1) / (Period+1) = 72e6 / 72 / 1000 = 1kHz
 *        占空比范围：0 ~ Period (1000-1=999)，但为了方便，接口使用 0~1000 对应 0%~100%
 */
void PWM_Init(void) {
    // 1. 使能定时器时钟和 GPIO 时钟
    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // 2. 配置 GPIO (PA0) 为复用推挽输出
    GPIO_InitTypeDef gpio_init = {0};
    gpio_init.Pin = GPIO_PIN_0;
    gpio_init.Mode = GPIO_MODE_AF_PP;
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &gpio_init);

    // 3. 配置定时器时基
    htim2_pwm.Instance = TIM2;
    htim2_pwm.Init.Prescaler = 72 - 1;          // 72MHz / 72 = 1MHz 计数时钟
    htim2_pwm.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2_pwm.Init.Period = 1000 - 1;           // 1MHz / 1000 = 1kHz
    htim2_pwm.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_PWM_Init(&htim2_pwm);

    // 4. 配置 PWM 通道 (Channel 1)
    TIM_OC_InitTypeDef oc_init = {0};
    oc_init.OCMode = TIM_OCMODE_PWM1;
    oc_init.Pulse = 0;                          // 初始占空比 0
    oc_init.OCPolarity = TIM_OCPOLARITY_HIGH;
    oc_init.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim2_pwm, &oc_init, TIM_CHANNEL_1);

    // 5. 启动 PWM 输出
    HAL_TIM_PWM_Start(&htim2_pwm, TIM_CHANNEL_1);
}

/**
 * @brief 设置 PWM 占空比
 * @param duty 占空比值，范围 0 ~ 1000 (对应 0% ~ 100%)
 */
void PWM_SetDuty(uint16_t duty) {
    if (duty > 1000) duty = 1000;
    // 由于 Period = 999，因此 duty 最大为 999 才是 100%，这里做个映射：duty=1000 => 999
    uint32_t compare = (duty == 1000) ? 999 : duty;
    __HAL_TIM_SET_COMPARE(&htim2_pwm, TIM_CHANNEL_1, compare);
}