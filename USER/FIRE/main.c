#include "stm32f10x.h"
#include "sys.h"
#include "usart1.h"
#include "delay.h"
#include "uart4.h"
#include "sim800c.h"
#include "string.h"
#include "lcd.h"
#include "flash.h"
#include "spi.h"	

#define  ID  05
#define  PM   6
#define  HCHO  14
#define  TEMPERATURE 15
#define  WET   16
u16 data[18];


/****************��ʾ��Ҫ������**************/
typedef struct  
{ 					    
	u8 pm25;	//PM2.5 ��λ
	u8 temp1;	//�¶���λ����		
	u8 temp2;	//�¶�һλС��
	u8  shidu1;	//ʪ����λ����			
	u8  shidu2;	//ʪ��һλС��
	u8  jiaq1;	//��ȩһλ����	
	u8  jiaq2;	 //��ȩ��λ����
  u8  jiaq3;
  u8  jiaq4;	
}canshu;

canshu ccanshu;

void show_init(void)
{
  showBEIJING(0,0);//��������
  POINT_COLOR=BLUE;
  BACK_COLOR=WHITE;
  showhanzi16(5,90,6);	delay_ms(100);  
	showhanzi16(25,90,7);	  delay_ms(100); 
	showhanzi16(45,90,8);	delay_ms(100);    
	showhanzi16(65,90,9);	delay_ms(100);   
	showhanzi16(85,90,10);	  delay_ms(100); 
	showhanzi16(105,90,11);delay_ms(100);  
  POINT_COLOR=BLUE;
  //showhanzi16(0,115,12);delay_ms(100); 
	//showhanzi16(17,115,13);	delay_ms(100);  
	showhanzi16(33-10,115,12);	delay_ms(100);  
	//showhanzi16(49,115,15);	delay_ms(100); 
	showhanzi16(65-10,115,13);delay_ms(100); 
	//showhanzi16(81,115,17);  delay_ms(100); 
  showhanzi16(97-10,115,14);	 delay_ms(100);  
	//showhanzi16(113,115,19); delay_ms(1200); 
   delay_ms(1000);  
    POINT_COLOR= BRED ;//��������Ϊ��ɫ 
    BACK_COLOR=BLACK;
    LCD_Clear(BLACK);
     
    LCD_ShowString(2,0,126,16,16,"ID:   ");//ID
    LCD_ShowxNum(30,0,ID,2,16,0xff);       //��λ��
    POINT_COLOR=BLUE; 
    Draw_Circle(64,54,40);
    Draw_Circle(64,54,41);
    Draw_Circle(64,54,42);
    POINT_COLOR=GREEN;
    LCD_ShowString(44,24,126,16,16,"PM2.5"); //PM2.5
    LCD_ShowString(48,75,126,12,12,"ug/m^3"); 
    LCD_Fill(44,44,82,68,YELLOW );
    POINT_COLOR=RED;
    LCD_ShowxNum(56,48,ccanshu.pm25,2,16,0xff);//��λ����
    
     POINT_COLOR=YELLOW;
    LCD_DrawLine(0,99,126,99);//�ĺ�һ��
    LCD_DrawLine(0,119,126,119);
    LCD_DrawLine(0,139,126,139);
    LCD_DrawLine(0,159,126,159);
    LCD_DrawLine(37,99,37,159);
     POINT_COLOR=GREEN;
    showhanzi16(2,101,0);//��ȩ
    showhanzi16(18,101,1); 
    LCD_ShowxNum(42,101,ccanshu.jiaq1,1,16,0);//һλ����
    LCD_ShowChar(50,101,'.',16,0);
    LCD_ShowxNum(58,101,ccanshu.jiaq2,3,16,0xff);//��λС��
    LCD_ShowString(89,105,126,16,12,"mg/m^3");
        
    showhanzi16(2,121,2);//�¶�
    showhanzi16(18,121,3); 
    LCD_ShowChar(42,121,'+',16,0);//������
    LCD_ShowxNum(50,121,ccanshu.temp1,2,16,0xff);//��λ����
    LCD_ShowChar(66,121,'.',16,0);
    LCD_ShowxNum(74,121,ccanshu.temp2,1,16,0xff);//һλС��
    LCD_ShowString(90,121,126,12,12,"o");
    LCD_ShowString(98,121,126,16,16,"C");
   
   showhanzi16(2,141,4);//ʪ��
   showhanzi16(18,141,5); 
   LCD_ShowxNum(50,141,ccanshu.shidu1,2,16,0xff);//��λ����
   LCD_ShowChar(66,141,'.',16,0);
   LCD_ShowxNum(74,141,ccanshu.shidu2,1,16,0xff);//һλС��
   LCD_ShowString(90,141,126,16,16,"%");

} 
void show(u16 pm,u16 hcho,u16 temp ,u16 wet)
{
   ccanshu.pm25=pm;
	 ccanshu.jiaq1=hcho/1000;
	 ccanshu.jiaq2=hcho%1000/100;
   ccanshu.jiaq3=hcho%1000%100/10;
   ccanshu.jiaq4=hcho%10;

	 ccanshu.temp1=temp/10;
	 ccanshu.temp2=temp%10;
	
   ccanshu.shidu1=wet/10;
	 ccanshu.shidu2=wet%10;


	  POINT_COLOR=RED; BACK_COLOR=YELLOW;
    if(ccanshu.pm25==0)
         LCD_ShowxNum(56,48, ccanshu.pm25,2,16,0xff);//��λ����   PM2.5
    else LCD_ShowxNum(56,48, ccanshu.pm25,2,16,0);
    
    POINT_COLOR=GREEN; BACK_COLOR=BLACK;
		
		LCD_ShowxNum(42,101,ccanshu.jiaq1,1,16,0);//һλ����    ��ȩ
    LCD_ShowxNum(58,101,ccanshu.jiaq2,1,16,0);//��λС��
    LCD_ShowxNum(66,101,ccanshu.jiaq3,1,16,0);
    LCD_ShowxNum(74,101,ccanshu.jiaq4,1,16,0);

//    if(ccanshu.jiaq1==0)
//         LCD_ShowxNum(42,101,ccanshu.jiaq1,1,16,0xff);//һλ����    ��ȩ
//    else LCD_ShowxNum(42,101,ccanshu.jiaq1,1,16,0);
//    if(ccanshu.jiaq2==0)
//         LCD_ShowxNum(58,101,ccanshu.jiaq2,3,16,0xff);//��λС��
//    else LCD_ShowNum(58,101,ccanshu.jiaq2,3,16);
    if(ccanshu.temp1==0)
         LCD_ShowxNum(50,121,ccanshu.temp1,2,16,0xff);//��λ����  �¶�
    else LCD_ShowxNum(50,121,ccanshu.temp1,2,16,0);
    if(ccanshu.temp2==0)
         LCD_ShowxNum(74,121,ccanshu.temp2,1,16,0xff);//һλС��
    else LCD_ShowxNum(74,121,ccanshu.temp2,1,16,0);
    if(ccanshu.shidu1==0)
          LCD_ShowxNum(50,141,ccanshu.shidu1,2,16,0xff);//��λ����  ʪ��
    else  LCD_ShowxNum(50,141,ccanshu.shidu1,2,16,0);
    if(ccanshu.shidu2==0)
         LCD_ShowxNum(74,141,ccanshu.shidu2,1,16,0xff);//һλС��
    else  LCD_ShowxNum(74,141,ccanshu.shidu2,1,16,0);
}







