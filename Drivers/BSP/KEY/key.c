#include "key.h"

// 按键状态变量（用于消抖）
static uint8_t up_last_state = 1;      // 上一次UP引脚电平（1=未按下，0=按下）
static uint8_t down_last_state = 1;    // 上一次DOWN引脚电平
static uint32_t up_last_time = 0;      // UP上次变化时间
static uint32_t down_last_time = 0;    // DOWN上次变化时间

#define DEBOUNCE_MS    50              // 消抖时间（毫秒）

/**
 * @brief 初始化按键GPIO（内部上拉输入）
 */
void KEY_Init(void) {
    GPIO_InitTypeDef gpio_init = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();

    gpio_init.Pin = KEY_UP_PIN | KEY_DOWN_PIN;
    gpio_init.Mode = GPIO_MODE_INPUT;
    gpio_init.Pull = GPIO_PULLUP;       // 内部上拉
    gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &gpio_init);
}

/**
 * @brief 扫描按键（非阻塞，带消抖）
 * @return KEY_NONE, KEY_UP, KEY_DOWN
 * 
 * 注意：需要在主循环中定期调用（如每10~50ms），或者每次调用时读取当前电平。
 * 本函数仅在检测到按键按下（下降沿）时返回一次按键值，自动清除标志。
 */
uint8_t KEY_Scan(void) {
    uint8_t up_reading = HAL_GPIO_ReadPin(KEY_UP_PORT, KEY_UP_PIN);
    uint8_t down_reading = HAL_GPIO_ReadPin(KEY_DOWN_PORT, KEY_DOWN_PIN);
    uint8_t result = KEY_NONE;

    // ----- UP 键处理 -----
    if (up_reading != up_last_state) {
        up_last_time = HAL_GetTick();
    }
    if ((HAL_GetTick() - up_last_time) >= DEBOUNCE_MS) {
        if (up_reading == 0 && up_last_state == 0) {  // 下降沿：按下
            result = KEY_UP;
        }
    }
    up_last_state = up_reading;

    // ----- DOWN 键处理 -----
    if (down_reading != down_last_state) {
        down_last_time = HAL_GetTick();
    }
    if ((HAL_GetTick() - down_last_time) >= DEBOUNCE_MS) {
        if (down_reading == 0 && down_last_state == 0) {
            result = KEY_DOWN;
        }
    }
    down_last_state = down_reading;

    return result;
}