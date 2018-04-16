

#include "led.h"
#include <stddef.h> // needed for definition of NULL
#include <stdlib.h> // malloc

typedef struct _LedLum_t    LedLum_t;  
struct _LedLum_t {
    uint16_t        t;   // 时间
    uint16_t        v;    // 亮度
    LedLum_t        *next;
};

typedef struct {
    uint16_t        currlum;    // 当前亮度
    uint16_t        nextlum;    // 下个亮度 （倒计时完成后的亮度)
    uint32_t        remnant;    // 倒计时状态切换
    
    LedLum_t       *header;
    
} LedItem_t;

typedef struct {
    uint8_t     selectChannel;  // 当前选择的通道
    uint8_t     channelcount;   
    uint16_t    maxratio;       // 灯光总最大亮度
    uint16_t    updatestate;    // 是否需要刷新
    uint32_t    switchstate;    // 开关状态  00 -- auto   01 -- close   10 -- open
    LedItem_t   items[8];
} LedData_t;


static LedData_t   leddata;

#define time_arr          0x17 // 24(23+1)  12MHz / 24 = 500KHz(2u)
#define MaxChannel        7       // 最大led通道
#define MaxLum            0xF

#define Led_LumMaxRatio            leddata.maxratio

#define Led_TurnStateGet(c)     (leddata.updatestate & (0x1 << (c)))
#define Led_TuruStateSet(c)     (leddata.updatestate |= 0x1 << (c))
#define Led_TuruStateClr(c)     (leddata.updatestate &= ~(0x1 << (c)))

#define LED_SWITCHSTATE_AUTO    0X00
#define LED_SWITCHSTATE_ON      0x01
#define LED_SWITCHSTATE_OFF     0X10

#define Led_SwitchGet(c)        (leddata.switchstate >> (c << 1) & 0xFF)
#define Led_SwitchSet(c, s)     (leddata.switchstate |= (s) << (c << 1))
#define Led_ChannelTimeStep(c)  if (leddata.items[c].remnant) leddata.items[c].remnant-- 
void    Led_ChannelRemantCheck(uint8_t c);

#define Led_UpdateChannel(c)    if (Led_TurnStateGet(c)) { Led_TuruStateClr(c); Led_UpdatePWM(c); }
void    Led_UpdatePWM(uint8_t c);

void Led_TimeRemnantUpdate(uint8_t c, uint32_t t);
    

void _memset(void *s, uint8_t c,uint32_t count)
{
    uint8_t *d = s;
    while(count--) *d ++= c;
}


