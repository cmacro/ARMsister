
#include "rtc.h"

#define IsLeapYear(y)       ((y % 4 == 0) && ((y % 100 != 0) || (y % 400 == 0)))
#define RCT_WaitRtOff()     while (!(RTC->CRL & RTC_CRL_RTOFF)) {__nop();__nop();__nop();__nop();__nop();}


uint8_t RTC_init(void)
{
    //检查是不是第一次配置时钟
    uint16_t temp = 0;

    // 开启 APB1总线和后备时钟
    RCC->APB1ENR |= RCC_APB1ENR_PWREN |
                    RCC_APB1ENR_BKPEN; 

    // 取消写保护
    // 复位备份区
    // 启用外部晶振
    PWR->CR |= PWR_CR_DBP;
    RCC->BDCR |= RCC_BDCR_BDRST;
    RCC->BDCR &= ~RCC_BDCR_BDRST;
    RCC->BDCR |= RCC_BDCR_LSEON;
    
    // 等待外部晶振
    while (!(RCC->BDCR & RCC_BDCR_LSERDY) && (temp++ < 1000));

    // 选择外部晶振并且启动RTC
    RCC->BDCR |= RCC_BDCR_RTCSEL_LSE |
                 RCC_BDCR_RTCEN;


    RCT_WaitRtOff();
    while (!(RTC->CRL & RTC_CRL_RSF)); 

    // 启用秒中断
    RTC->CRH |= RTC_CRH_SECIE;
    RCT_WaitRtOff();

    // 设置时钟
    RTC->CRL |= RTC_CRL_CNF;
    RTC->PRLH = 0x0000;
    RTC->PRLL = 0x7FFF;     // 1秒中断 32.768 (32767 + 1)

    RTC_Set(2014, 3, 8, 22, 10, 55);    //设置时间

    RTC->CRL &= ~RTC_CRL_CNF;           // 更新设置
    RCT_WaitRtOff();                    //等待RTC寄存器操作完成

    return 0;
}

// 日期转秒
const uint8_t mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
uint32_t GetSecCount(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint16_t t;
    uint32_t seccount = 0;

    if (syear < 1970 || syear > 2099) return 1;

    for (t = 1970; t < syear; t++)
    {
        // 处理闰年
        if (IsLeapYear(t)) seccount += 31622400;
        else seccount += 31536000;
    }
    
    smon--; 
    for (t = 0; t < smon; t++)
    {
        seccount += (uint32_t)mon_table[t] * 86400;
        //闰年2月加一天的秒钟数
        if (IsLeapYear(syear) && t == 1)
            seccount += 86400;
    }
    seccount += (uint32_t)(sday - 1) * 86400;
    seccount += (uint32_t)hour * 3600;
    seccount += (uint32_t)min * 60;
    seccount += sec;

    return seccount;
}

uint8_t RTC_SetTickCount(uint32_t t) {
    
    PWR->CR |= PWR_CR_DBP;    //取消备份区写保护
    RTC->CRL |= RTC_CRL_CNF;  //允许配置

    RTC->CNTL = t & 0xffff;
    RTC->CNTH = t >> 16;

    RTC->CRL &= ~RTC_CRL_CNF;  //配置更新
    RCT_WaitRtOff();    
    return 0;
}

uint8_t RTC_Set(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{

    uint32_t seccount;
    seccount = GetSecCount(syear, smon, sday, hour, min, sec);
    return RTC_SetTickCount(seccount);
}

uint32_t RTC_GetTime(void) {
    uint32_t timecnt;

    timecnt = RTC->CNTH << 16 | RTC->CNTL;
    timecnt %= 86400;   // 只要时间就OK了

    return timecnt;
}

uint32_t RTC_UpTimeOf(uint8_t kind) {
    uint32_t t, d;
    
    t = RTC->CNTH << 16 | RTC->CNTL;
    d = 0;
    if (kind == TIMEKIND_HOUR) {
        d = (t % 86400) / 3600;
        t -= d * 3600;
        d = (d < 23) ? d + 1 : 0;
        d *= 3600;
    } 
    else {
        d = (t % 86400 % 3600) / 60;
        t -= d * 60;
        d = (d < 59) ? d + 1 : 0;
        d *= 60;
    }
    t += d; 
    
    RTC_SetTickCount(t);
    
    return t;
}


