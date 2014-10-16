/**
  ********************************************************************
  *
  * @file     lcd.c
  * @author   fpack
  * @version  v1.0
  * @date     2014-9-7
  * @brief    LCD 驱动
  *
  ********************************************************************
  **/
  
  
#include "lcd.h"


/// LCD control line defines
//    PA0    ---------->  CS
//    PA1    ---------->  REST
//    PA5    ---------->  RS
//    PA6    ---------->  RW
//    PA7    ---------->  RD

#define Set_CS                  GPIOA->BSRR = GPIO_BSRR_BS0
#define Set_RSET                GPIOA->BSRR = GPIO_BSRR_BS1
#define Set_RS                  GPIOA->BSRR = GPIO_BSRR_BS5
#define Set_WR                  GPIOA->BSRR = GPIO_BSRR_BS6
#define Set_RD                  GPIOA->BSRR = GPIO_BSRR_BS7

#define Clr_CS                  GPIOA->BSRR = GPIO_BSRR_BR0
#define Clr_RSET                GPIOA->BSRR = GPIO_BSRR_BR1
#define Clr_RS                  GPIOA->BSRR = GPIO_BSRR_BR5
#define Clr_WR                  GPIOA->BSRR = GPIO_BSRR_BR6
#define Clr_RD                  GPIOA->BSRR = GPIO_BSRR_BR7

#define SetDataInputMode        GPIOB->CRL = GPIO_CR_IN_UPDOWN; \
                                GPIOB->CRH = GPIO_CR_IN_UPDOWN; 
#define SetDataOutputMode       GPIOB->CRL = GPIO_CR_GPOUT_PP2MHz; \
                                GPIOB->CRH = GPIO_CR_GPOUT_PP2MHz; 
                                
#define SetData(x)              GPIOB->ODR = x    // 设置端口数据
#define GetData                 GPIOB->IDR        // 获取端口数据

uint8_t   LCD_Succended   = 0x1;
uint16_t  LCD_DeviceCode  = 0x0000;

void LCD_DeInit(void)
{
  // 卸载屏幕
  //  关闭 B组 数据引脚
  RCC->APB2ENR &= ~RCC_APB2ENR_IOPBEN;
  GPIOB->CRL = GPIO_CR_RESET;
  GPIOB->CRH = GPIO_CR_RESET;
  LCD_Succended = 0x1;
}

