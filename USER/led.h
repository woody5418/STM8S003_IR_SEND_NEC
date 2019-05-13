/********************************************************************
 *@file        led.h
 *@author      Woody      QQ��2490006131
 *@version     V1.0
 *@date        2016-10-14
 *@brief       led ��ʼ���Լ�LED_Show����
 *******************************************************************/
#ifndef __LED_H
#define __LED_H

#include "stm8s.h"

#define LED_PORT            GPIOD	
#define LED_1               GPIO_PIN_6

#define ON       0		/* ����LED���� -- �͵�ƽ*/
#define OFF      1		/* ����LED���� -- �ߵ�ƽ*/

void LED_Init(void);
void IR_Init(void);
void LED_REVERSE(void);
void LED_Show(void);
void YES_STUDY(void);
void LED_Config(GPIO_Pin_TypeDef led, u8 state);


#endif