/*******��ʽ��u8 ass[1024]=
"AT+HTTPPARA=\"URL\",\"http://113.79.121.88:80/index.php?id=1&pm=45&hcho=11&temperature=28&wet=70&electricity=10\"";********/

char ass[1024]="AT+HTTPPARA=\"URL\",\"http://14.154.121.130:80/receive.php?location=a&id=";
char ass1[]="&pm2_5=\0&methanal=\0&temperature=\0&humidity=\0&electric=\0\"\0";
u16 len=0;
/*********��ȡ��Ҫ�Ĳ���***********/
u16 Get_data(u8 i)
{
	data[i]=256*USART_RX_BUF[2*i-1]+USART_RX_BUF[2*i];
	return(data[i]);	
}
/***************��u16ת��Ϊ�ַ���*****************/
void change(u8 temp[],u16 number )
{
	if(number<10)
		{
			temp[0]=number+'0';
			temp[1]='\0';
		}
	else if((9<number)&&(number<100))
		{
			temp[0]=number/10+'0';
			temp[1]=number%10+'0';
			temp[2]='\0';
		}
	else if((99<number)&&(number<1000))
		{
			temp[0]=number/100+'0';
			temp[1]=number/10%10+'0';
			temp[2]=number%10+'0';
			temp[3]='\0';
		}
	else if((999<number)&&(number<10000))
		{
			temp[0]=number/1000+'0';
			temp[1]=number/100%10+'0';
			temp[2]=number/10%10+'0';
			temp[3]=number%10+'0';
			temp[4]='\0';
		}
	else 
	{
		temp[0]=number/10000+'0';
		temp[1]=number/1000%10+'0';
		temp[2]=number/100%10+'0';
		temp[3]=number/10%10+'0';
		temp[4]=number%10+'0';
		temp[5]='\0';
	}
}

