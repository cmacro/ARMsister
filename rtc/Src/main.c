
#include "stm32f1xx.h"

#define IsLeapYear(y) ((y % 4 == 0) && ((y % 100 != 0) || (y % 400 == 0)))   

void SystemInit(void)
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

int8_t RCT_init(void)
{
  int16_t temp = 0;
    
  //if (!(BKP->DR1 & 0x1)) { 
    
    // 开启 APB1总线和后备时钟
    RCC->APB1ENR |= RCC_APB1ENR_PWREN |
                    RCC_APB1ENR_BKPEN;
    
    // 取消写保护
    PWR->CR |= PWR_CR_DBP;
    
    // 复位备份区
    RCC->BDCR |= RCC_BDCR_BDRST;
    RCC->BDCR &= ~RCC_BDCR_BDRST;
    // 启用外部晶振
    RCC->BDCR |= RCC_BDCR_LSEON;

    // 等待外部晶振
    while(!(RCC->BDCR & RCC_BDCR_LSERDY) && (temp < INT16_MAX)) temp++;
    if (temp == INT16_MAX) return 1;
    
    // 选择外部晶振并且启动RTC
    RCC->BDCR |= RCC_BDCR_RTCSEL_LSE |
                 RCC_BDCR_RTCEN; 
    
    while (!(RTC->CRL & RTC_CRL_RTOFF));  // 等待RTC寄存器操作完成
    while (!(RTC->CRL & RTC_CRL_RSF));    // 等待RTC寄存器同步

    RTC->CRH |= RTC_CRH_SECIE;            // 启用秒中断
    while (!(RTC->CRL & RTC_CRL_RTOFF));  // 等待RTC寄存器操作完成
    
    // 设置时钟
    RTC->CRL |= RTC_CRL_CNF;              // 允许配置
    
    RTC->PRLH = 0X0000;
    RTC->PRLL = 32767;
    
    RTC->CRL &= ~RTC_CRL_CNF;             // 更新设置            
    while (!(RTC->CRL & RTC_CRL_RTOFF)); 
    
    BKP->DR1 |= 0x1;  // 标示有配置
  //}
  //else {
  //  while (!(RTC->CRL & RTC_CRL_RSF));
  //  
  //  RTC->CRH |= RTC_CRH_SECIE;  // 启用秒中断
  //  while (!(RTC->CRL & RTC_CRL_RTOFF));  // 等待RTC寄存器操作完成
  //}
  
  return 0;
}

// 日期转秒
const uint8_t mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31}; 
uint8_t RCT_Set(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
  uint16_t t;
  uint32_t seccount = 0;
  
  if(syear < 1970 || syear > 2099) return 1;
  
  for(t = 1970; t < syear; t++) 
  {
    // 处理闰年
    if (IsLeapYear(t))
      seccount += 31622400;  
    else 
      seccount += 31536000; 
  }
  smon -= 1;
  
  for(t = 0; t < smon; t++) 
  {
    seccount += (uint32_t)mon_table[t] * 86400;
    //闰年2月加一天的秒钟数    
    if(IsLeapYear(syear) && t == 1)
      seccount += 86400;
  }
  seccount += (uint32_t)(sday-1) * 86400; 
  seccount += (uint32_t)hour * 3600; 
  seccount += (uint32_t)min * 60; 
  seccount += sec; 
  

  PWR->CR |= PWR_CR_DBP;    //取消备份区写保护
  RTC->CRL |= RTC_CRL_CNF;  //允许配置
  
  RTC->CNTL = seccount & 0xffff;
  RTC->CNTH = seccount >> 16;

  RTC->CRL &= ~RTC_CRL_CNF;  //配置更新
  while (!(RTC->CRL & RTC_CRL_RTOFF));  //等待 RTC 寄存器操作完成
  
  return 0;
}

int main(void)
{
  RCT_init();
  RCT_Set(2018, 4, 3, 22, 45, 0);
  
  while (1)
  {

  }
 

}

