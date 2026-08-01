/**
 ******************************************************************************
 * @file     main.c
 * @author   正点原子团队(ALIENTEK)
 * @version  V1.0
 * @date     2024-11-01
 * @brief    OLED 实验
 * @license  Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ******************************************************************************
 * @attention
 * 
 * 实验平台:正点原子 M48Z-M3最小系统板STM32F103版
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 ******************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"

#include "./BSP/KEY/key.h"
#include "./BSP/PWM/PWM.h"
#include "./BSP/TIMER/Timer.h"
#include "./BSP/ENCODER/Encoder.h"
#include "./BSP/ATK_OLED/atk_oled.h"

#include "pid.h"
PID_TypeDef speed_pid;


float speed_sv = 0;
float speed_cv = 0;
void OLED_show(float sv, float cv, float pwm)
{
    atk_oled_show_string(0, 0, "********", 24);
    atk_oled_show_string(0, 24, "PID TEST", 16);
		atk_oled_show_string(0, 40, "SV:", 12);
    atk_oled_show_string(64, 40, "CV:", 12);
		atk_oled_show_string(0, 52, "PWM:", 12);
		char tmp[6];
	  sprintf(tmp,"%3.1f", sv);
		atk_oled_show_string(24, 40, tmp, 12);
		sprintf(tmp,"%3.1f", cv);
		atk_oled_show_string(88, 40, tmp, 12);
		sprintf(tmp,"%3.1f", pwm);
		atk_oled_show_string(24, 52, tmp, 12);
		atk_oled_refresh_gram();                /* 更新显示到OLED */
}


extern volatile uint8_t timer10ms_flag;  // 或在 timer.h 中已声明

int main(void)
{	
    HAL_Init();                             /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9);     /* 设置时钟, 72Mhz */
    delay_init(72);                         /* 延时初始化 */
    //usart_init(115200);                     /* 串口初始化为115200 */
    led_init();                             /* 初始化LED */
    KEY_Init();          										// 初始化按键
		atk_oled_init();                            /* 初始化OLED */
		Timer_Init();            // 初始化 TIM4
		Encoder_Init();
		PWM_Init();             // 初始化 PWM，PA0 输出 1kHz 方波
	

		OLED_show(speed_sv, speed_cv, 0.0);
		PWM_SetDuty(speed_sv);

    Timer_Start_IT();        // 启动中断
	
		uint8_t t_cnt = 0;

    // 初始化 PID，输出范围 0~1000（对应 PWM 占空比）
	
	                       //kp  //ki  //kd   //输出值限幅
    PID_Init(&speed_pid, 0.0f, 0.0f, 0.0f, 1000.0f, 0.0f);

    // 可选：修改积分分离阈值和死区阈值
    speed_pid.integral_sep_th = 20.0f;   // 误差小于 20 RPM 时停止积分
    speed_pid.dead_zone_th = 30.0f;      // 输出小于 30 时强制为 0
		
    PID_SetTarget(&speed_pid, speed_sv);   // 目标转速: speed_sv
		
		
		while (1) {
				
				if (timer10ms_flag) {
            timer10ms_flag = 0;
						t_cnt++;
            // 每 10ms 执行一次的任务（如 PID 计算、编码器读取）
					
						//获取当前转速
						speed_cv = Encoder_GetSpeed_RPM();
						//计算控制输出PWM
						float pwm_out = PID_Compute(&speed_pid, speed_cv);
						//应用控制输出PWM
						PWM_SetDuty((int16_t)pwm_out);
					
						uint8_t key = KEY_Scan();
						if (key == KEY_UP) {
								// UP 键按下：增加目标值
								speed_sv += 10;
								PID_SetTarget(&speed_pid, speed_sv); 
								OLED_show(speed_sv, speed_cv, pwm_out);
								continue;
						} else if (key == KEY_DOWN) {
								// DOWN 键按下：减少目标值
								speed_sv -= 10;
								PID_SetTarget(&speed_pid, speed_sv); 
								OLED_show(speed_sv, speed_cv, pwm_out);
								continue;
						}
						if(t_cnt==10){
								t_cnt = 0;
								OLED_show(speed_sv, speed_cv, pwm_out);
						}
        }
				
    }
	
}