int8_t Led_init(void)           
{
    // 8路led，引脚对应
    //  
    // time3 
    // PA6   ---- LED 1 通道
    // PA7   ---- 2
    // PB0   ---- 3
    // PB1   ---- 4
    // TIME 4
    // PB6   ---- 5
    // PB7   ---- 6
    // PB8   ---- 7
    // PB9   ---- 8
    
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN | RCC_APB1ENR_TIM4EN;
    
    // 输出 2MHz 复用推挽
    // TIME3 PA6 PA7 PB0 PB1
    GPIOA->CRL &= ~(GPIO_CRL_MODE6_Msk | GPIO_CRL_CNF6_Msk);    // TIME3 PA6
    GPIOA->CRL |= GPIO_CRL_MODE6_1 | GPIO_CRL_CNF6_1;
    GPIOA->CRL &= ~(GPIO_CRL_MODE7_Msk | GPIO_CRL_CNF7_Msk);    // TIME3 PA7
    GPIOA->CRL |= GPIO_CRL_MODE7_1 | GPIO_CRL_CNF7_1;
    GPIOB->CRL &= ~(GPIO_CRL_MODE0_Msk | GPIO_CRL_CNF0_Msk);    // TIME3 PB0
    GPIOB->CRL |= GPIO_CRL_MODE0_1 | GPIO_CRL_CNF0_1; 
    GPIOB->CRL &= ~(GPIO_CRL_MODE1_Msk | GPIO_CRL_CNF1_Msk);    // TIME3 PB1
    GPIOB->CRL |= GPIO_CRL_MODE1_1 | GPIO_CRL_CNF1_1;
                  
    // Time4 2MHz 复用推挽
    GPIOB->CRL &= ~(GPIO_CRL_MODE6_Msk | GPIO_CRL_CNF6_Msk | 
                    GPIO_CRL_MODE7_Msk | GPIO_CRL_CNF7_Msk);
    GPIOB->CRH &= ~(GPIO_CRH_MODE8_Msk | GPIO_CRH_CNF8_Msk | 
                    GPIO_CRH_MODE9_Msk | GPIO_CRH_CNF9_Msk);
    GPIOB->CRL |= GPIO_CRL_MODE6_1 | GPIO_CRL_CNF6_1 |
                  GPIO_CRL_MODE7_1 | GPIO_CRL_CNF7_1;
    GPIOB->CRH |= GPIO_CRH_MODE8_1 | GPIO_CRH_CNF8_1 | 
                  GPIO_CRH_MODE9_1 | GPIO_CRH_CNF9_1; 
    
    //
    // TIME3 配置输出
    //   1、配置前关闭
    //   2、设置比对输出
    //   3、预装载使能  OC模式为PWM1
    //   4、预装载 0.5MHz  = 12000 / (23 + 1) = 500KHz 2u (1MHz = 1u)
    //   5、不分频
    //   6、使能CCER，使能CR 
    TIM3->CR1 = 0x0U;
    TIM3->CCER  = 0x0U; // 配置前关闭时钟     
    TIM3->CCMR1 = 0x0U; // 清除配置  
    TIM3->CCR1 = 0x0;
    TIM3->CCR2 = 0x0;
    TIM3->CCR3 = 0x0;
    TIM3->CCR4 = 0x0;
    
    TIM3->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_CC2S); 
    TIM3->CCMR2 &= ~(TIM_CCMR2_CC3S | TIM_CCMR2_CC4S);
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 |
                   TIM_CCMR1_OC2PE | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2; 
    TIM3->CCMR2 |= TIM_CCMR2_OC3PE | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 |
                   TIM_CCMR2_OC4PE | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2; 
    TIM3->ARR = time_arr; 
    TIM3->PSC = 0;  
    TIM3->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E |  TIM_CCER_CC3E | TIM_CCER_CC4E;
    TIM3->CR1 |= TIM_CR1_CEN | TIM_CR1_ARPE;
    
    //
    // TIME4 设置
    TIM4->CR1 = 0x0U;
    TIM4->CCER  = 0X0U; // 配置前关闭时钟     
    TIM4->CCMR1 = 0x0U; // 清除配置
    TIM4->CCR1 = 0x0;
    TIM4->CCR2 = 0x0;
    TIM4->CCR3 = 0x0;
    TIM4->CCR4 = 0x0;
    
    TIM4->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_CC2S); 
    TIM4->CCMR2 &= ~(TIM_CCMR2_CC3S | TIM_CCMR2_CC4S);
    TIM4->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 |
                   TIM_CCMR1_OC2PE | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2; 
    TIM4->CCMR2 |= TIM_CCMR2_OC3PE | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 |
                   TIM_CCMR2_OC4PE | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2; 
    TIM4->ARR = time_arr; 
    TIM4->PSC = 0;  
    TIM4->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E |  TIM_CCER_CC3E | TIM_CCER_CC4E;
    TIM4->CR1 |= TIM_CR1_CEN | TIM_CR1_ARPE;
    

    return 0;
}

