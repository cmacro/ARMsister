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

int main(void)
{
  uint8_t ud = 'a';
  Delay_init();
  serial_init();
  
  // 输出 a~z
  while (ud <= 'z') {
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = ud;
    ud++;
  }
    
  printf("\ntest!\n");
  printf("USART1 使能, 使能输出，使能输入\n");
  
  ud = 'a';
  while (1){  
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
