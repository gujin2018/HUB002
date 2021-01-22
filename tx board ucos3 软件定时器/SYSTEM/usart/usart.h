#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f10x.h"

#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
#define EN_USART2_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����	 
#define EN_USART3_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����	

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO�ڵ�ַӳ��
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x40010C0C 
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //��� 

#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //��� 

#define LED_RED	PCout(13)
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart1_init(u32 bound);
void uart2_init(u32 bound, u8 mode);
void uart3_init(u32 bound, u8 mode);

void Charge_TX_Board(void);
void Charge_RX_Board(void);
#endif


