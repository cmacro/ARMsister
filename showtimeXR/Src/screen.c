

#include "oled.h"
#include "light.h"

void AMSScreen_ShowTime(uint32_t t) {
    uint8_t h, m;
    uint8_t d[6];
    
    h = t / 3600;
    m = (t % 3600) / 60;
    
    d[0] = (h / 10) + 0x30;
    d[1] = (h % 10) + 0x30;
    d[2] = t & 1 ? ':' : ' ';
    d[3] = (m / 10) + 0x30;
    d[4] = (m % 10) + 0x30;  
    d[5] = 0x0;    
    
    if (t & 1) {
        if (Light_StateClick(LIGHTSTATE_SETHOUR)) {
            d[2] = '\'';
        } 
        else if (Light_StateClick(LIGHTSTATE_SETMIN)) {
            d[2] = '.';
        }
    }

    OLED_ShowString(88, 3, d, 8);
}
