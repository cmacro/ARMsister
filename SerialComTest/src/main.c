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


static __IO uint16_t LedData = 0x0f00;


void TestPortB_Configuration(void)
{
  ///  
  ///  测试B组引脚
  ///    
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
  
	GPIOB->ODR = 0x0000;
  // 关闭JTAG调试，空出 PB3 PB4
  AFIO->MAPR &= ~AFIO_MAPR_SWJ_CFG;
	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
  // 设置2MHz 推挽输出
	GPIOB->CRL = GPIO_CR_OUT_PP2MHz;
	GPIOB->CRH = GPIO_CR_OUT_PP2MHz;  
  
}

void TestPortB_FlickerSignal(void)
{
  uint8_t i = 0x0;
  /// 延迟，确认引脚情况。是否有LED灯被打开的情况。
  Delay(50);
  
  ///  
  ///  点亮LED作为引脚测试
  ///  
  ///    0 --- 关闭
  ///    1 --- 打开
  ///
  ///  引脚全部拉低，关闭所有LED。
	GPIOB->ODR = 0x0000;
  /// 启动流水灯
	for (i = 0; i < 10; i++){
		GPIOB->BSRR = 0xffff;
		Delay(20);
		GPIOB->BRR = 0xffff;
		Delay(20);
	}  
}

void TestPortB_Flow()
{
		Delay(5);
		LedData = LedData << 1 | LedData >> 15;
		GPIOB->ODR = LedData;  
}

int main(void)
{
  uint8_t ud = 'a';
  Delay_init();
  TestPortB_Configuration();
  serial_init();
   
  
  // 输出 a~z
  while (ud <= 'z') {
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = ud;
    ud++;
  }
    
  TestPortB_FlickerSignal();
    
  printf("\ntest!\n");
  printf("USART1 使能, 使能输出，使能输入\n");
  
  ud = 'a';
  while (1){  
    TestPortB_Flow();
    
    Delay(20);
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = ud;
    //ud++;    
    
//    if (ud > 'z') {
//      ud = 'a';
//      printf("\n");
//    }
  };   	
  
}
