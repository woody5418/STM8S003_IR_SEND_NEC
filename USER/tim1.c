/********************************************************************
 *@file        tim1.c
 *@author      Woody      QQ��2490006131
 *@version     V1.0
 *@date        2016-10-14
 *@brief       tim1 ��ʼ����PWM����
 *******************************************************************/
#include "tim1.h"
#include "tim2.h"
#include "delay.h"
#include "flash_eeprom.h"
#include "uart.h"
#include "led.h"
#include "key.h"

u8 t_cnt=0;  // 655+655ms �������Ƿ����
u8 EDGE_FLAG=0;  //����ת����־λ
u8 BiJiao_DATA_FLAG=0;  //���ܿ���

volatile u16 num=0;   //����ֵ
u16 IR_FLAG_300=0; //����ֵ
u16 _delay_10us=0;  // 10usһ�εļ���
u16 IR_FLAG_REMOVE=0; //���αȽ���ֵ
u16 TIM_IR_BUFFER[280];  // ��������f
u16 IR_STA_CNT=0;  // ��ַ����ֵ
u16 data_ir=0;
u32 NEC_IR_DATA=0;
/*******************************************************************************
 * ��������TIM1_Init
 * ����  ������TIM1_Init�ĳ�ʼ��
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 ******************************************************************************/
void TIM1_Init(void)
{
    CLK->PCKENR1 |=(1<<7);//��ʱ��1����ʱ��
    TIM1->CR1 = 0;        //���ض���ģʽ ���ϼ���ģʽ
    TIM1->CR2 = 0;
    TIM1->CR1 |= (1<<7);  //Ԥװ��ʹ��
    
    TIM1->PSCRH=0x00;     //Ԥ��Ƶֵ  16Mhz/160 =10us
    TIM1->PSCRL=0X9F;     //9F  10us    
    TIM1->CNTRH=0x00;     //��������ʼֵ 
    TIM1->CNTRL=0x00;
    
    TIM1->ARRH=0XFF;      //�Զ���װ��ֵ 65535
    TIM1->ARRL=0XFF;      //655ms һ���ж�
    TIM1->IER |=(1<<0);   //�����ж�
    
    TIM1->CCMR3 |=(1<<0);    //CC3ͨ��������Ϊ���룬IC3ӳ����TI3FP3��  
    TIM1->CCER2 |=(1<<1);  //ͨ��3�½��ز���      
    TIM1->CCER2 |=(1<<0);  //���벶��ͨ��3ʹ�� 
    TIM1->IER |=(1<<3);    //������3�ж�
    TIM1->CR1=0x01;        //ʹ�ܼ�����
}
/*******************************************************************************
 * ��������qiehuanyan
 * ����  ������qiehuanyan���������½����л�����
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 ******************************************************************************/
void qiehuanyan(void)
{
  if(EDGE_FLAG == 0)
  {
    EDGE_FLAG=1;
    TIM1->CCER2 &=~(1<<1);  //ͨ��3�����ز���
    TIM1->CCER2 |=(1<<0);  //���벶��ͨ��3ʹ��   
    return;
    
  }else
    if(EDGE_FLAG == 1) 
    {
      EDGE_FLAG=0;
      TIM1->CCER2 |=(1<<1);  //ͨ��3�½��ز���     
      TIM1->CCER2 |=(1<<0);  //���벶��ͨ��3ʹ�� 
      return;
    }
}

////////////////////   ����   ///////////////////////////  
/*******************************************************************************
 * ��������TIM1_CAP_COM_IRQHandler
 * ����  ��TIM1�жϣ������������벶��
 * ����  ����
 * ���  ����
 * ����  ����
 ******************************************************************************/
#pragma vector=12+2
__interrupt void TIM1_CAP_COM_IRQHandler(void)
{
  TIM1->SR1 &=~(1<<3); //����Ƚ�3�жϱ�־λ
  TIM1->CCER2 &= ~(1<<0); //�ر����벶�� 
  t_cnt=0;
  if(KEY_STUDY == 1) // ѧϰģʽ����
  {
    TIM_IR_BUFFER[IR_STA_CNT] = TIM1_GetCapture3(); //����ֵ  
  }else 
  {
    data_ir = TIM1_GetCapture3();
  }
  IR_STA_CNT++;
  TIM1->CNTRH=0X00;
  TIM1->CNTRL=0X00;
  qiehuanyan();
}

////////////////////   ��ʱ�ж�   ///////////////////////////  
/*******************************************************************************
 * ��������TIM1_UPD_OVF_TRG_BRK_IRQHandler
 * ����  ��TIM1�жϣ����������ж�
 * ����  ����
 * ���  ����
 * ����  ����
 ******************************************************************************/
