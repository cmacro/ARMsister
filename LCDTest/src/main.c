/**
  ********************************************************************
  *
  * @file     main.c
  * @author   fpack
  * @version  v1.0
  * @date     2014-8-29
  * @brief    小穆妹纸
  *
  ********************************************************************
  **/

#include "armsis.h"
#include <stdio.h>


/// LCD control line defines
//    PA0    ---------->  CS
//    PA1    ---------->  REST
//    PA5    ---------->  RS
//    PA6    ---------->  RW
//    PA7    ---------->  RD

#define Set_CS                  GPIOA->BSRR = GPIO_BSRR_BS0
#define Set_RSET                GPIOA->BSRR = GPIO_BSRR_BS1
#define Set_RS                  GPIOA->BSRR = GPIO_BSRR_BS5
#define Set_WR                  GPIOA->BSRR = GPIO_BSRR_BS6
#define Set_RD                  GPIOA->BSRR = GPIO_BSRR_BS7

#define Clr_CS                  GPIOA->BSRR = GPIO_BSRR_BR0
#define Clr_RSET                GPIOA->BSRR = GPIO_BSRR_BR1
#define Clr_RS                  GPIOA->BSRR = GPIO_BSRR_BR5
#define Clr_WR                  GPIOA->BSRR = GPIO_BSRR_BR6
#define Clr_RD                  GPIOA->BSRR = GPIO_BSRR_BR7

#define SetDataInputMode        GPIOB->CRL = GPIO_CR_IN_UPDOWN; \
                                GPIOB->CRH = GPIO_CR_IN_UPDOWN; 
#define SetDataOutputMode       GPIOB->CRL = GPIO_CR_GPOUT_PP2MHz; \
                                GPIOB->CRH = GPIO_CR_GPOUT_PP2MHz; 
                                
                                

#define SetData(x)              GPIOB->ODR = x    // 设置端口数据
#define GetData                 GPIOB->IDR        // 获取端口数据

uint8_t   LCD_Succended   = 0x1;
uint16_t  LCD_DeviceCode  = 0x0;


//    底层函数
void LCD_CtrlLinesConfig(void)
{
  /**
  * LCD GPIO configuration
  *
    Control line
    PA0    ---------->  CS
    PA1    ---------->  REST
    PA5    ---------->  RS
    PA6    ---------->  RW
    PA7    ---------->  RD

    Data line
    PB00    ----------> DB0
    PB01    ----------> DB1
    PB02    ----------> DB2
    PB03    ----------> DB3
    PB04    ----------> DB4
    PB05    ----------> DB5
    PB06    ----------> DB6
    PB07    ----------> DB7
    PB08    ----------> DB10
    PB09    ----------> DB11
    PB10    ----------> DB12
    PB11    ----------> DB13
    PB12    ----------> DB14
    PB13    ----------> DB15
    PB14    ----------> DB16
    PB15    ----------> DB17
  **/    
  
  //
  // 打开控制线
  //   数据线  PB组端口(DB0 ~ DB17)  设置为2MHz开漏输出
  //   控制线  PA8 ~ PA12            设置为2MHz推挽输出
  //
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
  // 关闭JTAG-DP，释放 PB04、PB05端口
  AFIO->MAPR &= ~AFIO_MAPR_SWJ_CFG;
  AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
 
  //
  // 数据线 
  SetDataOutputMode;
  
  // 
  //  控制线 
  GPIOA->CRL &= ~ (GPIO_CRL_SET0 | 
                   GPIO_CRL_SET1 | 
                   GPIO_CRL_SET5 | 
                   GPIO_CRL_SET6 | 
                   GPIO_CRL_SET7);
  GPIOA->CRL |= GPIO_CR_GPOUT_PP2MHz & 
                (GPIO_CRL_SET0 |           // CS
                 GPIO_CRL_SET1 |           // RESET 
                 GPIO_CRL_SET5 |           // RS
                 GPIO_CRL_SET6 |           // RW
                 GPIO_CRL_SET7);           // RD 
                  
  GPIOB->ODR = 0xffff;
  Set_CS;
  Set_RS;
  Set_WR;
  Set_RD;
  Set_RSET;
  Delay(20);
}

//// 基础函数
void LCD_WriteReg(uint8_t reg, uint16_t val)
{
  // 读取寄存器
  //   16线模式
  //   
  SetDataOutputMode;
  
  Clr_CS;
  
  Clr_RS;
  Set_RD;
  //Delay_us(1);
  SetData(reg);
  //Delay_us(1);
  Clr_WR;
  Set_WR;
  Set_RS;
  
  Set_RS;
  Set_RD;
  //Delay_us(1);
  SetData(val);
  //Delay_us(1);
  Clr_WR;
  Set_WR;
  
  Set_CS;
}

uint16_t LCD_ReadReg(uint16_t reg)
{
  // 写寄存器
  //    16线模式
	uint16_t d;
  
  SetDataOutputMode;
  
	Clr_CS;
  
	Set_RS;
	Set_RD;
  SetData(reg);
  Clr_WR;
	Set_WR;
  Set_RS;
    
  SetDataInputMode;       // 设置输入模式
  Set_RD;
	Clr_RD;
	d = GetData;
  
  Set_CS;
  
	return d;  
}

uint16_t LCD_ReadDriverCode(void)
{
  uint16_t d;
  Delay(20);
  LCD_WriteReg(0x00, 0x0001);
  Delay(20);
  
  d = LCD_ReadReg(0x00);
  
  return d;
}

int main(void)
{
  uint8_t i;
  __IO uint16_t code = 0x0f00;
  
  Delay_init();
  serial_init();
  
  Delay(50);
  printf("********************\nserial test \n********************\n");
  Delay(50);
  
  LCD_CtrlLinesConfig();
  
  // 设置控制线  
  //LCD_CtrlLinesConfig();
  // 读取LCD驱动ID
  //code = ReadLCDDriverCode();
  
  // 显示
  while (1){  
    //code = LCD_ReadDriverCode();
    //printf("%x\n", code);
    //Delay(100);
		code = code << 1 | code >> 15;
		GPIOB->ODR = code;    
    Delay(10);
    
  };   	
  
}
