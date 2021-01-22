#include  "delay.h"

/**********************************************************************************
 * ����1 ����
 * ����1 PA9 PA10���� �� �ж���Ӧ����
 * ��汾  ��ST_v3.5
**********************************************************************************/

#include "usart.h"


//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);
	USART1->DR = (u8) ch;      
	return ch;
}
#endif 




//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  

//��ʼ��IO ����1 
//bound:������
void uart1_init(u32 bound){
  GPIO_InitTypeDef GPIO_InitStructure;				   //IO�����ýṹ��
	USART_InitTypeDef USART_InitStructure;				   //�������ýṹ��
	NVIC_InitTypeDef NVIC_InitStructure;				   //�ж����ýṹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	USART_DeInit(USART1);          //��λ����1  �ڿ�������ǰ ��ö�Ҫ��λһ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������	 PA.9ѡ����״̬���ܽ��봮��ģʽ
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10
	USART_InitStructure.USART_BaudRate = bound;                //���������� һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;     //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;        //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	   //�շ�ģʽ   ����������ý��� �����
  USART_Init(USART1, &USART_InitStructure); //��ʼ������

#if EN_USART1_RX		  //���ʹ���˽���  
   //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�	 ���յ����ݽ����ж�
#endif

  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 
}

//mode 0: only TX ; mode 1: only Rx ;
void uart2_init(u32 bound,u8 mode){
  GPIO_InitTypeDef GPIO_InitStructure;				   //IO�����ýṹ��
	USART_InitTypeDef USART_InitStructure;				   //�������ýṹ��
	NVIC_InitTypeDef NVIC_InitStructure;				   //�ж����ýṹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	USART_DeInit(USART2);          //��λ����1  �ڿ�������ǰ ��ö�Ҫ��λһ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������	 PA.9ѡ����״̬���ܽ��봮��ģʽ
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA3
	USART_InitStructure.USART_BaudRate = bound;                //���������� һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;     //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;        //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	(mode == 0) ? (USART_InitStructure.USART_Mode = USART_Mode_Tx) : (USART_InitStructure.USART_Mode = USART_Mode_Rx);
  USART_Init(USART2, &USART_InitStructure); //��ʼ������

	if(mode){//���ʹ���˽���  
   //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�	 ���յ����ݽ����ж�
	}else{
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	}

  USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 
}

void uart3_init(u32 bound,u8 mode){
  GPIO_InitTypeDef GPIO_InitStructure;				   //IO�����ýṹ��
	USART_InitTypeDef USART_InitStructure;				   //�������ýṹ��
	NVIC_InitTypeDef NVIC_InitStructure;				   //�ж����ýṹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	USART_DeInit(USART3);          //��λ����1  �ڿ�������ǰ ��ö�Ҫ��λһ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������	 PA.9ѡ����״̬���ܽ��봮��ģʽ
	GPIO_Init(GPIOB, &GPIO_InitStructure); //��ʼ��PB10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);  //��ʼ��PB11
	USART_InitStructure.USART_BaudRate = bound;                //���������� һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;     //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;        //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	(mode == 0) ? (USART_InitStructure.USART_Mode = USART_Mode_Tx) : (USART_InitStructure.USART_Mode = USART_Mode_Rx);
  USART_Init(USART3, &USART_InitStructure); //��ʼ������

	if(mode){//���ʹ���˽���   
   //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�	 ���յ����ݽ����ж�
	}else{
	USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
	}

  USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ��� 
}
//���� ������һ��16λ����USART_RX_STA ����ʾ �ɼ������ݳ���  ����״̬�� �൱��һ���Ĵ���
//USART_RX_STA     15		    14	         13-0
//				 �������	���յ�0x0d	  ���յ����ݳ���  û���ռ�1 ��ʾ����һ���ֽ�
//USART_RX_STA=0 ��Ϊ����������׼��

//���ڽ����жϵ�ǰ���� ���ݵ�����Իس�Ϊ׼  ��  0x0d 0x0a  

void USART1_IRQHandler(void)                	//����1�ж���Ӧ����		 �����ֲ�����㶨��
	{
	u8 Res;													//�����ڽ��յ�����  RXNE������1 
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
		
		if((USART_RX_STA&0x8000)==0)//����δ���
			{
			if(USART_RX_STA&0x4000)//���յ���0x0d
				{
				if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x8000;	//��������� 			  //���յ��س��ĺ��ֽ�  ��λ״̬�Ĵ��� 
				}
			else //��û�յ�0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x4000;					 //���յ��س���ǰһ�ֽ�  ��λ״̬�Ĵ���
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;			//�����յ����� ����������
					USART_RX_STA++;									//����+1 Ϊ��һ����׼��
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}   		 
     } 

} 

u8 init=0;
//u8 L_IR[5] = {0XFA,0XF1,0x33,0x33,0x33};
//u8 R_IR[5] = {0XFA,0XF1,0xCC,0xCC,0xCC};

