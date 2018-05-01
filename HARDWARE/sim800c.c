#include "sim800c.h"	
#include "uart4.h"
#include "delay.h"
#include "string.h" 
#include "usart1.h"
#include "lcd.h"
//sim800c���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//    ����,�ڴ�Ӧ������λ��(str��λ��)
u8* sim800c_check_cmd(u8 *str)
{
	char *strx=0;
	if(UART4_RX_STA&0X8000)		//���յ�һ��������
	{ 
		UART4_RX_BUF[UART4_RX_STA&0X7FFF]=0;//��ӽ�����
		printf("%s\r\n",UART4_RX_BUF);
		strx=strstr((const char*)UART4_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}


//��sim900a��������
//cmd:���͵������ַ���(����Ҫ��ӻس���),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 sim800c_send_cmd(u8* cmd,u8* ack,u16 waittime)
{
	u8 res=0; 
	UART4_RX_STA=0;
	if((u32)cmd<=0XFF)
	{
		while(DMA2_Channel5->CNDTR!=0);	//�ȴ�ͨ��5�������   
		UART4->DR=(u32)cmd;
	}
	else u4_printf("%s\r\n",cmd);//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
			if(UART4_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(sim800c_check_cmd(ack))
				{
					//LCD_ShowString(90,0,120,12,12,"Finish ");
					break;//�õ���Ч���� 
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