void Led_LoadConfig(void) {
      
    // channel  color       
    //    1     Cool White
    //    2     Deep Blue
    //    3     Blue
    //    4     Green
    //    5     Photo red
    //    6     UV
    //    7     Violet
    //    8     Warm white
    leddata.maxratio = 100;
    leddata.channelcount = 8;
    
    // 1     Cool White
    Led_ChannelTimeSet(0, 10,  8 * 60);
    Led_ChannelTimeSet(0, 30,  8 * 60 + 10);
    Led_ChannelTimeSet(0, 60,  8 * 60 + 30);
    Led_ChannelTimeSet(0, 90,  9 * 60);
    Led_ChannelTimeSet(0, 100, 10 * 60);
    Led_ChannelTimeSet(0, 90, 15 * 60);
    Led_ChannelTimeSet(0, 80, 16 * 60);
    Led_ChannelTimeSet(0, 50, 16 * 60 + 30);
    Led_ChannelTimeSet(0, 20, 17 * 60);
    Led_ChannelTimeSet(0, 0,  17 * 60 + 50);
    
    //    2     Deep Blue
    Led_ChannelTimeSet(1, 10,  9 * 60);
    Led_ChannelTimeSet(1, 50,  9 * 60 + 30);
    Led_ChannelTimeSet(1, 80,  10 * 60);
    Led_ChannelTimeSet(1, 100, 11 * 60);
    Led_ChannelTimeSet(1, 90,  14 * 60 + 30);
    Led_ChannelTimeSet(1, 80,  15 * 60);
    Led_ChannelTimeSet(1, 50,  15 * 60 + 30);
    Led_ChannelTimeSet(1, 10,  16 * 60);
    Led_ChannelTimeSet(1, 0,   16 * 60 + 30);
    //    3     Blue
    Led_ChannelTimeSet(2, 10,  9 * 60);
    Led_ChannelTimeSet(2, 50,  9 * 60 + 30);
    Led_ChannelTimeSet(2, 80,  10 * 60);
    Led_ChannelTimeSet(2, 100, 11 * 60);
    Led_ChannelTimeSet(2, 90,  14 * 60 + 30);
    Led_ChannelTimeSet(2, 80,  15 * 60);
    Led_ChannelTimeSet(2, 50,  15 * 60 + 30);
    Led_ChannelTimeSet(2, 10,  16 * 60);
    Led_ChannelTimeSet(2, 0,   16 * 60 + 30);
    //    4     Green
    Led_ChannelTimeSet(3, 10,  11 * 60);
    Led_ChannelTimeSet(3, 80,  12 * 60);
    Led_ChannelTimeSet(3, 100, 13 * 60);
    Led_ChannelTimeSet(3, 80,  14 * 60);
    Led_ChannelTimeSet(3, 0,   15 * 60);
    //    5     Photo red
    Led_ChannelTimeSet(4, 10,  11 * 60);
    Led_ChannelTimeSet(4, 80,  12 * 60);
    Led_ChannelTimeSet(4, 100, 13 * 60);
    Led_ChannelTimeSet(4, 80,  14 * 60);
    Led_ChannelTimeSet(4, 0,   15 * 60);
    //    6     UV
    Led_ChannelTimeSet(5, 10,  11 * 60);
    Led_ChannelTimeSet(5, 80,  12 * 60);
    Led_ChannelTimeSet(5, 100, 13 * 60);
    Led_ChannelTimeSet(5, 80,  14 * 60);
    Led_ChannelTimeSet(5, 0,   15 * 60);
    //    7     Violet
    Led_ChannelTimeSet(6, 10,  11 * 60);
    Led_ChannelTimeSet(6, 80,  12 * 60);
    Led_ChannelTimeSet(6, 100, 13 * 60);
    Led_ChannelTimeSet(6, 80,  14 * 60);
    Led_ChannelTimeSet(6, 0,   15 * 60);    
    //    8     Warm white
    Led_ChannelTimeSet(7, 10,  7 * 60);
    Led_ChannelTimeSet(7, 30,  8 * 60 + 30);
    Led_ChannelTimeSet(7, 60,  8 * 60 + 50);
    Led_ChannelTimeSet(7, 90,  9 * 60 + 10);
    Led_ChannelTimeSet(7, 100, 10 * 60);
    Led_ChannelTimeSet(7, 90, 15 * 60);
    Led_ChannelTimeSet(7, 80, 16 * 60);
    Led_ChannelTimeSet(7, 50, 16 * 60 + 30);
    Led_ChannelTimeSet(7, 20, 17 * 60);
    Led_ChannelTimeSet(7, 0,  18 * 60 + 30);    
}

void Led_Update(uint32_t t) {
    
    Led_TimeRemnantUpdate(0, t);
    Led_TimeRemnantUpdate(1, t);
    Led_TimeRemnantUpdate(2, t);
    Led_TimeRemnantUpdate(3, t);
    Led_TimeRemnantUpdate(4, t);
    Led_TimeRemnantUpdate(5, t);
    Led_TimeRemnantUpdate(6, t);
    Led_TimeRemnantUpdate(7, t);

    Led_UpdateChannel(0);
    Led_UpdateChannel(1);
    Led_UpdateChannel(2);
    Led_UpdateChannel(3);
    Led_UpdateChannel(4);
    Led_UpdateChannel(5);
    Led_UpdateChannel(6);
    Led_UpdateChannel(7); 
}