//void Charge_TX_Board(void)
//{
//	if(init != 1){
//		uart2_init(9600,0);	 //���ڳ�ʼ��Ϊ9600,0:only tx
//		uart3_init(9600,0);	 //���ڳ�ʼ��Ϊ9600,0:only tx
//		printf(" tx board \r\n");
//		init = 1;
//	}
//	for(u8 i=0; i<5; i++)
//	{
//		while (USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
//		USART_SendData(USART2, L_IR[i]);
//		delay_ms(10);
//		while (USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET);
//		USART_SendData(USART3, R_IR[i]);
//	}	
//}	

u8 L_IR_Rec[3],R_IR_Rec[3],L_IR_RecTemp,R_IR_RecTemp,L_IR_SendTemp,R_IR_SendTemp,Lcount,Rcount,L_State,R_State,L_Timing,R_Timing,L_Timing_L,L_Timing_R,R_Timing_L,R_Timing_R;
u8 Usart1_Send_Buffer[5] = {0XAA,0XBB,0x00,0X00,0XCC};
/*
Usart1_Send_Buffer[2] :�����, 0x01:���յ����źţ�0x10:���յ����źţ�0x11��5ms������ͬʱ�յ�
Usart1_Send_Buffer[2] :�ҽ���ͬ��
*/

void Charge_RX_Board(void)
{
	if(init != 2){
		uart2_init(9600,1);	 //���ڳ�ʼ��Ϊ9600,0:only rx
		uart3_init(9600,1);	 //���ڳ�ʼ��Ϊ9600,0:only rx
		printf(" rx board \r\n");
		init = 2;
	}
	
	if(L_Timing++ >= 5) {L_IR_RecTemp = 0; L_Timing = 5;}
	if(R_Timing++ >= 5) {R_IR_RecTemp = 0; R_Timing = 5;}
//	Usart1_Send_Buffer[2] = L_State;
//	Usart1_Send_Buffer[3] = R_State;
	if(L_Timing_L++ >= 15) {L_IR_SendTemp &= ~(0x01<<4); L_Timing_L = 15;}
	if(L_Timing_R++ >= 15) {L_IR_SendTemp &= ~(0x01<<0); L_Timing_R = 15;}	
	if(R_Timing_L++ >= 15) {R_IR_SendTemp &= ~(0x01<<4); R_Timing_L = 15;}
	if(R_Timing_R++ >= 15) {R_IR_SendTemp &= ~(0x01<<0); R_Timing_R = 15;}	
	
	if(L_IR_RecTemp == 0x33 ) 
		L_IR_SendTemp = L_IR_SendTemp | (0X01 << 4);
	else if(L_IR_RecTemp == 0xCC)
		L_IR_SendTemp = L_IR_SendTemp | (0X01 << 0);
	
	if(R_IR_RecTemp == 0x33 ) 
		R_IR_SendTemp = R_IR_SendTemp | (0X01 << 4);
	else if(R_IR_RecTemp == 0xCC)
		R_IR_SendTemp = R_IR_SendTemp | (0X01 << 0);
	
	Usart1_Send_Buffer[2] = L_IR_SendTemp;
	Usart1_Send_Buffer[3] = R_IR_SendTemp;	
	
	for(u8 i=0; i<5; i++)
	{
		while (USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
		USART_SendData(USART1, Usart1_Send_Buffer[i]);
	}	
}	

void USART2_IRQHandler(void)                	
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  
	{
		L_Timing = 0;
		L_IR_RecTemp = USART_ReceiveData(USART2);
		if(L_IR_RecTemp == 0x33)
			L_Timing_L = 0;
		else if(L_IR_RecTemp == 0xCC)
			L_Timing_R = 0;
//		L_IR_Rec[Lcount] = USART_ReceiveData(USART2);
//		(L_IR_Rec[0] == 0xAA)?(Lcount++):(Lcount = 0);
//		if (Lcount == 3)
//		{	
//			if(L_IR_Rec[2] == 0xBB)
//			{
//				L_State = L_IR_Rec[1];
//			}			
//			Lcount = 0;
//		}
	} 
}

void USART3_IRQHandler(void)                	
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  
	{
		R_Timing = 0;
		R_IR_RecTemp = USART_ReceiveData(USART3);
		if(R_IR_RecTemp == 0x33)
			R_Timing_L = 0;
		else if(R_IR_RecTemp == 0xCC)
			R_Timing_R = 0;
//		R_IR_Rec[Rcount] = USART_ReceiveData(USART3);
//		(R_IR_Rec[0] == 0xAA)?(Rcount++):(Rcount = 0);
//		if (Rcount == 3)
//		{	
//			if(R_IR_Rec[2] == 0xBB)
//			{
//				R_State = R_IR_Rec[1];
//			}			
//			Rcount = 0;
//		}
	} 
}








































