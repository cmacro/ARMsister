#include "light.h"

uint32_t Key_StateValues = 0x0;


#define KEY_SELECT                  0x1
#define KEY_TURN                    0x2
#define KEY_PWM                     0x4
#define KEY_TIME                    0x8


#define Key_Clicked(keyid)          (Key_StateValues & keyid)
#define Key_StateClr(keyid)         (Key_StateValues &= ~keyid)
#define Key_StateSet(keyid)         (Key_StateValues |= keyid)


void Key_Update(void) {
    if (Key_Clicked(KEY_SELECT)) {
        Key_StateClr(KEY_SELECT);
        Light_SelectNextChannel();
    }
    else if (Key_Clicked(KEY_TURN)) {
        Key_StateClr(KEY_TURN);
        Light_Turn(Light_CurrentChannel());
    }
    else if (Key_Clicked(KEY_PWM)) {
        Key_StateClr(KEY_PWM);
        Light_AdjustLum(Light_CurrentChannel(), LED_ADJUST_TURN);
    }
}
