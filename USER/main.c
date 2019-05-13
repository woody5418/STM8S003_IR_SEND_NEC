/*************** (C) COPYRIGHT  ��ά���� ***************************************
 * �ļ���  ��main.c
 * ����    ������ģ��    
 * ��汾  ��V1.1.0
 * ������Ա��Woody QQ��2490006131
 * �޸�ʱ�䣺2016-10-14
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "delay.h"
#include "sysclock.h"
#include "tim1.h"
#include "tim2.h"
#include "flash_eeprom.h"
#include "led.h"
#include "key.h"
#include "adc.h"
/*  Defines -----------------------------------------------------------*/
/*  Variate ---------------------------------------------------------*/
u8 Write_Flag=0;

int main(void)
{ 
  static u16 ADC_Value; 
  static u8 eeprom_flag=0;
  static u8 falg_flag_flag_led=0;

  /* ϵͳʱ�ӳ�ʼ�� */
  SystemClock_Init(); 
  delay_init(16);
  Init_EEPROM(); 
  TIM1_Init();
  TIM2_Init();
  ADC_Init();
  LED_Init();
  KEY_Init();
  enableInterrupts(); 	/* �������ж� */ 
  LED_Show();
  
  while (1)
  { 
   // sendNEC();
    
    if(eeprom_flag==0)
    {
      eeprom_flag=1;
      ReadEEPROM(TIM_IR_BUFFER,IR_FLAG_REMOVE);
      IR_FLAG_REMOVE = value_eeprom ;
      value_eeprom=0;
      GPIO_WriteHigh(LED_PORT, LED_1);
      delay_ms(500);
      GPIO_WriteLow(LED_PORT, LED_1);
    }
    
    ADC_Value = ADC1_GetConversionValue(); //0-1024
   // delay_ms(1);
    key_study();
    if(key_led_flag == 1)
    {
      LED_REVERSE();
    }    
    if(ADC_Value > 45000)  // ��
    {
       delay_ms(20);
       if(ADC_Value > 45000)
       {
        ADC_Value=0;   
        Send_IR(TIM_IR_BUFFER);
        delay_ms(500);
       }
    } 
    if(Write_Flag == 1)
    {
      WriteEEPROM(TIM_IR_BUFFER,IR_FLAG_REMOVE);
      Write_Flag=0;
      falg_flag_flag_led=1;
    }
    if(falg_flag_flag_led == 1)
    {
      falg_flag_flag_led=0;
      GPIO_WriteHigh(LED_PORT, LED_1);
      delay_ms(500);
      GPIO_WriteLow(LED_PORT, LED_1);
      NEC_IR_DATA = ChangeNEC_IR(TIM_IR_BUFFER);
    } 
  }
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
    
  }
}
#endif






