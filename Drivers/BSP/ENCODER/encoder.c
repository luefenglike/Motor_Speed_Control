#include "encoder.h"

// 编码器参数（请根据实际电机修改）
#define ENCODER_PPR       6      // 编码器线数（每转脉冲数）
#define REDUCTION_RATIO   42      // 减速比（电机轴转一圈，编码器转 REDUCTION_RATIO 圈）
#define SAMPLING_TIME_MS  10      // 测速采样周期 (ms)，必须与调用频率一致

// 总脉冲数（四倍频后）
#define TOTAL_PPR         (ENCODER_PPR * REDUCTION_RATIO * 4)

static TIM_HandleTypeDef htim3_encoder;
static int16_t last_count = 0;      // 上次采样计数值

/**
 * @brief 初始化 TIM3 为编码器模式 (PA6: CH1, PA7: CH2)
 * @note  计数值范围 0~65535，支持正反转自动加减
 */
void Encoder_Init(void) {
    // 1. 使能定时器时钟
    __HAL_RCC_TIM3_CLK_ENABLE();
    // 2. 配置 GPIO (PA6, PA7) 为复用推挽输入（实际上用于捕获）
    __HAL_RCC_GPIOA_CLK_ENABLE();
	
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;          // 内部上拉，增强抗干扰
		gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &gpio);

    // 3. 配置 TIM3 时基
    htim3_encoder.Instance = TIM3;
    htim3_encoder.Init.Prescaler = 0;
    htim3_encoder.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3_encoder.Init.Period = 0xFFFF;        // 最大计数值
    htim3_encoder.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Encoder_Init(&htim3_encoder, NULL);

    // 4. 配置编码器模式 (TI1 和 TI2 双沿计数，即四倍频)
    TIM_Encoder_InitTypeDef enc_cfg = {0};
    enc_cfg.EncoderMode = TIM_ENCODERMODE_TI12;     // 计数上升沿和下降沿
    enc_cfg.IC1Polarity = TIM_ICPOLARITY_RISING;
    enc_cfg.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    enc_cfg.IC1Prescaler = TIM_ICPSC_DIV1;
    enc_cfg.IC1Filter = 0;
    enc_cfg.IC2Polarity = TIM_ICPOLARITY_RISING;
    enc_cfg.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    enc_cfg.IC2Prescaler = TIM_ICPSC_DIV1;
    enc_cfg.IC2Filter = 0;
    HAL_TIM_Encoder_Init(&htim3_encoder, &enc_cfg);

    // 5. 启动编码器计数
    HAL_TIM_Encoder_Start(&htim3_encoder, TIM_CHANNEL_ALL);
    Encoder_ClearCount();
    last_count = 0;
}

/**
 * @brief 获取当前编码器计数值（有符号）
 * @return 计数值，范围 -32768 ~ 32767（溢出自动回绕，差值计算正确）
 */
int16_t Encoder_GetCount(void) {
    return (int16_t)__HAL_TIM_GET_COUNTER(&htim3_encoder);
}

/**
 * @brief 清零编码器计数值
 */
void Encoder_ClearCount(void) {
    __HAL_TIM_SET_COUNTER(&htim3_encoder, 0);
    last_count = 0;
}

/**
 * @brief 获取电机实际转速 (RPM)
 * @note  必须在固定时间间隔（如 10ms）调用，否则测速不准
 * @return 转速，正值表示正转，负值表示反转
 */
float Encoder_GetSpeed_RPM(void) {
    int16_t current = Encoder_GetCount();
    int16_t delta = current - last_count;       // 采样周期内的脉冲变化
    last_count = current;

    // 转速计算公式：RPM = (Δ计数 × 60) / (总脉冲数每转 × 采样时间(秒))
    // 采样时间为 SAMPLING_TIME_MS / 1000
    float rpm = (float)delta * 60.0f / (TOTAL_PPR * (SAMPLING_TIME_MS / 1000.0f));
    return rpm;
}