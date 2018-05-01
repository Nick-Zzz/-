#include "sim800c.h"	
#include "uart4.h"
#include "delay.h"
#include "string.h" 
#include "usart1.h"
#include "lcd.h"
//sim800c发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8* sim800c_check_cmd(u8 *str)
{
	char *strx=0;
	if(UART4_RX_STA&0X8000)		//接收到一次数据了
	{ 
		UART4_RX_BUF[UART4_RX_STA&0X7FFF]=0;//添加结束符
		printf("%s\r\n",UART4_RX_BUF);
		strx=strstr((const char*)UART4_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}


//向sim900a发送命令
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 sim800c_send_cmd(u8* cmd,u8* ack,u16 waittime)
{
	u8 res=0; 
	UART4_RX_STA=0;
	if((u32)cmd<=0XFF)
	{
		while(DMA2_Channel5->CNDTR!=0);	//等待通道5传输完成   
		UART4->DR=(u32)cmd;
	}
	else u4_printf("%s\r\n",cmd);//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(UART4_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(sim800c_check_cmd(ack))
				{
					//LCD_ShowString(90,0,120,12,12,"Finish ");
					break;//得到有效数据 
				}
				UART4_RX_STA=0;
			} 
		}
		//if(waittime==0)res=1; 
	}
	return res;
} 


void gprs_init(void)
{
	sim800c_send_cmd("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"" , "OK",300);
	sim800c_send_cmd("AT+SAPBR=3,1,\"APN\",\"CMNET\"" , "OK",300);
  sim800c_send_cmd("AT+SAPBR=1,1","OK",300);
	sim800c_send_cmd("AT+HTTPINIT","OK",300);
}