/*****************�������id��pm��hcho��temperature��wet��eletricity��ȡhttp***************/
void httpget(u16 id,u16 pm,u16 hcho,u16 temperature,u16 wet,u16 eletricity)
{ 
	u8 temp[6];
	u8 i=0;
	char* p;
	char *q=ass1;
	p=ass+len;
//��Ӳ���id	
	change(temp,id);
	while(temp[i]!='\0')
	{
		*p=temp[i];
		 p++;
		 i+=1;
	}
	while(*q!='\0')
	{
		*p=*q;
		p++;
		q++;
	}
//��Ӳ���pm	
	i=0;
	change(temp,pm);
	while(temp[i]!='\0')
	{
		*p=temp[i];
		 p++;
		 i++;
	}
	q++;
	while(*q!='\0')
	{
		*p=*q;
		p++;
		q++;
	}
//��Ӳ���hcho
	i=0;
	change(temp,hcho);
	while(temp[i]!='\0')
	{
		*p=temp[i];
		 p++;
		 i++;
	}
	q++;
	while(*q!='\0')
	{
		*p=*q;
		p++;
		q++;
	}
//��Ӳ���temperature
	i=0;
	change(temp,temperature);
	while(temp[i]!='\0')
	{
		*p=temp[i];
		 p++;
		i++;
	}
	q++;
	while(*q!='\0')
	{
		*p=*q;
		p++;
		q++;
	}
//��Ӳ���wet	
	i=0;
	change(temp,wet);
	while(temp[i]!='\0')
	{
		*p=temp[i];
		 p++;
		i++;
	}
	q++;
	while(*q!='\0')
	{
		*p=*q;
		p++;
		q++;
	}
//��Ӳ���eletricity
	i=0;
	change(temp,eletricity);
	while(temp[i]!='\0')
	{
		*p=temp[i];
		 p++;
		i++;
	}
	q++;
	while(*q!='\0')
	{
		*p=*q;
		p++;
		q++;
	}
//��ӡ�\"��	
	q++;
	while(*q!='\0')
	{
		*p=*q;
		p++;
		q++;
	}	
	
}


//void shownumber(u16 data)
//{ 
//	int i;
//	char a[6];
//	for( i=6;i>0;i--)
//	{
//		a[i]=data%10+'0';
//		data=data/10;
//	}
//		for(i=1;i<7;i++)
//	{
//	  USART_SendData(USART1,a[i] );
//		while((USART1->SR&0X40)==0);//�ȴ����ͽ�
//	}
//	
//printf("\n");
//}

