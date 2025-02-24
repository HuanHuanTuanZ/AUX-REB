
#include "iic.h"
#include "delay.h"

#define	SlaveAddress   0x3C	  //定义器件在IIC总线中的从地址


u8 QMC_buf[8];


void Single_Write_HMC5883(u8 REG_Address,u8 REG_data)
{
    IIC_Start();                  //起始信号
    IIC_Send_Byte(SlaveAddress);   //发送设备地址+写信号
    IIC_Send_Byte(REG_Address);    //内部寄存器地址，请参考中文pdf 
    IIC_Send_Byte(REG_data);       //内部寄存器数据，请参考中文pdf
    IIC_Stop();                   //发送停止信号
}
u8 HMC5883_RecvByte()
{
    u8 i;
    u8 dat = 0;
		SDA_OUT();
    IIC_SDA = 1;                    //使能内部上拉,准备读取数据,
		SDA_IN();
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;
        IIC_SCL = 1;                //拉高时钟线
        delay_us(5);             //延时
        dat |= READ_SDA;             //读数据               
        IIC_SCL = 0;                //拉低时钟线
        delay_us(5);             //延时
    }
    return dat;
}

/**************************************
发送应答信号
入口参数:ack (0:ACK 1:NAK)
**************************************/
void HMC5883_SendACK(u8 ack)
{
		SDA_OUT();
    IIC_SDA = ack;                  //写应答信号
    IIC_SCL = 1;                    //拉高时钟线
    delay_us(5);                 //延时
    IIC_SCL = 0;                    //拉低时钟线
    delay_us(5);                      //延时
}


void Multiple_Read_HMC5883(void)
{   u8 i;
    IIC_Start();                          //起始信号
    IIC_Send_Byte(SlaveAddress);           //发送设备地址+写信号
    IIC_Send_Byte(0x03);                   //发送存储单元地址，从0x3开始	
    IIC_Start();                          //起始信号
    IIC_Send_Byte(SlaveAddress+1);         //发送设备地址+读信号
	 for (i=0; i<6; i++)                      //连续读取6个地址数据，存储中BUF
    {
        QMC_buf[i] = HMC5883_RecvByte();          //BUF[0]存储数据
       
				if (i == 5)
        {
           HMC5883_SendACK(1);                //最后一个数据需要回NOACK
        }
        else
        {
          HMC5883_SendACK(0);                //回应ACK
       }
   }
    IIC_Stop();                          //停止信号
    delay_ms(3);
}


void Init_HMC5883()
{
     Single_Write_HMC5883(0x02,0x00);  //
}

