#ifndef __KEY_H
#define __KEY_H

#include "stm32f1xx_hal.h"

// 引脚定义
#define KEY_UP_PIN      GPIO_PIN_4
#define KEY_UP_PORT     GPIOA
#define KEY_DOWN_PIN    GPIO_PIN_5
#define KEY_DOWN_PORT   GPIOA

// 按键返回值
#define KEY_NONE        0   // 无按键
#define KEY_UP          1   // UP键按下
#define KEY_DOWN        2   // DOWN键按下

// 函数声明
void KEY_Init(void);                // 初始化按键GPIO
uint8_t KEY_Scan(void);             // 扫描按键（非阻塞，带消抖）

#endif