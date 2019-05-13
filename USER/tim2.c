/********************************************************************
 *@file        tim2.c
 *@author      Woody      QQ��2490006131
 *@version     V1.0
 *@date        2016-10-14
 *@brief       tim1 ��ʼ����PWM����
 *******************************************************************/
#include "tim2.h"
#include "uart.h"
#include "delay.h"
#include "tim1.h"

u8 send_flag=0;
volatile u8 flag_senf=0;
u16 count=0;

/*******************************************************************************
 * ��������TIM2_Init
 * ����  ������TIM2_Init�ĳ�ʼ��
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 ******************************************************************************/
void TIM2_Init(void)
{
  TIM2->PSCR = 0x04; //ϵͳʱ�Ӿ�Ԥ��Ƶf=16M/16=1Mhz,04=4, 2^4=16;  1us
  TIM2->ARRH = 0x00; // 1/1Mhz * 13=13us
  TIM2->ARRL = 0x0D; //0X000D=13 
  TIM2->IER |= 0x01; //���и����ж�
  //TIM2->CR1 |= 0x01; //ʹ�ܼ�����
  TIM2_Cmd(DISABLE);
}
/*******************************************************************************
 * ��������TIM2_UPD_OVF_BRK_IRQHandler
 * ����  ��TIM2�жϣ�����38Khz
 * ����  ����
 * ���  ����
 * ����  ����
 * ˵��  ��50us * 800 =40ms
 ******************************************************************************/
#pragma vector=0xF       //13+2
__interrupt void TIM2_UPD_OVF_BRK_IRQHandler(void)//����жϣ����Ŀ���ָ��75ҳ
{
  TIM2->SR1 = 0; //����жϱ��
  count++;
  if(count >= num) 
  {
     TIM2_Cmd(DISABLE);  
     flag_senf=count=0;
   }
  if(send_flag == 0)  
  {
     send_flag=1;
     IR_Send_H;
     LED__H;
  }else 
  {
     send_flag = 0;
     IR_Send_L;
     LED__L;
  } 
}