#pragma vector=11+2
__interrupt void TIM1_UPD_OVF_TRG_BRK_IRQHandler(void)
{
  TIM1->SR1 = 0; ////����ж�   ����жϱ��
  if(IR_STA_CNT)
  {
  if(t_cnt==1)
  {
   t_cnt=0;
   if(IR_STA_CNT == IR_FLAG_300) 
   {
     if(KEY_STUDY == 1)
     {
       IR_FLAG_REMOVE = IR_FLAG_300;
       
       if((IR_FLAG_REMOVE <= 280)&&(IR_FLAG_REMOVE>=30))
       {
        // WriteEEPROM(TIM_IR_BUFFER,IR_FLAG_REMOVE);
         Write_Flag=1;
         KEY_STUDY=0;  // �˳�ѧϰģʽ
         key_led_flag=0;  // �˳�ѧϰ����ģʽ
         IR_FLAG_300=0;
         IR_STA_CNT=0;
         t_cnt=0;
         EDGE_FLAG=0;  
         TIM1->CCER2 |=(1<<1);  //ͨ��3�½��ز���     
         TIM1->CCER2 |=(1<<0);  //���벶��ͨ��3ʹ��
         return;
         
       } else
       {
         IR_FLAG_300=0;
         IR_STA_CNT=0;
         t_cnt=0;
         EDGE_FLAG=0;
         TIM1->CCER2 |=(1<<1);  //ͨ��3�½��ز���     
         TIM1->CCER2 |=(1<<0);  //���벶��ͨ��3ʹ��
         return;
       }
     }    
     t_cnt=0;
     EDGE_FLAG=0;
     IR_FLAG_300=0; // ��һ�μ�������
     IR_STA_CNT=0;  //��������
     TIM1->CNTRH=0X00;
     TIM1->CNTRL=0X00;
     TIM1->CCER2 |=(1<<1);  //ͨ��3�½��ز���     
     TIM1->CCER2 |=(1<<0);  //���벶��ͨ��3ʹ��
     return;
   }else 
   {
     IR_FLAG_300=0; // ��һ�μ�������
     IR_STA_CNT=0;  //��������
     EDGE_FLAG=0;
     TIM1->CCER2 |=(1<<1);  //ͨ��3�½��ز���     
     TIM1->CCER2 |=(1<<0);  //���벶��ͨ��3ʹ��
     return;
   }
  }else 
    if(t_cnt==0) 
    {
      IR_FLAG_300=IR_STA_CNT; //��¼��һ���жϵļ���ֵ
    } 
  t_cnt++;
  }
}


static void start_PWM(u16 num_t)
{
    send_flag = 0;
    flag_senf=1;
    count=0;
    num = num_t;
    TIM2_Cmd(ENABLE); 
    while(flag_senf) ;
}

//9ms ��  4.5ms  ��
static void  Logic_Start(void)
{
    IR_Send_L; 
    start_PWM(550);   //9ms
    IR_Send_L;
    delay_ms(4);
    delay_10us(33);
}

//560us��    1685us ��
static void  Logic_1(void)
{
    IR_Send_L;
    start_PWM(33);  //560us  H
    IR_Send_L;
    delay_10us(158);  //1685us  L
}
// 560us��  565us ��
static void  Logic_0(void)
{
    IR_Send_L;
    start_PWM(33);  //560us  H
    IR_Send_L;
    delay_10us(52);  //560us
}




//����NEC��
void sendNECcode(u32 temp)
{
    Logic_Start();
    for(char i=0;i<32;i++){
      if(temp & 0x80000000) {
        Logic_1();
        LED__H;
      }
      else {
        Logic_0();
        LED__L;
      }
      temp <<= 1;
    }
    IR_Send_L;
    start_PWM(33);  //560us  H
    IR_Send_L;
    LED__L;
}

//  ����ѧϰ�ĺ������
void sendLearning(u16* temp)
{
  // u8 Start_flag=0;
   u16 j_cnt;
   TIM1_Cmd(DISABLE);
 // TIM1->CCER2 &= ~(1<<0); //�ر����벶��
  for(j_cnt=0;j_cnt<IR_FLAG_REMOVE;j_cnt++)
  {
    if(j_cnt == 0) continue;    
    
      if((j_cnt%2) == 0)
      {
        IR_Send_L;
        LED__L;
        delay_10us(temp[j_cnt]);
        continue;
      }  
      if((j_cnt%2) != 0)
      {
         if((temp[j_cnt]>800) && (temp[j_cnt]<1000)) 
            num = temp[j_cnt]*10/18;
         else
            num = temp[j_cnt]*10/13;
        send_flag = 0;
       // Start_flag=1;
        flag_senf=1;
        count=0;
        TIM2_Cmd(ENABLE); 
        while(flag_senf) ;
        IR_Send_L;
        LED__L;
        continue;
      }
  }
  IR_Send_L;
  LED__L;
  //TIM1->CCER2 |=(1<<0);  //���벶��ͨ��3ʹ��
  TIM1_Cmd(ENABLE);
} 

//�����NEC����������
void Send_IR(u16* temp){
  
  if(NEC_IR_DATA){  //NEC ͷ  9+4.5
      sendNECcode(NEC_IR_DATA);   //NEC 
  }else{
      //sendLearning(temp);  //����
  }
}

//����ת����NEC��
u32  ChangeNEC_IR(u16* temp)
{
    u8 j=3;
    u32 NEC_temp;  // ��������f
    if(((1100>=temp[1]) && (temp[1]>=700))&&( (600>=temp[2]) && (temp[2]>=300) )){  //NEC ͷ  9+4.5
       for(char i=0;i<32;i++){
         if(((80>=temp[j]) && (temp[j]>=30)) && ((80>=temp[j+1]) && (temp[j+1]>=30))){  //0
            NEC_temp <<=1;
         }
         if(((80>=temp[j]) && (temp[j]>=30)) && ((190>=temp[j+1]) && (temp[j+1]>=130))){  //1
            NEC_temp <<= 1;
            NEC_temp |= 0x0001;
         }
         j+=2;
       }
       return NEC_temp;
    }
    return 0;
}
