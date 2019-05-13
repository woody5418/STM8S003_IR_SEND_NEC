/********************************************************************
 *@file        key.c
 *@author      Woody      QQ��2490006131
 *@version     V1.0
 *@date        2016-10-14
 *@brief       KEY��ʼ���Լ�ɨ�躯��
 *******************************************************************/
#include "key.h"
#include "led.h"
#include "tim1.h"
#include "delay.h"
#include "string.h"

u8 KEY_STUDY=0;  //����ѧϰģʽ
//u8 LED_Low=0;
u8 key_led_flag=0;
u16 Bijiao_buffer[280];

/*******************************************************************************
 * ��������key_init
 * ����  ������key�ĳ�ʼ��
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 ******************************************************************************/
void KEY_Init(void)
{       
    GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_IN_PU_NO_IT);         
}
/*******************************************************************************
 * ��������key_study
 * ����  ������keyɨ�躯������ѧϰģʽ
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 ******************************************************************************/
void key_study(void)
{
  if(GPIO_ReadInputPin(GPIOD, GPIO_PIN_4) == 0)
   {
     delay_ms(20);
     if(GPIO_ReadInputPin(GPIOD, GPIO_PIN_4) == 0)
     {
       KEY_STUDY=1; // ����ѧϰģʽ
       IR_STA_CNT=0;
       key_led_flag=1;
       IR_FLAG_REMOVE=0;
       IR_FLAG_300=0;
       memset(TIM_IR_BUFFER,0,280);
       t_cnt=0;
     }
   }
}










