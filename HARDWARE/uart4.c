#include "uart4.h"	
#include "sys.h"
#include "delay.h" 
#include "timer4.h"
#include "stm32f10x_dma.h"
#include "string.h"
#include "stdarg.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
//#if 1
//#pragma import(__use_no_semihosting)             
////标准库需要的支持函数                 
//struct __FILE 
//{ 
//	int handle; 

//}; 

//FILE __stdout;       
////定义_sys_exit()以避免使用半主机模式    
//_sys_exit(int x) 
//{ 
//	x = x; 
//} 
////重定义fputc函数 
//int fputc(int ch, FILE *f)
//{      
//	while((UART4->SR&0X40)==0);//循环发送,直到发送完毕   
//    UART4->DR = (u8) ch;      
//	return ch;
//}
//#endif 

/*使用microLib的方法*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 

//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 UART4_RX_BUF[UART4_MAX_RECV_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
__align(8) u8 UART4_TX_BUF[UART4_MAX_SEND_LEN]; 
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 UART4_RX_STA=0;       //接收状态标记	  

//初始化IO 串口1 
//bound:波特率
void uart4_init(u32 bound){
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	  USART_InitTypeDef USART_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	 
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//使能，GPIOC时钟
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
 	  USART_DeInit(UART4);  //复位串口4
	 //UART4_TX   PC.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化PC9
   
    //UART4_RX	  PC.11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);  //初始化PC.11

   //Uart4 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级3
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	  NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	 USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	 USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	 USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	 USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	 USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	 USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

   USART_Init(UART4, &USART_InitStructure); //初始化串口
	 USART_DMACmd(UART4,USART_DMAReq_Tx,ENABLE);  	//使能串口4的DMA发送
	 UART_DMA_Config(DMA2_Channel5,(u32)&UART4->DR,(u32)UART4_TX_BUF);//DMA2通道5,外设为串口4,存储器为UART4_TX_BUF
	 USART_Cmd(UART4, ENABLE);                    //使能串口 
	 USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启中断
	 TIM4_Init(99,7199);		//10ms中断
		UART4_RX_STA=0;		//清零
		TIM4_Set(0);			//关闭定时器4

}
#if EN_UART4_RX   //如果使能了接收
void UART4_IRQHandler(void)                	//串口4中断服务程序
	{
	u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntEnter();    
#endif

if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)//接收到数据
	{	 
		Res =USART_ReceiveData(UART4);				 
		if(UART4_RX_STA<UART4_MAX_RECV_LEN)		//还可以接收数据
		{
		  TIM_SetCounter(TIM4,0);//计数器清空      
			if(UART4_RX_STA==0)TIM4_Set(1);	 	//使能定时器4的中断 
			UART4_RX_BUF[UART4_RX_STA++]=Res;		//记录接收到的值
	
		}else 
		{
			UART4_RX_STA|=1<<15;					//强制标记接收完成
		} 		
		
	}  											 



#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();  											 
#endif
} 
#endif	



//DMA1的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//DMA_CHx:DMA通道CHx
//cpar:外设地址
//cmar:存储器地址    
void UART_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar)
{
	DMA_InitTypeDef DMA_InitStructure;
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);	//使能DMA传输
  DMA_DeInit(DMA_CHx);   //将DMA的通道1寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;  //DMA外设ADC基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar;  //DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  //数据传输方向，从内存读取发送到外设
	DMA_InitStructure.DMA_BufferSize = 0;  //DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMA通道 x拥有中优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA_CHx, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器	
} 
//开启一次DMA传输
void UART_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,u16 len)
{
	DMA_Cmd(DMA_CHx, DISABLE );  //关闭 指示的通道        
	DMA_SetCurrDataCounter(DMA_CHx,len);//DMA通道的DMA缓存的大小	
	DMA_Cmd(DMA_CHx, ENABLE);           //开启DMA传输
}	   

//串口4,printf 函数
//确保一次发送数据不超过USART2_MAX_SEND_LEN字节
void u4_printf(char* fmt,...)  
{  
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)UART4_TX_BUF,fmt,ap);
	va_end(ap);
	while(DMA2_Channel5->CNDTR!=0);	//等待通道7传输完成   
	UART_DMA_Enable(DMA2_Channel5,strlen((const char*)UART4_TX_BUF)); 	//通过dma发送出去
}


