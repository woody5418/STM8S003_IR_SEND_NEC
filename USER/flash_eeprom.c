/********************************************************************
 *@file        flash_eeprom.c
 *@author      Woody      QQ��2490006131
 *@version     V1.0
 *@date        2016-10-14
 *@brief       eeprom��ʼ��
 *******************************************************************/
#include "flash_eeprom.h"
#include "uart.h"
#include "tim1.h"
#include "delay.h"


u16 value_eeprom;
/*******************************************************************************
 * ��������Init_EEPROM
 * ����  ������EEPROM�ĳ�ʼ��
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 * ˵��  : ÿ���ͺŵ�EEPROM�Ĵ�С��һ�������ô˺�����ʱ��Ҫע�⽫Ҫд�����ֽ�����
         �Ĵ�С�Ƿ񳬹����ͺŵ�EEPROM�ĵ�ַ��
         ��������EEPROM���ͺ���STM8S208, STM8S207, STM8S007, STM8AF52Ax, STM8AF62Ax 
         EEPROM�ĵ�ַ�Ǵ�0x004000��0x0047ff����2048Byte,ÿ��Block��128Byte,һ��16��Block.
         BlockNum @ 1~16
         ��������EEPROM���ͺ���STM8S105, STM8S005, STM8AF626x
         EEPROM�ĵ�ַ�Ǵ�0x004000��0x0043ff����1024Byte,ÿ��Block��128Byte,һ��8��Block.
         BlockNum @ 1~8
         С������EEPROM���ͺ���STM8S103, STM8S003, STM8S903 
         EEPROM�ĵ�ַ�Ǵ�0x004000��0x00427f����1024Byte,ÿ��Block��64Byte,һ��10��Block.
         BlockNum @ 1~10
 ******************************************************************************/
void Init_EEPROM(void)
{
    FLASH_DeInit();
}
/*******************************************************************************
 * ��������FLASH_Read_Word
 * ����  ����FLASH ��ȡһ��U16���͵�����
 * ����  ����
 * ���  ��u32�������
 * ����  ���ⲿ����
 ******************************************************************************/
void ReadEEPROM(u16* data,u16 number)
{
  u16 i;
  u8 data_change_H;
  u8 data_change_L;
  
  FLASH_Unlock(FLASH_MEMTYPE_DATA);
  data_change_H = FLASH_ReadByte(FLASH_DATA_START_PHYSICAL_ADDRESS);
  data_change_L = FLASH_ReadByte(FLASH_DATA_START_PHYSICAL_ADDRESS +1);
  number = (u16)((data_change_H & 0X00FF)<<8 | (data_change_L & 0x00FF));
  value_eeprom = number;
  
  for(i=0;i<number;i++)
  {
     data[i+1] = *(u16*)(0x4000+2+i*2);
  }
  
  FLASH_Lock(FLASH_MEMTYPE_DATA);
}
/*******************************************************************************
 * ��������FLASHu16Byte
 * ����  ����FLASH �洢һ��U16���͵�����
 * ����  ����
 * ���  ���ڲ�
 * ����  ��.C����
 ******************************************************************************/
static void FLASHu16Byte(u16 Address, u16 Data)
{
    //*(( u8*)Address)  = *((u8*)(&Data));
    //while(FLASH->IAPSR & 0x40 == 0);
   if(*((u8*)(&Data)))
    {
        *(( u8*)Address)  = *((u8*)(&Data));
        while(FLASH->IAPSR & 0x40 == 0);
    }
   if(*((u8*)(&Data)+1))
    {
        *(((u8*)Address) + 1) = *((u8*)(&Data)+1);
        while(FLASH->IAPSR & 0x40 == 0);
    }
    //*(((u8*)Address) + 1) = *((u8*)(&Data)+1);
    //while(FLASH->IAPSR & 0x40 == 0);
}
/*******************************************************************************
 * ��������WriteEEPROM
 * ����  ��WriteEEPROMдһ��U16���͵����ݵ�EEPROM
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 ******************************************************************************/
void WriteEEPROM(u16* data,u16 number)
{
  u16 i;
  u8  *f;
  
  FLASH_Unlock(FLASH_MEMTYPE_DATA); // ����EEPROM
  delay_ms(10);
  for(i=0;i<600;i++)
  {
     f = (u8*)(0X4000+i);
     if(*f!=0) 
     {
        *f=0;
        while(FLASH->IAPSR & 0x40 == 0);
     }
  } 
  FLASHu16Byte(0X4000,number);
  while(FLASH->IAPSR & 0x40 == 0);
  for(i=0;i<number;i++)
  {
    FLASHu16Byte((0X4000+2)+(i*2),data[i+1]);
  }
  FLASH_Lock(FLASH_MEMTYPE_DATA); 
} 

