void Led_UpdatePWM(uint8_t c)
{
    // 更新PWM分频值
    uint8_t level;
    
    if (Led_SwitchGet(c) & LED_SWITCHSTATE_ON)
        level = time_arr * 100 / Led_LumMaxRatio;
    else if (Led_SwitchGet(c) & LED_SWITCHSTATE_OFF) 
        level = 0;
    else {
        // 分频值计算
        //   可调级别  0 ~ 15
        //   每级分频值 = 当前分频 / 15级  
        //   当前级分频值 = 每档分频值 * 当前级
        //   
        //   分频值 = 每级分频 * 当前级 / 最大亮度百分比
        //  
        //   由于分频值比较小，优先所有值先乘后除
        level = time_arr * leddata.items[c].currlum * Led_LumMaxRatio / (100 * MaxLum);
    }

    
    switch (c) {
        case 0: TIM3->CCR1 = level; break;
        case 1: TIM3->CCR2 = level; break;
        case 2: TIM3->CCR3 = level; break;
        case 3: TIM3->CCR4 = level; break;
        case 4: TIM4->CCR1 = level; break;
        case 5: TIM4->CCR2 = level; break;
        case 6: TIM4->CCR3 = level; break;
        case 7: TIM4->CCR4 = level; break;
    }
}

void Led_TimeRemnantUpdate(uint8_t c, uint32_t t) {
    LedLum_t *p;

    if (leddata.items[c].remnant) 
        return;
    
    p = leddata.items[c].header;
    while (p && t >= p->t) { 
        p = p->next;
    }
    if (!p) p = leddata.items[c].header;
    
    if (p) {
        if (t <= p->t)
            leddata.items[c].remnant = p->t - t;
        else
            leddata.items[c].remnant = p->t + (24*60*60 - t); 
        
        leddata.items[c].currlum = leddata.items[c].nextlum;
        leddata.items[c].nextlum = p->v;
        Led_TuruStateSet(c);
    }
}

void Led_ChannelTimeSet(uint8_t c, uint16_t ratio, uint32_t t) {
    // 设置通道时间
    LedLum_t *n;
    LedLum_t *p;

    t *= 60;  // 把分进位成秒
    
    p = leddata.items[c].header;
    while (p && p->t < t) p = p->next;
    if (p && p->t == t) {
        p->v = (uint16_t) MaxLum / ratio;
        return;
    }

    n = (LedLum_t *)malloc(sizeof(LedLum_t));
    if (!n) 
        return;

    n->t = t;
    n->v = (uint16_t) MaxLum / ratio;
    n->next = NULL;

    if (!leddata.items[c].header) {
        leddata.items[c].header = n;
    } else if (t < leddata.items[c].header->t) {
        n->next = leddata.items[c].header;
        leddata.items[c].header = n;
    } else {
        p = leddata.items[c].header;
        while (p && p->next && p->next->t < t) p = p->next;
        p->next = n;
    }
}

uint8_t Led_AdjustLum(uint8_t channel, uint8_t state) {
    // 调节通道名亮度
    return 0;
    
}

void Led_TimeStep(void) {
    Led_ChannelTimeStep(0);
    Led_ChannelTimeStep(1);
    Led_ChannelTimeStep(2);
    Led_ChannelTimeStep(3);
    Led_ChannelTimeStep(4);
    Led_ChannelTimeStep(5);
    Led_ChannelTimeStep(6);
    Led_ChannelTimeStep(7);
}

uint8_t Led_Turn(uint8_t c) {
    uint8_t s;
    
    Led_TuruStateSet(c);
    
    s =  Led_SwitchGet(c);
    if (!s) s = s >> 1;
    else s = 0x2;
    Led_SwitchSet(c, s);
    
    return Led_SwitchGet(c);
}
 
uint8_t Led_SelectNextChannel(void) {
    leddata.selectChannel++;
    if (leddata.selectChannel >= leddata.channelcount)
        leddata.selectChannel = 0; 
    return leddata.selectChannel;
}

uint8_t Led_CurrentChannel(void) {
    return leddata.selectChannel;
}

uint8_t Led_TurnOf(uint8_t c, uint8_t state) {
    // 控制单个
    
    Led_TuruStateSet(c);

    
    return 0;
}
