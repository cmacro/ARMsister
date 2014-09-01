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

int main(void)
{
	uint16_t d = 0x0f00;
	uint8_t i = 0x0;

  //SysTick_Configuration();
  Delay_init();

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
  
	GPIOB->ODR = d;
  while (1){
		Delay(5);
		d = d << 1 | d >> 15;
		GPIOB->ODR = d;
  };   	
  
}