//void display(void)
//{
//  u8 t=1;	
//	for(t=1;t<18;t++)
//				{
//					switch(t)
//					{ 
//							//case 1:printf ("  PM1.0:%dug/m3 \r\n   ",Get_data(t));break;
//							//case 2:printf ("PM2.5: %dug/m3 \r\n  ",Get_data(t));break;
//							//case 3:printf ("PM10:  %dug/m3 \r\n",Get_data(t));break;
//							//case 4:printf (" PM1.0:%dug/m3 \r\n",Get_data(t));break;
//						case 6:printf ("PM2.5: %d ug/m3 \r\n",Get_data(t));break;
//							//case 6:printf (" PM10:  %dug/m3 \r\n",Get_data(t));break;
//							//	case 7:printf ("%d   ",Get_data(t));break;
//							//case 8:printf ("%d   ",Get_data(t));break;
//							//case 9:printf ("%d   ",Get_data(t));break;
//							//case 10:printf ("%d   ",Get_data(t));break;
//							//case 11:printf ("%d   ",Get_data(t));break;
//							//case 12:printf ("%d   \n",Get_data(t));break;
//						case 14:printf ("��ȩŨ����ֵ: %.3f mg/m3 \r\n ",(float)Get_data(t)/1000);break;
//						case 15:printf ("�¶�ֵ: %.1f'C  \r\n ",(float)Get_data(t)/10);break;
//						case 16:printf ("ʪ��ֵ: %.1f   \r\n ",(float)Get_data(t)/10);break;
//							//case 16:printf ("%d  \n ",Get_data(t));break;
//							//case 17:printf ("%d   ",Get_data(t));break;
//						
//						
//					}
//							
//				}
//	printf("\r\n");
//}

//void lcd_display()
//{
//				LCD_ShowString(60,150,200,16,16,"Temp:   . C");
//				LCD_ShowNum(60+40+8,150,Get_data(15)/10,2,16);	//��ʾ��������	    
//				LCD_ShowNum(60+40+32,150,Get_data(15)%10,1,16);	//��ʾС������ 		
//				
//				LCD_ShowString(60,150+16,200,16,16,"PM2.5:  ug/m3");
//				LCD_ShowNum(60+40+8,150+16,Get_data(6),2,16);	//��ʾ��������	    
//				
//				LCD_ShowString(60,150+32,200,16,16,"wet:  . %");
//				LCD_ShowNum(60+32,150+32,Get_data(16)/10,2,16);	//��ʾ��������
//				LCD_ShowNum(60+32+24,150+32,Get_data(16)%10,1,16);	//��ʾС������ 
//			  
//				httpget(ID,Get_data(PM),Get_data(HCHO),Get_data(TEMPERATURE),Get_data(WET),60);
//				LCD_ShowString(0,10,200,80,16,ass);
//			  

//}
void httpsend(void )
{
	httpget(ID,Get_data(PM),Get_data(HCHO),Get_data(TEMPERATURE),Get_data(WET),100);
	sim800c_send_cmd(ass,"OK",100);
	while(sim800c_send_cmd("AT+HTTPACTION=0", "200",1000));
}



int main()
{
	SystemInit ();    		 					//��ϵͳʱ�ӳ�ʼ��Ϊ72M
	delay_init();	    		 				//��ʱ������ʼ��	
	NVIC_Configuration();					// �����ж����ȼ�����
	uart1_init(9600);	 	 					//����USART1��ʼ��Ϊ9600
	uart4_init(9600);   				 //����UART4��ʼ��Ϊ9600
	SPI_Flash_Init();        //��ʼ��Flash;	 	
	SPI1_Init();	//SPI1��ʼ��
	LCD_Init();	
  show_init();
	gprs_init();       					//gprs ��ʼ��
	len=strlen(ass);        	 //���㷢���ַ��ĳ���
//	LCD_Init();
//	POINT_COLOR=RED;//��������Ϊ��ɫ
	//printf("��ʼ�����\n");
	LCD_ShowString(90,0,120,12,12,"Init");
	delay_ms(500);
	while(1)
	{		
		show(Get_data(PM),Get_data(HCHO),Get_data(TEMPERATURE),Get_data(WET));
		if(USART_RX_STA&0x0100)
			{   
				LCD_ShowString(90,0,120,12,12,"Send...");
				//printf("׼������\n");
				  httpsend();
//				sim800c_send_cmd("AT+HTTPPARA=\"URL\",\"www.baidu.com\"","OK",100);
//				while(sim800c_send_cmd("AT+HTTPACTION=0", "200",10000));
				//printf("���ͳɹ�\n");
				LCD_ShowString(90,0,120,12,12,"Finish ");
				
				USART_RX_STA=0;
				USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	
			
			
			}	
			

	


		

			

	}

}














