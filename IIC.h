/*
 * IIC.h
 *
 *  Created on: Jul 24, 2022
 *      Author: emisario
 */

#ifndef INC_IIC_H_
#define INC_IIC_H_

#include "main.h"
#include "IIC.h"

#define SCL_PIN		GPIO_PIN_6
#define SCL_PORT	GPIOB

#define SDA_PIN 	GPIO_PIN_7
#define SDA_PORT	GPIOB

#define SDA_IN()  {SDA_PORT->CRL&=(0X0FFFFFFF);SDA_PORT->CRL|=(0X40000000);}
#define SDA_OUT() {SDA_PORT->CRL&=(0X0FFFFFFF);SDA_PORT->CRL|=(0X10000000);}

//#define IIC_SCL(x) (x?(SCL_PORT->ODR|=SCL_PIN):(SCL_PORT->ODR&(~SCL_PIN)))
//#define IIC_SDA(x) (x?(SDA_PORT->ODR|=SDA_PIN):(SDA_PORT->ODR&(~SDA_PIN)))

void IIC_SCL(uint8_t val);
void IIC_SDA(uint8_t val);

void IIC_Init(void);              			 
void IIC_Start(void);			
void IIC_Stop(void);	  		
void IIC_Ack(void);				
void IIC_NAck(void);			
uint8_t IIC_Get_Ack(void); 			
uint8_t IIC_Write_Byte(uint8_t txd);		
void IIC_Write_Reg(uint8_t daddr,uint8_t addr,uint8_t dat) ;
void IIC_Read_Reg(uint8_t daddr,uint8_t addr,uint32_t length,uint8_t *pbuf);
#endif /* INC_IIC_H_ */
