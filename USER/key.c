/********************************************************************
 *@file        key.c
 *@author      Woody      QQ：2490006131
 *@version     V1.0
 *@date        2016-10-14
 *@brief       KEY初始化以及扫描函数
 *******************************************************************/
#include "key.h"
#include "led.h"
#include "tim1.h"
#include "delay.h"
#include "string.h"

u8 KEY_STUDY=0;  //进入学习模式
//u8 LED_Low=0;
u8 key_led_flag=0;
u16 Bijiao_buffer[280];

/*******************************************************************************
 * 函数名：key_init
 * 描述  ：配置key的初始化
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 ******************************************************************************/
void KEY_Init(void)
{       
    GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_IN_PU_NO_IT);         
}
/*******************************************************************************
 * 函数名：key_study
 * 描述  ：配置key扫描函数进入学习模式
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 ******************************************************************************/
void key_study(void)
{
  if(GPIO_ReadInputPin(GPIOD, GPIO_PIN_4) == 0)
   {
     delay_ms(20);
     if(GPIO_ReadInputPin(GPIOD, GPIO_PIN_4) == 0)
     {
       KEY_STUDY=1; // 进入学习模式
       IR_STA_CNT=0;
       key_led_flag=1;
       IR_FLAG_REMOVE=0;
       IR_FLAG_300=0;
       memset(TIM_IR_BUFFER,0,280);
       t_cnt=0;
     }
   }
}










