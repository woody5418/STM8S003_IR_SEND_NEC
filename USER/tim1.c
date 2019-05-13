/********************************************************************
 *@file        tim1.c
 *@author      Woody      QQ：2490006131
 *@version     V1.0
 *@date        2016-10-14
 *@brief       tim1 初始化，PWM设置
 *******************************************************************/
#include "tim1.h"
#include "tim2.h"
#include "delay.h"
#include "flash_eeprom.h"
#include "uart.h"
#include "led.h"
#include "key.h"

u8 t_cnt=0;  // 655+655ms 检测接受是否完毕
u8 EDGE_FLAG=0;  //边沿转换标志位
u8 BiJiao_DATA_FLAG=0;  //功能控制

volatile u16 num=0;   //数组值
u16 IR_FLAG_300=0; //计数值
u16 _delay_10us=0;  // 10us一次的计数
u16 IR_FLAG_REMOVE=0; //二次比较数值
u16 TIM_IR_BUFFER[280];  // 缓冲数组f
u16 IR_STA_CNT=0;  // 地址计数值
u16 data_ir=0;
u32 NEC_IR_DATA=0;
/*******************************************************************************
 * 函数名：TIM1_Init
 * 描述  ：配置TIM1_Init的初始化
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 ******************************************************************************/
void TIM1_Init(void)
{
    CLK->PCKENR1 |=(1<<7);//定时器1配置时钟
    TIM1->CR1 = 0;        //边沿对齐模式 向上计数模式
    TIM1->CR2 = 0;
    TIM1->CR1 |= (1<<7);  //预装载使能
    
    TIM1->PSCRH=0x00;     //预分频值  16Mhz/160 =10us
    TIM1->PSCRL=0X9F;     //9F  10us    
    TIM1->CNTRH=0x00;     //计数器初始值 
    TIM1->CNTRL=0x00;
    
    TIM1->ARRH=0XFF;      //自动重装的值 65535
    TIM1->ARRL=0XFF;      //655ms 一次中断
    TIM1->IER |=(1<<0);   //允许中断
    
    TIM1->CCMR3 |=(1<<0);    //CC3通道被配置为输入，IC3映射在TI3FP3上  
    TIM1->CCER2 |=(1<<1);  //通道3下降沿捕获      
    TIM1->CCER2 |=(1<<0);  //输入捕获通道3使能 
    TIM1->IER |=(1<<3);    //允许捕获3中断
    TIM1->CR1=0x01;        //使能计数器
}
/*******************************************************************************
 * 函数名：qiehuanyan
 * 描述  ：配置qiehuanyan的上升或下降沿切换函数
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 ******************************************************************************/
void qiehuanyan(void)
{
  if(EDGE_FLAG == 0)
  {
    EDGE_FLAG=1;
    TIM1->CCER2 &=~(1<<1);  //通道3上升沿捕获
    TIM1->CCER2 |=(1<<0);  //输入捕获通道3使能   
    return;
    
  }else
    if(EDGE_FLAG == 1) 
    {
      EDGE_FLAG=0;
      TIM1->CCER2 |=(1<<1);  //通道3下降沿捕获     
      TIM1->CCER2 |=(1<<0);  //输入捕获通道3使能 
      return;
    }
}

////////////////////   捕获   ///////////////////////////  
/*******************************************************************************
 * 函数名：TIM1_CAP_COM_IRQHandler
 * 描述  ：TIM1中断，用于配置输入捕获
 * 输入  ：无
 * 输出  ：无
 * 调用  ：无
 ******************************************************************************/
#pragma vector=12+2
__interrupt void TIM1_CAP_COM_IRQHandler(void)
{
  TIM1->SR1 &=~(1<<3); //清除比较3中断标志位
  TIM1->CCER2 &= ~(1<<0); //关闭输入捕获 
  t_cnt=0;
  if(KEY_STUDY == 1) // 学习模式进入
  {
    TIM_IR_BUFFER[IR_STA_CNT] = TIM1_GetCapture3(); //捕获值  
  }else 
  {
    data_ir = TIM1_GetCapture3();
  }
  IR_STA_CNT++;
  TIM1->CNTRH=0X00;
  TIM1->CNTRL=0X00;
  qiehuanyan();
}

