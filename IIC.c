#include "IIC.h"
#include "stm32f1xx_hal.h"

void delay_us(uint32_t t) {
	/* counter reset to 0x00, TOF is cleared*/
	uint32_t i;
	t*=100;
	for (i = 0; i < t; i++) {
		__nop();__nop();__nop();__nop();
	}
}

void IIC_Init(void) 
{
 
}

void IIC_SCL(uint8_t val) {
	if (val) {
		SCL_PORT->ODR |= (SCL_PIN);
	} else {
		SCL_PORT->ODR &= ~(SCL_PIN);
	}

}
void IIC_SDA(uint8_t val) {
	if (val) {
		SDA_PORT->ODR |= (SDA_PIN);
	} else {
		SDA_PORT->ODR &= ~(SDA_PIN);
	}
}
uint8_t READ_SDA() {

	return ((uint8_t) ((SDA_PORT->IDR & SDA_PIN)?1:0));
}

//IIC启动
//描述：无
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA(1);
	delay_us(2);
	IIC_SCL(1);
	delay_us(2);
 	IIC_SDA(0); //START:when CLK is high,DATA change from high to low 
	delay_us(2);
	IIC_SCL(0); //钳住I2C总线，准备发送或接收数据 
	delay_us(2);
}	  

//IIC结束
//描述:无
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SDA(0);//STOP:when CLK is high DATA change form low to high
 	delay_us(2);
	IIC_SCL(1);
	delay_us(2);
	IIC_SDA(1);//发送I2C总线结束信号
	delay_us(2);
	IIC_SCL(0);
	delay_us(2);
}

//主设备发出应答
//描述：主设备(比如单片机)，从从设备读取字节后，如果要继续读取，
//      就要给从设备一个ACK(即所谓的“应答”，数据位SDA为0)
void IIC_Ack(void)
{
	SDA_OUT();
	IIC_SDA(0);
	delay_us(2);
	IIC_SCL(1);
	delay_us(2);
	IIC_SCL(0);
	delay_us(2);
}

//主设备发出无应答
//描述：主设备(比如单片机)，从从设备读取字节后，如果不再进行读取，
//      就要给从设备一个NACK(即所谓的“无应答”，数据位SDA为1)
void IIC_NAck(void)
{
	SDA_OUT();
	IIC_SDA(1);
	delay_us(2);
	IIC_SCL(1);
	delay_us(2);
	IIC_SCL(0);
	delay_us(2);
}

//主设备检测从设备应答
//返回：从设备的应答值
//描述：主设备(比如单片机)，向从设备写入字节后，要检测从设备发出的应答，
//      如果数据位SDA为0，说明向从设备成功写入了字节
uint8_t IIC_Get_Ack(void)
{
	uint8_t ucErr=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA(1);
	delay_us(2);	   
	IIC_SCL(1);
	delay_us(2);	 
	ucErr = READ_SDA();
	delay_us(2);
	IIC_SCL(0);
	delay_us(2);	
	return ucErr;  
} 		
		 				     
//主设备向从设备写入一字节
//参数：txd:将要写入的字节 
//返回：从设备的应答值
//描述：向从设备写入一个字节，并返回从设备的应答值
uint8_t IIC_Write_Byte(uint8_t txd)
{                        
  uint8_t t;   
	SDA_OUT(); //sda设置为输出	    
  IIC_SCL(0);//拉低时钟开始数据传输
  for(t=0;t<8;t++)
  {    
   if(txd&0x80)
	  IIC_SDA(1);
   else
	  IIC_SDA(0);
	 
   txd <<= 1;
	 
   delay_us(2);   //对TEA5767这三个延时都是必须的
   IIC_SCL(1);
   delay_us(2); 
   IIC_SCL(0);	
   delay_us(2);
  }
	
	return IIC_Get_Ack();	
} 	    

//主设备从从设备读取一字节
//返回：读到的字节
//描述：无
uint8_t IIC_Read_Byte(void)
{
	uint8_t i,receive=0;
	SDA_IN();//SDA设置为输入
  for(i=0;i<8;i++ )
	{ 
	 IIC_SCL(1);
	 delay_us(2);
   if(READ_SDA()) receive|=(0x80>>i);;   
	 delay_us(2);
	 IIC_SCL(0);
	 delay_us(2);
  }					 
  return receive;
}

void IIC_Write_Reg(uint8_t daddr,uint8_t addr,uint8_t dat) 
{
	IIC_Start(); //启动IIC传输
	IIC_Write_Byte((daddr<<1)&0XFE); //R/W是读/写选择位，0为写操作，1为读操作
	IIC_Write_Byte(addr); //写入地址
	IIC_Write_Byte(dat); //写入一个字节的数据
	IIC_Stop(); //IIC传输结束
}

void IIC_Read_Reg(uint8_t daddr,uint8_t addr,uint32_t length,uint8_t *pbuf)
{
	unsigned char i;
	
	IIC_Start(); //启动传输，这里再次启动传输，因为下面要写入“读操作”的指令码
	IIC_Write_Byte(((daddr<<1)|0X01)); //读取数据
	for(i=0;i<length-1;i++)
	{
		pbuf[i]=IIC_Read_Byte(); //读取length-1个字节
		IIC_Ack();			   //前length-1个字节，每读完一个字节要向芯片提供ACK，即告诉其继续提供下面的数据，读取操作还未完毕
	}
	pbuf[i]=IIC_Read_Byte(); //读取最后一个字节，即第length个字节
	IIC_NAck();   //读取最后一个字节后，要向器件发出NACK，告诉其读取操作结束，不再向下读了
	IIC_Stop(); //传输结束  
}

