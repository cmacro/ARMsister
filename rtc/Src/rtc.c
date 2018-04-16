
#include "rtc.h"

#define IsLeapYear(y)       ((y % 4 == 0) && ((y % 100 != 0) || (y % 400 == 0))) 
#define RCT_WaitRtOff()     while (!(RTC->CRL & RTC_CRL_RTOFF))         // 等待RTC寄存器操作完成

int8_t RCT_init(void)
{
    int16_t temp = 0;

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
                 
    RCT_WaitRtOff();    // 等待RTC寄存器操作完成
    while (!(RTC->CRL & RTC_CRL_RSF));      // 等待RTC寄存器同步

    RTC->CRH |= RTC_CRH_SECIE;              // 启用秒中断
    RCT_WaitRtOff();    // 等待RTC寄存器操作完成
    
    // 设置时钟
    RTC->CRL |= RTC_CRL_CNF;                // 允许配置
    
    RTC->PRLH = 0x0000;
    RTC->PRLL = 0x8000;                      // 1秒中断 32.768
    
    RTC->CRL &= ~RTC_CRL_CNF;               // 更新设置            
    RCT_WaitRtOff();
  
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
