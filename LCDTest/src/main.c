
#include "stm32_sis.h"

uint16_t rd[243];

void _Delay(uint32_t nCount)
{
  u32 iDelay;
  u32 x, y;  
  // 延迟
  for (iDelay=0; iDelay < nCount; iDelay++)
    for (x=0; x < 1600; x++)
      for (y=0; y < 1000; y++) 
          ;
} 

void TestRegister(void)
{
  uint8_t i;
  //__IO uint32_t rd = 0x0000;
  
  for (i = 0; i <= LCD_REG_242; i++)
    rd[i] = LCD_ReadReg(i);
}


int main(void)
{
  SysTick_Configuration();
  LCD_Init();
  
  LCD_Clear(LCD_COLOR_BLUE2);

  TestRegister();
  
  while (1){

  };                   
  
}
