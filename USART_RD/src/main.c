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

/*----------------------------------------------------------------------------
 Define  Baudrate setting (BRR) for USART
 *----------------------------------------------------------------------------*/
#define __DIV(__PCLK, __BAUD)       ((__PCLK*25)/(4*__BAUD))
#define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
#define __DIVFRAQ(__PCLK, __BAUD)   (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) * 16 + 50) / 100)
#define __USART_BRR(__PCLK, __BAUD) ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))


static __IO uint16_t LedData = 0x0f00;

//struct __FILE { int handle; /* Add whatever you need here */ };
//FILE __stdout;
//FILE __stdin;

int fputc(int ch, FILE *f)
{
  while (!(USART1->SR & USART_SR_TXE));
  USART1->DR = (ch & 0x1FF);
  
  return (ch);
}

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

void USART1_Configuration(void)
{
  //
  //  设置串口调试
  //   
  //     输  出： USART1
  //     引  脚： PA9(TX), PA10(RX)
  //     波特率： 9600
  //     数据位： 8 bit (default)  (CR1)
  //     校  验： none  (default)  (CR1)
  //     停止位： 1 bit (default)  (CR2)
  //     流控制： none  (default)  (CR3)
  //
  // 清除设置后上面配置为系统默认状态
  
  //int i;
  /// 使能复用功能，使能GPIOA，使能USART1
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;
  // 关闭映射，确保USART使用 PA9，PA10
  AFIO->MAPR &= ~AFIO_MAPR_USART1_REMAP;
  
  // 清除PA9，PA10状态
  GPIOA->CRH &= ~( GPIO_CRH_CNF9 | GPIO_CRH_MODE9 | 
                   GPIO_CRH_CNF10 | GPIO_CRH_MODE10 );
  // 设置PA9 发送 为复用推挽输出 2MHz
  GPIOA->CRH |= GPIO_CR_AFOUT_PP2MHz & ( GPIO_CRH_CNF9 | GPIO_CRH_MODE9 );
  // 设置PA10接收 为复用上拉下拉模式
  GPIOA->CRH |= GPIO_CR_AFIN_PULLDOWN & ( GPIO_CRH_CNF10 | GPIO_CRH_MODE10 );
  

  // 设置波特率为 9600
  // 计算方法
  //          系统时钟 / (16分频 * 波特率)
  //   Baud = 72,000,000 / (16 * 9600) = 468.75
  //   整数部分 << 4 + 取整(小数部分 * 16)
  //   468 << 4 + 0.75 * 16
  USART1->BRR = __USART_BRR(SystemCoreClock, 9600);
  
  // 清除寄存器状态
  USART1->CR1 = USART_CR1_REST;
  USART1->CR2 = USART_CR2_REST;           // 停止位 1
  USART1->CR3 = USART_CR3_REST;           // 没用控制流
  
  // 防止产生不必要的信息
  //for (i = 0; i < 0x1000; i++) __NOP();
  
  // USART1 使能, 使能输出，使能输入
  USART1->CR1 =  USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
  
}

int main(void)
{
  uint8_t ud = 'a';
  Delay_init();
  TestPortB_Configuration();
  USART1_Configuration();
   
  
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
    ud++;    
    
    if (ud > 'z') {
      ud = 'a';
      printf("\n");
    }
  };   	
  
}