void LCD_Init(void)
{
  LCD_Succended=0x0;
  LCD_CtrlLinesConfig();
  
  //************* Reset LCD Driver ****************//
  Set_RSET;
  Delay(1);        
  Clr_RSET;
  Delay(1);                           // This delay time is necessary
  Set_RSET;
  Delay(5);     
  
  LCD_WriteReg(0x00,0x0001);
  Delay(5);                           //等待内部启振

  LCD_DeviceCode=LCD_ReadReg(0x00);
    
  //************* Start Initial Sequence **********//
  LCD_WriteReg(0xE7, 0x1014);
  LCD_WriteReg(0x01, 0x0100);        // set SS and SM bit
  LCD_WriteReg(0x02, 0x0200);        // set 1 line inversion 
  LCD_WriteReg(0x03, 0x1030);        // set GRAM write direction and BGR=1. 
  LCD_WriteReg(0x08, 0x0202);        // set the back porch and front porch
  LCD_WriteReg(0x09, 0x0000);        // set non-display area refresh cycle ISC[3:0]
  LCD_WriteReg(0x0A, 0x0008);        // FMARK function    
  LCD_WriteReg(0x0C, 0x0101);        // RGB interface setting
  LCD_WriteReg(0x0D, 0x0000);        // Frame marker Position 
  LCD_WriteReg(0x0F, 0x0000);        // RGB interface polarity 
  //*************Power On sequence ****************// 
  LCD_WriteReg(0x10, 0x0000);        //* SAP,BT[3:0],AP,DSTB,SLP,STB
  LCD_WriteReg(0x11, 0x0007);        // DC1[2:0],DC0[2:0],VC[2:0]
  LCD_WriteReg(0x12, 0x0000);        // VREG1OUT voltage 
  LCD_WriteReg(0x13, 0x0000);        // VDV[4:0] for VCOM amplitude  
  Delay(20);                          // Dis-charge capacitor power voltage
  LCD_WriteReg(0x10, 0x1690);        // SAP,BT[3:0],AP,DSTB,SLP,STB
  LCD_WriteReg(0x11, 0x0227);        // DC1[2:0],DC0[2:0],VC[2:0]  
  Delay(5);                           // Delay 50ms
  LCD_WriteReg(0x12, 0x000C);        // Internal reference voltage= Vci;  
  Delay(5);                           // Delay 50ms 
  LCD_WriteReg(0x13, 0x0800);        // Set VDV[4:0] for VCOM amplitude
  LCD_WriteReg(0x29, 0x0011);        // Set VCM[5:0] for VCOMH
  LCD_WriteReg(0x2B, 0x000B);        // Set Frame Rate   
  Delay(5);                           // Delay 50ms 
  LCD_WriteReg(0x20, 0x0000);        // GRAM horizontal Address
  LCD_WriteReg(0x21, 0x0000);        // GRAM Vertical Address    
  
  // ----------- Adjust the Gamma  Curve ----------//
  LCD_WriteReg(0x30, 0x0000);
  LCD_WriteReg(0x31, 0x0106);
  LCD_WriteReg(0x32, 0x0000);
  LCD_WriteReg(0x35, 0x0204);
  LCD_WriteReg(0x36, 0x160A);
  LCD_WriteReg(0x37, 0x0707);
  LCD_WriteReg(0x38, 0x0106);
  LCD_WriteReg(0x39, 0x0707);
  LCD_WriteReg(0x3C, 0x0402);
  LCD_WriteReg(0x3D, 0x0C0F);   
  
  //------------------ Set GRAM area ---------------// 
  LCD_WriteReg(0x50, 0x0000);        // Horizontal GRAM Start Address
  LCD_WriteReg(0x51, 0x00EF);        // Horizontal GRAM End Address
  LCD_WriteReg(0x52, 0x0000);        // Vertical GRAM Start Address
  LCD_WriteReg(0x53, 0x013F);        // Vertical GRAM Start Address
  LCD_WriteReg(0x60, 0xA700);        // Gate Scan Line
  LCD_WriteReg(0x61, 0x0001);        // NDL,VLE,REV  
  LCD_WriteReg(0x6A, 0x0000);        // set scrolling line 
  
  //-------------- Partial Display Control ---------//
  LCD_WriteReg(0x80, 0x0000);
  LCD_WriteReg(0x81, 0x0000);
  LCD_WriteReg(0x82, 0x0000);
  LCD_WriteReg(0x83, 0x0000);
  LCD_WriteReg(0x84, 0x0000);
  LCD_WriteReg(0x85, 0x0000);   
  
  //-------------- Panel Control -------------------//
  LCD_WriteReg(0x90, 0x0010);
  LCD_WriteReg(0x92, 0x0600);
  LCD_WriteReg(0x93, 0x0003);
  LCD_WriteReg(0x95, 0x0110);
  LCD_WriteReg(0x97, 0x0000);
  LCD_WriteReg(0x98, 0x0000);  
  
  /* Set GRAM write direction and BGR = 1 */
  /* I/D=01 (Horizontal : increment, Vertical : decrement) */
  /* AM=1 (address is updated in vertical writing direction) */  
  LCD_WriteReg(0x03, 0x1018);
  LCD_WriteReg(0x07, 0x0000);        // 262K color and display ON
  Delay(5);                           // Delay 50ms 
  LCD_WriteReg(0x07, 0x0123);        // 262K color and display ON
  Delay(5);                           // Delay 50ms 
  
  LCD_WriteReg(0x0020,0x0000);                                                            
  LCD_WriteReg(0x0021,0x0000);  
}

void LCD_SetCursor(uint16_t x, uint16_t y)
{
  LCD_WriteReg(0x20, y);
  LCD_WriteReg(0x21, x);
}

void LCD_Clear(uint16_t Color)
{
  unsigned long int   i;

  LCD_SetCursor(0x0000, 0x0000);
  
  SetDataOutputMode;
  Clr_CS; 
  LCD_WriteRAM_Prepare();
  for(i=0;i<76800;i++)
    LCD_WriteRAM(Color);
  Set_CS;  
}
  
void LCD_DrawChar(uint16_t X, uint16_t Y, uint8_t c)
{
}
  
void LCD_TextOut(uint16_t X, uint16_t Y, uint8_t *c)
{
}

//// 基础函数
void LCD_WriteReg(uint8_t reg, uint16_t val)
{
  // 读取寄存器
  //   16bit模式
  //   
  // nCS       ----\____________________________/----------
  // RS        ------\__________/--------------------------
  // nRD       --------------------------------------------
  // nWR       ---------\_____/----------\_____/-----------
  // DB[17:0]  ------\/-------------\/------------\/-------
  // DB[17:0]        || write index || write data ||
  // DB[17:0]  ------/\-------------/\------------/\-------

  SetDataOutputMode;
  
  Clr_CS;
  
  Clr_RS;
  Set_RD;
  SetData(reg);
  Clr_WR;
  Set_WR;
  Set_RS;
  
  SetData(val);
  Clr_WR;
  Set_WR;
  
  Set_CS;
}

