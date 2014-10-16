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

#include <stdio.h>
#include "armsis.h"
#include "lcd.h"






int main(void)
{
  u16 Buf[10];
  u8 BufCnt = 0;  
  u8 CompBuf = 0;
  
  uint16_t row = 0x0000;
  uint8_t  idx;
  __IO uint16_t code = 0x000f; // 值 0x000f用来测试B组引脚状态（流水灯）
  
  Delay_init();
  serial_init();
  
//  Delay(50);
//  printf("********************\nserial test \n********************\n");
//  Delay(5);
  
  // 调试LCD
  printf("\n---------------------------------\n");
  LCD_Init();

  LCD_Clear(LCD_COLOR_GREEN);
  
  code = LCD_ReadDriverCode();
  row++;
  printf("N:%-4d  %#x\n", row, code);
  Delay(100);

  for (idx = 0x01; idx <= 0xF2; idx++) {
    code = LCD_ReadReg(idx);
    printf("R%-4x  %#x\n", idx, code);
   
    Delay(1);    
  }
    
  printf("\n---------------------------------\n");
  
  while (1){  
    if (USART1->SR & USART_SR_RXNE) {
      Buf[BufCnt] = (USART1->DR & 0x1FF);
      CompBuf = (u8)((BufCnt == 10) || (Buf[BufCnt] == 13));
      BufCnt++;
    }
    
    if (CompBuf) {
      for (idx = 0; idx < BufCnt; idx++)
        serial_send(Buf[idx]);
      
      BufCnt = 0;
      CompBuf = 0;
    }
    
    // LCD 获取ID 测试
//    code = LCD_ReadDriverCode();
//    row++;
//    printf("N:%-4d  %#x\n", row, code);
//    Delay(100);
      
    /// B组引脚流水灯测试
		//code = code << 1 | code >> 15;
		//GPIOB->ODR = code;    
    //Delay(10);
    LCD_Clear(LCD_COLOR_RED);
  };   	
  
}
