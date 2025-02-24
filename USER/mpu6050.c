#include "sys.h"
#include "delay.h"
#include "usart.h"  
#include "mpu6050.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//MPU6050 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/9
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//初始化MPU6050
//返回值:0,成功
//    其他,错误代码
//****************************************
// ??MPU6050????



typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;


//IO操作函数	 
#define MPU_IIC_SCL    PAout(2) //SCL
#define MPU_IIC_SDA    PAout(3) //SDA	 
#define MPU_READ_SDA   PAin(3)  //输入SDA 


//初始化IIC
void MPU_SDA_IN(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void MPU_SDA_OUT(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void Delay5us()
{ 
	u16 n=165;
	while(n--);	
}
void I2C_SendACK(u8 ack)
{
	MPU_SDA_OUT();
	MPU_IIC_SDA = ack;                  //?????
	MPU_IIC_SCL = 1;                    //?????
	Delay5us();                 //??
	MPU_IIC_SCL = 0;                    //?????
	Delay5us();                 //??
}
//**************************************
//I2C??????
//**************************************
u8 I2C_RecvACK()
{
	u8 i;
	MPU_IIC_SCL = 1;
	MPU_SDA_IN();
	Delay5us();                 //??
	i = MPU_READ_SDA;                   //?????
	MPU_IIC_SCL = 0;                    //?????
	Delay5us();                 //??
	return i;
}

 

void I2C_SendByte(uchar txd)
{
  u8 t;   
	MPU_SDA_OUT(); 	    
  MPU_IIC_SCL=0;//拉低时钟开始数据传输
  for(t=0;t<8;t++)
  {              
		MPU_IIC_SDA=(txd&0x80)>>7;
		txd<<=1; 	  
		MPU_IIC_SCL=1;
		Delay5us();    
		MPU_IIC_SCL=0;	
		Delay5us();    
  }	
}
uchar I2C_RecvByte()
{
	u8 i,receive=0;
	MPU_SDA_IN();
  for(i=0;i<8;i++ )
	{
    MPU_IIC_SCL=0; 
    Delay5us();   
		MPU_IIC_SCL=1;
    receive<<=1;
    if(MPU_READ_SDA)receive++;   
		Delay5us();   
  }					 
  return receive;
}
void I2C_Stop()
{
	MPU_SDA_OUT(); 	 
	MPU_IIC_SDA = 0;                    //?????
	MPU_IIC_SCL = 1;                    //?????
	Delay5us();                 //??
	MPU_IIC_SDA = 1;                    //?????
	Delay5us();                 //??
}
void I2C_Start()
{
	MPU_SDA_OUT(); 	 
	MPU_IIC_SDA = 1;                    //?????
	MPU_IIC_SCL = 1;                    //?????
	Delay5us();                 //??
	MPU_IIC_SDA = 0;                    //?????
	Delay5us();                 //??
	MPU_IIC_SCL = 0;                    //?????
}

u8 MPU_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	MPU_SDA_IN();      //SDA设置为输入  
	MPU_IIC_SDA=1;Delay5us();   
	MPU_IIC_SCL=1;Delay5us();   
	while(MPU_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			I2C_Stop();
			return 1;
		}
	}
	MPU_IIC_SCL=0;//时钟输出0 	   
	return 0;  
}

u8 Single_WriteI2C(uchar REG_Address,uchar REG_data)
{
	I2C_Start();                  //????
	I2C_SendByte(SlaveAddress);   //??????+???
	if(MPU_IIC_Wait_Ack())	//等待应答
	{
		I2C_Stop();		 
		return 1;		
	}
	I2C_SendByte(REG_Address);    //???????,
	MPU_IIC_Wait_Ack();		//等待应答 
	I2C_SendByte(REG_data);       //???????,
	if(MPU_IIC_Wait_Ack())	//等待ACK
	{
		I2C_Stop();   
		return 1;		 
	}	
	I2C_Stop();  
  return 0;		//??????
}


uchar Single_ReadI2C(uchar REG_Address)
{
	uchar REG_data;
	I2C_Start();                   //????
	I2C_SendByte(SlaveAddress);    //??????+???
	MPU_IIC_Wait_Ack();		//等待应答 
	I2C_SendByte(REG_Address);     //????????,?0??	
	MPU_IIC_Wait_Ack();		//等待应答
	I2C_Start();                   //????
	I2C_SendByte(SlaveAddress+1);  //??????+???
	MPU_IIC_Wait_Ack();		//等待应答
	REG_data=I2C_RecvByte();       //???????
	I2C_SendACK(0);                //??????
	I2C_Stop();                    //????
	return REG_data;
}

int GetData(u8 REG_Address)
{
	uchar H,L;
	short int temp;
	H=Single_ReadI2C(REG_Address);
	L=Single_ReadI2C(REG_Address+1);
	temp=(H<<8)+L;
	if(temp>18000)
	{
		return temp-65535;
	}
	else return temp;   //????
}

void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );	//使能GPIOB时钟
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_2|GPIO_Pin_3); 	//PA2,PA3 输出高
}



void MPU6050_Init(void)
{ 
	IIC_Init();
	MPU_IIC_SCL=1;
	MPU_IIC_SDA=1;
	Single_WriteI2C(PWR_MGMT_1, 0x00);	//??????
	Single_WriteI2C(SMPLRT_DIV, 0x07);
	Single_WriteI2C(MPU_CONFIG, 0x06);
	Single_WriteI2C(GYRO_CONFIG, 0x18);
	Single_WriteI2C(ACCEL_CONFIG, 0x01);
}



