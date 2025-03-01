#include "NRF.h"
#include "delay.h"
#include "spi.h"
#include "usart.h"
#include "led.h"
extern u8 nrf_len;

const u8 TX_ADDRESS[TX_ADR_WIDTH]={'L','O','V','E','!'};//发送地址
const u8 RX_ADDRESS[RX_ADR_WIDTH]={'L','O','V','E','!'};
extern const char address_0[5];
extern u8 address[5];
extern u8 hopping[5];
//初始化24L01的IO口
void NRF24L01_Init(void)
{ 	
	GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	 //使能PB,G端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 	
	
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2;	//PB8  PB9 推挽 	  
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化指定IO
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PB3 输入  
	GPIO_Init(GPIOC, &GPIO_InitStructure); 

	
  SPI2_Init();    		//初始化SPI	 
	SPI_Cmd(SPI2, DISABLE); // SPI外设不使能
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//SPI主机
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//时钟悬空低
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//数据捕获于第1个时钟沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由软件控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;		//定义波特率预分频的值:波特率预分频值为16
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPI2, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	SPI_Cmd(SPI2, ENABLE); //使能SPI外设			 
	NRF24L01_CE=0; 			//使能24L01
	NRF24L01_CSN=1;			//SPI片选取消   		 	 
}

u8 NRF24L01_Check(void)
{
	u8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	u8 i;
	SPI2_SetSpeed(SPI_BaudRatePrescaler_8); //spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）   	 
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//写入5个字节的地址.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //读出写入的地址  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;//检测24L01错误	
	return 0;		 //检测到24L01
}	 	 

u8 NRF24L01_Write_Reg(u8 reg,u8 value)
{
	u8 status;	
	NRF24L01_CSN=0;                 //使能SPI传输
	status =SPI2_ReadWriteByte(reg);//发送寄存器号 
	SPI2_ReadWriteByte(value);      //写入寄存器的值
	NRF24L01_CSN=1;                 //禁止SPI传输	   
	return(status);       			//返回状态值
}

u8 NRF24L01_Read_Reg(u8 reg)
{
	u8 reg_val;	    
 	NRF24L01_CSN = 0;          //使能SPI传输		
	SPI2_ReadWriteByte(reg);   //发送寄存器号
	reg_val=SPI2_ReadWriteByte(0XFF);//读取寄存器内容
	NRF24L01_CSN = 1;          //禁止SPI传输		    
	return(reg_val);           //返回状态值
}	

u8 NRF24L01_Read_Buf(u8 reg,u8 *pBuf,u8 len)
{
	u8 status,u8_ctr;	       
	NRF24L01_CSN = 0;           //使能SPI传输
	status=SPI2_ReadWriteByte(reg);//发送寄存器值(位置),并读取状态值   	   
 	for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPI2_ReadWriteByte(0XFF);//读出数据
	NRF24L01_CSN=1;       //关闭SPI传输
	return status;        //返回读到的状态值
}

u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
	u8 status,u8_ctr;	    
 	NRF24L01_CSN = 0;          //使能SPI传输
	status = SPI2_ReadWriteByte(reg);//发送寄存器值(位置),并读取状态值
	for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI2_ReadWriteByte(*pBuf++); //写入数据	 
	NRF24L01_CSN = 1;       //关闭SPI传输
	return status;          //返回读到的状态值
}		

u8 TX(u8 *txbuf)
{
	u8 i=0;
	u8 sta;
 	SPI2_SetSpeed(SPI_BaudRatePrescaler_8);//spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）   
	NRF24L01_CE=0;
  NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,nrf_len);//写数据到TX BUF  32个字节
 	NRF24L01_CE=1;//启动发送	 + 
	while(NRF24L01_IRQ!=0)	
	{
		i++;
		if(i==255)break;
	}
	sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值	   
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志	
	if(sta&MAX_TX)//达到最大重发次数
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//清除TX FIFO寄存器 
		return MAX_TX; 
	}
	if(sta&TX_OK)//发送完成
	{
		return TX_OK;
	}
	return 0xff;//其他原因发送失败
}

u8 RX(u8 *rxbuf)
{
	u8 sta;		    							   
	SPI2_SetSpeed(SPI_BaudRatePrescaler_8); //spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）   
	sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值    	 
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta&RX_OK)//接收到数据
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,nrf_len);//读取数据
		NRF24L01_Write_Reg(FLUSH_RX,0xff);//清除RX FIFO寄存器 
		return 1; 
	}	   
	return 0;//没收到任何数据
}				


void TX_Address(u8 *p)
{
	NRF24L01_CE=0;	 
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,p,TX_ADR_WIDTH);//写TX节点地址 
  NRF24L01_CE = 1; 
}
void RX_Address(u8 *p)
{
	NRF24L01_CE=0;	 
	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,p,RX_ADR_WIDTH); //设置RX节点地址,主要为了使能ACK	  	
  NRF24L01_CE = 1; 
}


void NRF_power(u8 p,u8 s)				//发射功率设置
{
	u8 temp=0x01;
	if(p==3)temp|=0x06;
	else if(p==2)temp|=0x04;
	else if(p==1)temp|=0x02;
	else if(p==0)temp|=0x00;
	
	if(s==2)temp|=0x08;      //2Mbps
	else if(s==1)temp|=0x00; //1Mbps
	else if(s==0)temp|=0x28; //250Kbps
	
	NRF24L01_CE=0;
	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,temp);		  //0db 修正之前注释错误
	NRF24L01_CE=1;
}


void NRF_channel(u8 c)
{
	NRF24L01_CE=0;
	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,c);       //设置RF通道为40
	NRF24L01_CE=1;
}

void TX_Mode(void)
{														 
	NRF24L01_CE=0;	
	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
	NRF24L01_CE=1;//CE为高,10us后启动发送
}

void RX_Mode(void)
{
	NRF24L01_CE=0;	 
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
  NRF24L01_CE = 1; //CE为高,进入接收模式 
}		

void NRF_setup()
{
	NRF24L01_CE=0;	 
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(u8*)address,TX_ADR_WIDTH);//写TX节点地址 
	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)address,RX_ADR_WIDTH); //设置RX节点地址,主要为了使能ACK	  	
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0);    //使能通道0的自动应答    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,1);//使能通道0的接收地址  	 	  
	NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
  NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,nrf_len);//选择通道0的有效数据宽度 	    
 	//NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x07);//设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
	NRF_power(3,0)	;//发射功率设置
	RX_Mode();
	NRF_channel(hopping[0]);
  NRF24L01_CE = 1; //CE为高,进入接收模式 
}