////////////////////   定时中断   ///////////////////////////  
/*******************************************************************************
 * 函数名：TIM1_UPD_OVF_TRG_BRK_IRQHandler
 * 描述  ：TIM1中断，用于配置中断
 * 输入  ：无
 * 输出  ：无
 * 调用  ：无
 ******************************************************************************/
#pragma vector=11+2
__interrupt void TIM1_UPD_OVF_TRG_BRK_IRQHandler(void)
{
  TIM1->SR1 = 0; ////溢出中断   清除中断标记
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
         KEY_STUDY=0;  // 退出学习模式
         key_led_flag=0;  // 退出学习亮灯模式
         IR_FLAG_300=0;
         IR_STA_CNT=0;
         t_cnt=0;
         EDGE_FLAG=0;  
         TIM1->CCER2 |=(1<<1);  //通道3下降沿捕获     
         TIM1->CCER2 |=(1<<0);  //输入捕获通道3使能
         return;
         
       } else
       {
         IR_FLAG_300=0;
         IR_STA_CNT=0;
         t_cnt=0;
         EDGE_FLAG=0;
         TIM1->CCER2 |=(1<<1);  //通道3下降沿捕获     
         TIM1->CCER2 |=(1<<0);  //输入捕获通道3使能
         return;
       }
     }    
     t_cnt=0;
     EDGE_FLAG=0;
     IR_FLAG_300=0; // 上一次计数清零
     IR_STA_CNT=0;  //计数清零
     TIM1->CNTRH=0X00;
     TIM1->CNTRL=0X00;
     TIM1->CCER2 |=(1<<1);  //通道3下降沿捕获     
     TIM1->CCER2 |=(1<<0);  //输入捕获通道3使能
     return;
   }else 
   {
     IR_FLAG_300=0; // 上一次计数清零
     IR_STA_CNT=0;  //计数清零
     EDGE_FLAG=0;
     TIM1->CCER2 |=(1<<1);  //通道3下降沿捕获     
     TIM1->CCER2 |=(1<<0);  //输入捕获通道3使能
     return;
   }
  }else 
    if(t_cnt==0) 
    {
      IR_FLAG_300=IR_STA_CNT; //记录第一次中断的计数值
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

//9ms 高  4.5ms  低
static void  Logic_Start(void)
{
    IR_Send_L; 
    start_PWM(550);   //9ms
    IR_Send_L;
    delay_ms(4);
    delay_10us(33);
}

//560us高    1685us 低
static void  Logic_1(void)
{
    IR_Send_L;
    start_PWM(33);  //560us  H
    IR_Send_L;
    delay_10us(158);  //1685us  L
}
// 560us高  565us 高
static void  Logic_0(void)
{
    IR_Send_L;
    start_PWM(33);  //560us  H
    IR_Send_L;
    delay_10us(52);  //560us
}




//发送NEC码
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

//  发送学习的红外编码
void sendLearning(u16* temp)
{
  // u8 Start_flag=0;
   u16 j_cnt;
   TIM1_Cmd(DISABLE);
 // TIM1->CCER2 &= ~(1<<0); //关闭输入捕获
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
  //TIM1->CCER2 |=(1<<0);  //输入捕获通道3使能
  TIM1_Cmd(ENABLE);
} 

//检测是NEC吗还是万能码
void Send_IR(u16* temp){
  
  if(NEC_IR_DATA){  //NEC 头  9+4.5
      sendNECcode(NEC_IR_DATA);   //NEC 
  }else{
      //sendLearning(temp);  //万能
  }
}

//数据转化到NEC码
u32  ChangeNEC_IR(u16* temp)
{
    u8 j=3;
    u32 NEC_temp;  // 缓冲数组f
    if(((1100>=temp[1]) && (temp[1]>=700))&&( (600>=temp[2]) && (temp[2]>=300) )){  //NEC 头  9+4.5
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