uint16_t LCD_ReadReg(uint8_t reg)
{
  // 读取寄存器
  //    16bit模式
  //
  // nCS       ----\_____________________________________/------
  // RS        ------\__________/-------------------------------
  // nRD       -------------------------\_____/-----------------
  // nWR       ---------\_____/---------------------------------
  // DB[17:0]  ------\/-------------\   /------------\
  // DB[17:0]        || write index |---| read  data |----------
  // DB[17:0]  ------/\-------------/   \------------/
  
	uint16_t d;
  
  SetDataOutputMode;
  
  Clr_CS;
  
  Clr_RS;
  Set_RD;
  SetData(reg);
  Clr_WR;
  Set_WR;
  Set_RS;
    
  SetDataInputMode;       // 设置输入模式
  Set_RD;
  Clr_RD;
  d = GetData;
  
  Set_CS;
  
  return d;  
}

void LCD_WriteRAM_Prepare(void)
{
  Set_RS;
  Set_RD;
  SetData(0x22);
  Clr_WR;
  Set_WR;
  Set_RS;
}
  
void LCD_WriteRAM(uint16_t RGB_Code)
{
  SetData(RGB_Code);
  Clr_WR;
  Set_WR;
  
 // Delay_us(1);
}
  
uint16_t LCD_ReadRAM(void)
{
  __IO uint16_t d;
  
  Set_RD;
  Clr_RD;
  d = GetData;
  
  return d;
}

void LCD_PowerOn(void)
{
}

void LCD_DisplayOn(void)
{
}

void LCD_DisplayOff(void)
{
}

//    底层函数
void LCD_CtrlLinesConfig(void)
{
  /**************************************
   * LCD GPIO configuration
   **************************************
   *  Control line
   * ------------------------------------
   *  PA0    ---------->  CS
   *  PA1    ---------->  REST
   *  PA5    ---------->  RS
   *  PA6    ---------->  RW
   *  PA7    ---------->  RD
   * ------------------------------------
   * Data line
   * ------------------------------------
   *  PB00    ----------> DB0
   *  PB01    ----------> DB1
   *  PB02    ----------> DB2
   *  PB03    ----------> DB3
   *  PB04    ----------> DB4
   *  PB05    ----------> DB5
   *  PB06    ----------> DB6
   *  PB07    ----------> DB7
   *  PB08    ----------> DB10
   *  PB09    ----------> DB11
   *  PB10    ----------> DB12
   *  PB11    ----------> DB13
   *  PB12    ----------> DB14
   *  PB13    ----------> DB15
   *  PB14    ----------> DB16
   *  PB15    ----------> DB17
  ***************************************/    
  
  //
  // 打开控制线
  //   数据线  PB组端口(DB0 ~ DB17)  设置为2MHz开漏输出
  //   控制线  PA8 ~ PA12            设置为2MHz推挽输出
  //
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
  // 关闭JTAG-DP，释放 PB04、PB05端口
  AFIO->MAPR &= ~AFIO_MAPR_SWJ_CFG;
  AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
 
  //
  // 数据线 
  SetDataOutputMode;
  
  // 
  //  控制线 
  GPIOA->CRL &= ~ (GPIO_CRL_SET0 | 
                   GPIO_CRL_SET1 | 
                   GPIO_CRL_SET5 | 
                   GPIO_CRL_SET6 | 
                   GPIO_CRL_SET7);
  GPIOA->CRL |= GPIO_CR_GPOUT_PP2MHz & 
                (GPIO_CRL_SET0 |           // CS
                 GPIO_CRL_SET1 |           // RESET 
                 GPIO_CRL_SET5 |           // RS
                 GPIO_CRL_SET6 |           // RW
                 GPIO_CRL_SET7);           // RD 
                  
  GPIOB->ODR = 0xffff;
  Set_CS;
  Set_RS;
  Set_WR;
  Set_RD;
  Set_RSET;
  //Delay(50);
}

uint16_t LCD_ReadDriverCode(void)
{
  uint16_t d;
  LCD_WriteReg(0x00, 0x0001);

  // 初始化内部晶振
  //  写入寄存器后需要延迟，再读取。否则第一次法正确读取
  Delay(5);                   
  d = LCD_ReadReg(0x00);
  
  return d;
}
