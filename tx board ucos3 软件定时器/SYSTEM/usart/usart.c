#include  "delay.h"

/**********************************************************************************
 * 串口1 配置
 * 串口1 PA9 PA10配置 及 中断相应函数
 * 库版本  ：ST_v3.5
**********************************************************************************/

#include "usart.h"


//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);
	USART1->DR = (u8) ch;      
	return ch;
}
#endif 




//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  

//初始化IO 串口1 
//bound:波特率
void uart1_init(u32 bound){
  GPIO_InitTypeDef GPIO_InitStructure;				   //IO口配置结构体
	USART_InitTypeDef USART_InitStructure;				   //串口配置结构体
	NVIC_InitTypeDef NVIC_InitStructure;				   //中断配置结构体
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
 	USART_DeInit(USART1);          //复位串口1  在开启外设前 最好都要复位一下外设
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出	 PA.9选择复用状态才能进入串口模式
	GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10
	USART_InitStructure.USART_BaudRate = bound;                //波特率设置 一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;     //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;        //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	   //收发模式   这里可以配置仅发 或仅收
  USART_Init(USART1, &USART_InitStructure); //初始化串口

#if EN_USART1_RX		  //如果使能了接收  
   //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断	 接收到数据进入中断
#endif

  USART_Cmd(USART1, ENABLE);                    //使能串口 
}

//mode 0: only TX ; mode 1: only Rx ;
void uart2_init(u32 bound,u8 mode){
  GPIO_InitTypeDef GPIO_InitStructure;				   //IO口配置结构体
	USART_InitTypeDef USART_InitStructure;				   //串口配置结构体
	NVIC_InitTypeDef NVIC_InitStructure;				   //中断配置结构体
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
 	USART_DeInit(USART2);          //复位串口1  在开启外设前 最好都要复位一下外设
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出	 PA.9选择复用状态才能进入串口模式
	GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA3
	USART_InitStructure.USART_BaudRate = bound;                //波特率设置 一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;     //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;        //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	(mode == 0) ? (USART_InitStructure.USART_Mode = USART_Mode_Tx) : (USART_InitStructure.USART_Mode = USART_Mode_Rx);
  USART_Init(USART2, &USART_InitStructure); //初始化串口

	if(mode){//如果使能了接收  
   //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断	 接收到数据进入中断
	}else{
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	}

  USART_Cmd(USART2, ENABLE);                    //使能串口 
}

void uart3_init(u32 bound,u8 mode){
  GPIO_InitTypeDef GPIO_InitStructure;				   //IO口配置结构体
	USART_InitTypeDef USART_InitStructure;				   //串口配置结构体
	NVIC_InitTypeDef NVIC_InitStructure;				   //中断配置结构体
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能USART1，GPIOA时钟
 	USART_DeInit(USART3);          //复位串口1  在开启外设前 最好都要复位一下外设
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出	 PA.9选择复用状态才能进入串口模式
	GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化PB10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化PB11
	USART_InitStructure.USART_BaudRate = bound;                //波特率设置 一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;     //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;        //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	(mode == 0) ? (USART_InitStructure.USART_Mode = USART_Mode_Tx) : (USART_InitStructure.USART_Mode = USART_Mode_Rx);
  USART_Init(USART3, &USART_InitStructure); //初始化串口

	if(mode){//如果使能了接收   
   //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断	 接收到数据进入中断
	}else{
	USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
	}

  USART_Cmd(USART3, ENABLE);                    //使能串口 
}
//这里 定义了一个16位数据USART_RX_STA 来表示 采集的数据长度  数据状态等 相当于一个寄存器
//USART_RX_STA     15		    14	         13-0
//				 接收完成	接收到0x0d	  接收的数据长度  没接收加1 表示多了一个字节
//USART_RX_STA=0 则为接收数据做准备

//串口进入中断的前提是 数据的最后以回车为准  即  0x0d 0x0a  

void USART1_IRQHandler(void)                	//串口1中断响应程序		 其名字不能随便定义
	{
	u8 Res;													//当串口接收到数据  RXNE将被置1 
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		
		if((USART_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART_RX_STA&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 			  //接收到回车的后字节  置位状态寄存器 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x4000;					 //接收到回车的前一字节  置位状态寄存器
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;			//将接收的数据 存入数组中
					USART_RX_STA++;									//长度+1 为下一次做准备
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
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
//		uart2_init(9600,0);	 //串口初始化为9600,0:only tx
//		uart3_init(9600,0);	 //串口初始化为9600,0:only tx
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
Usart1_Send_Buffer[2] :左接受, 0x01:接收到左信号，0x10:接收到右信号，0x11：5ms内左右同时收到
Usart1_Send_Buffer[2] :右接收同理
*/

void Charge_RX_Board(void)
{
	if(init != 2){
		uart2_init(9600,1);	 //串口初始化为9600,0:only rx
		uart3_init(9600,1);	 //串口初始化为9600,0:only rx
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








































