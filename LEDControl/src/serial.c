/**
  ********************************************************************
  *
  * @file     serial.c
  * @author   fpack
  * @version  v1.0
  * @date     2014-9-1
  * @brief    小穆妹纸串口调试
  *
  ********************************************************************
  **/


#include "serial.h"
#include "armsis.h"

/*----------------------------------------------------------------------------
 Define  Baudrate setting (BRR) for USART
 *----------------------------------------------------------------------------*/
#define __DIV(__PCLK, __BAUD)       ((__PCLK*25)/(4*__BAUD))
#define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
#define __DIVFRAQ(__PCLK, __BAUD)   (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) * 16 + 50) / 100)
#define __USART_BRR(__PCLK, __BAUD) ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))


//struct __FILE { int handle; /* Add whatever you need here */ };
//FILE __stdout;
//FILE __stdin;

int fputc(int ch, FILE *f)
{
  // 等待USART1 数据发送完成（发送区域空）
  while (!(USART1->SR & USART_SR_TXE));
  USART1->DR = (ch & 0x1FF);
  
  return (ch);
}


void serial_send(uint8_t d)
{
  while (!(USART1->SR & USART_SR_TXE));
  USART1->DR = (d & 0x1FF);  
}


void serial_init(void)
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
  
  int i;
  /// 使能复用功能，使能GPIOA，使能USART1
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;
  // 关闭映射，确保USART使用 PA9，PA10
  AFIO->MAPR &= ~AFIO_MAPR_USART1_REMAP;
  
  // 清除PA9，PA10状态
  GPIOA->CRH &= ~( GPIO_CRH_CNF9 | GPIO_CRH_MODE9 | 
                   GPIO_CRH_CNF10 | GPIO_CRH_MODE10);
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
  for (i = 0; i < 0x1000; i++) __NOP();
  
  // USART1 使能, 使能输出，使能输入
  USART1->CR1 =  USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}
