/**
  ********************************************************************
  *
  * @file     serial.c
  * @author   fpack
  * @version  v1.0
  * @date     2014-9-1
  * @brief    LED control
  *
  ********************************************************************
  **/

#include "armsis.h"
#include "serial.h"

void init(void)
{
    // 使用外部8M晶振，启用PPL，系统时钟为48MHz，总线时钟12HMz
    RCC->CFGR = RCC_CFGR_SW_PLL |       // 启用PPL
                RCC_CFGR_PLLSRC |       // 使用PPL作为时钟源
                RCC_CFGR_PLLMULL6 |     // 6倍频，系统时钟 48Mhz
                RCC_CFGR_HPRE_DIV4 |    // 预分频，总线时钟 12MHz    
                RCC_CFGR_PPRE1_DIV1 |   // 低速总线 12MHz
                RCC_CFGR_PPRE2_DIV1 |   // 高速总线 12MHz
                RCC_CFGR_USBPRE;        // USB直接使用PPL倍频时钟 48MHz

    // 开启PPL和外部时钟
    RCC->CR =   RCC_CR_PLLON | 
                RCC_CR_HSEON;
                
    // 闪存访问延迟，48MHz ~ 72MHz = 010。 复位值：0x30
    FLASH->ACR =    0x1U | // FLASH_ACR_LATENCY_1 ? |  // 01 Flash访问周期 (24Mhz ~ 48MHz] 范围
                    FLASH_ACR_PRFTBE | FLASH_ACR_PRFTBS;      // AHB时钟分频 != 1 时必须开启
                  //0x32;

    // 确定外部高速晶振起效
    while (!(RCC->CR & RCC_CR_HSERDY));
    // 确定PLL设置起效
    while (!(RCC->CR & RCC_CR_PLLRDY));
    // 确定PLL为系统时钟源
    while(!(RCC->CFGR & RCC_CFGR_SWS_PLL));
}

int main(void)
{
    init();
    serial_init();
    
    uint8_t ud = 'a';
    Delay_init();
    //TestPortB_Configuration();
    //USART1_Configuration();


    // 输出 a~z
    while (ud <= 'z') {
      while (!(USART1->SR & USART_SR_TXE));
      USART1->DR = ud;
      ud++;
    }
      
    //TestPortB_FlickerSignal();
      
    printf("\ntest!\n");
    printf("USART1 使能, 使能输出，使能输入\n");
    
    ud = 'a';
    while (1){  
      //TestPortB_Flow();
      
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
