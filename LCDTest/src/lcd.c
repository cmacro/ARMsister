/**
	*	
	*	@file			lcd.c
	*	@author		crystalice
	* @version	v1.0
	* @date			2014-08-09
	*	@brief		LCD 驱动，支持LCD_ILI9320
	*
	*/

#include "lcd.h"

/// LCD control line defines
//    PA08  ---------->  RS
//    PA09  ---------->  RW
//    PA10  ---------->  RD
//    PA11  ---------->  CS
//    PA12  ---------->  REST
#define Set_CS              GPIOA->BSRR = GPIO_BSRR_BS11
#define Set_RD              GPIOA->BSRR = GPIO_BSRR_BS10
#define Set_WR              GPIOA->BSRR = GPIO_BSRR_BS9
#define Set_RS              GPIOA->BSRR = GPIO_BSRR_BS8
#define Set_RSET            GPIOA->BSRR = GPIO_BSRR_BS12

#define Clr_CS              GPIOA->BSRR = GPIO_BSRR_BR11
#define Clr_RD              GPIOA->BSRR = GPIO_BSRR_BR10
#define Clr_WR              GPIOA->BSRR = GPIO_BSRR_BR9
#define Clr_RS              GPIOA->BSRR = GPIO_BSRR_BR8
#define Clr_RSET            GPIOA->BSRR = GPIO_BSRR_BR12

#define SetDataInputMode        GPIOB->CRL = GPIO_CR_PPPDINPUT; \
                                GPIOB->CRH = GPIO_CR_PPPDINPUT
#define SetDataOutputMode       GPIOB->CRL = GPIO_CR_OUT_50MHz | GPIO_CR_GP_PUSHPULL; \
                                GPIOB->CRH = GPIO_CR_OUT_50MHz | GPIO_CR_GP_PUSHPULL


#define SetData(x)              GPIOB->ODR = x    // 设置端口数据
#define GetData                 GPIOB->IDR    // 获取端口数据


uint8_t   LCD_Succended   = 0x1;
uint16_t  LCD_DeviceCode  = 0x0;


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
  LCD_CtrlLinesConfig();
  
  Delay(20);
  LCD_WriteReg(0x00, 0x0001);
  Delay(20);
  LCD_DeviceCode = LCD_ReadReg(0x00);
  //if (LCD_DeviceCode == LCD_DC_ILI9320) {
    LCD_Succended = 0x0;
    /* Start Initial Sequence ----------------------------------------------------*/
    LCD_WriteReg(LCD_REG_229,0x8000); /* Set the internal vcore voltage */
    LCD_WriteReg(LCD_REG_0,  0x0001); /* Start internal OSC. */
    LCD_WriteReg(LCD_REG_1,  0x0100); /* set SS and SM bit */
    LCD_WriteReg(LCD_REG_2,  0x0700); /* set 1 line inversion */
    LCD_WriteReg(LCD_REG_3,  0x1030); /* set GRAM write direction and BGR=1. */
    LCD_WriteReg(LCD_REG_4,  0x0000); /* Resize register */
    LCD_WriteReg(LCD_REG_8,  0x0202); /* set the back porch and front porch */
    LCD_WriteReg(LCD_REG_9,  0x0000); /* set non-display area refresh cycle ISC[3:0] */
    LCD_WriteReg(LCD_REG_10, 0x0000); /* FMARK function */
    LCD_WriteReg(LCD_REG_12, 0x0000); /* RGB interface setting */
    LCD_WriteReg(LCD_REG_13, 0x0000); /* Frame marker Position */
    LCD_WriteReg(LCD_REG_15, 0x0000); /* RGB interface polarity */
    /* Power On sequence ---------------------------------------------------------*/
    LCD_WriteReg(LCD_REG_16, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    LCD_WriteReg(LCD_REG_17, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
    LCD_WriteReg(LCD_REG_18, 0x0000); /* VREG1OUT voltage */
    LCD_WriteReg(LCD_REG_19, 0x0000); /* VDV[4:0] for VCOM amplitude */
    Delay(20);                 /* Dis-charge capacitor power voltage (200ms) */
    LCD_WriteReg(LCD_REG_16, 0x17B0); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    LCD_WriteReg(LCD_REG_17, 0x0137); /* DC1[2:0], DC0[2:0], VC[2:0] */
    Delay(20);                  /* Delay 50 ms */
    LCD_WriteReg(LCD_REG_18, 0x0139); /* VREG1OUT voltage */
    Delay(20);                  /* Delay 50 ms */
    LCD_WriteReg(LCD_REG_19, 0x1d00); /* VDV[4:0] for VCOM amplitude */
    LCD_WriteReg(LCD_REG_41, 0x0013); /* VCM[4:0] for VCOMH */
    Delay(20);                  /* Delay 50 ms */
    LCD_WriteReg(LCD_REG_32, 0x0000); /* GRAM horizontal Address */
    LCD_WriteReg(LCD_REG_33, 0x0000); /* GRAM Vertical Address */
    /* Adjust the Gamma Curve ----------------------------------------------------*/
    LCD_WriteReg(LCD_REG_48, 0x0006);
    LCD_WriteReg(LCD_REG_49, 0x0101);
    LCD_WriteReg(LCD_REG_50, 0x0003);
    LCD_WriteReg(LCD_REG_53, 0x0106);
    LCD_WriteReg(LCD_REG_54, 0x0b02);
    LCD_WriteReg(LCD_REG_55, 0x0302);
    LCD_WriteReg(LCD_REG_56, 0x0707);
    LCD_WriteReg(LCD_REG_57, 0x0007);
    LCD_WriteReg(LCD_REG_60, 0x0600);
    LCD_WriteReg(LCD_REG_61, 0x020b);
    
    /* Set GRAM area -------------------------------------------------------------*/
    LCD_WriteReg(LCD_REG_80, 0x0000); /* Horizontal GRAM Start Address */
    LCD_WriteReg(LCD_REG_81, 0x00EF); /* Horizontal GRAM End Address */
    LCD_WriteReg(LCD_REG_82, 0x0000); /* Vertical GRAM Start Address */
    LCD_WriteReg(LCD_REG_83, 0x013F); /* Vertical GRAM End Address */
    LCD_WriteReg(LCD_REG_96,  0x2700); /* Gate Scan Line */
    LCD_WriteReg(LCD_REG_97,  0x0001); /* NDL,VLE, REV */
    LCD_WriteReg(LCD_REG_106, 0x0000); /* set scrolling line */
    /* Partial Display Control ---------------------------------------------------*/
    LCD_WriteReg(LCD_REG_128, 0x0000);
    LCD_WriteReg(LCD_REG_129, 0x0000);
    LCD_WriteReg(LCD_REG_130, 0x0000);
    LCD_WriteReg(LCD_REG_131, 0x0000);
    LCD_WriteReg(LCD_REG_132, 0x0000);
    LCD_WriteReg(LCD_REG_133, 0x0000);
    /* Panel Control -------------------------------------------------------------*/
    LCD_WriteReg(LCD_REG_144, 0x0010);
    LCD_WriteReg(LCD_REG_146, 0x0000);
    LCD_WriteReg(LCD_REG_147, 0x0003);
    LCD_WriteReg(LCD_REG_149, 0x0110);
    LCD_WriteReg(LCD_REG_151, 0x0000);
    LCD_WriteReg(LCD_REG_152, 0x0000);
    /* Set GRAM write direction and BGR = 1 */
    /* I/D=01 (Horizontal : increment, Vertical : decrement) */
    /* AM=1 (address is updated in vertical writing direction) */
    LCD_WriteReg(LCD_REG_3, 0x1018);
    LCD_WriteReg(LCD_REG_7, 0x0173); /* 262K color and display ON */   
  //}
  
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

void ili9320_WriteIndex(u16 idx)
{    
  GPIOB->ODR=0XFFFF;    //全部输出高
  
  Clr_RS;
  Set_RD;
  _delay_(1);         
  GPIOB->ODR = idx;
  _delay_(1);
  Clr_WR;
  Set_WR;
  Set_RS;
}

void ili9320_WriteData(u16 data)
{
  GPIOB->ODR=0XFFFF;    //全部输出高
  Set_RS;
  Set_RD;
  _delay_(1);         
  GPIOB->ODR= data;
  _delay_(1);
  Clr_WR;
  Set_WR;
}


u16 ili9320_ReadData(void)
{
  u16 val = 0;
  GPIOB->ODR =0X0000;
  Set_RS;
  Set_WR;
  Clr_RD;  
  _delay_(1);                 
  val = GPIOB->IDR;
  _delay_(1);
  Set_RD;
  return val;
}



//// 基础函数
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
  // 读取寄存器
  //   16线模式，P51
  //   
//  Clr_CS;
//  Clr_RS;
//  Set_RD;
//  SetData(LCD_Reg);
//  Clr_WR;
//  Set_WR;
//  Set_RS;
//  
//  SetData(LCD_RegValue);
//  Clr_WR;
//  Set_WR;
//  Set_CS;
  
  
  Clr_CS;
  ili9320_WriteIndex(LCD_Reg);      
  ili9320_WriteData(LCD_RegValue);    
  Set_CS;  
}
  
void LCD_WR_REG(u8 data)
{ 
	//Clr_RS;//写地址  
 	Clr_CS;// LCD_CS_CLR; 
	SetData(data); // DATAOUT(data); 
	Clr_WR;// LCD_WR_CLR; 
	Set_WR;// LCD_WR_SET; 
 	//Set_CS; // LCD_CS_SET;   
}

uint16_t LCD_ReadReg(uint8_t LCD_Reg)
{
//  // 写寄存器
//  //    16线模式， P51
//	__IO uint16_t d;
//  
//	Clr_CS;
//	Set_RS;
//	Set_RD;
//  SetData(LCD_Reg);
//  Clr_WR;
//	Set_WR;
//  Set_RS;
//    
//  SetDataInputMode;
//  Set_RD;
//	Clr_RD;
//	d = GetData;
//  Set_CS;
//  
//  SetDataOutputMode;
//  
//	return d;

	u16 t;
  Clr_RS;
  Clr_CS;
  
	LCD_WR_REG(LCD_Reg);  //写入要读的寄存器号  
	GPIOB->CRL=0X88888888; //PB0-7  上拉输入
	GPIOB->CRH=0X88888888; //PB8-15 上拉输入
	GPIOB->ODR=0XFFFF;    //全部输出高

  //Set_RS;// LCD_RS_SET;
	//Clr_CS;// LCD_CS_CLR;
	//读取数据(读寄存器时,并不需要读2次)
	Clr_RD;// LCD_RD_CLR;
	_delay_(1); // delay_us(5);//FOR 8989,延时5us					   
	Set_RD; // LCD_RD_SET;
	t = GetData; // t=DATAIN;  
	Set_CS; // LCD_CS_SET; 

	GPIOB->CRL=0X33333333; //PB0-7  上拉输出
	GPIOB->CRH=0X33333333; //PB8-15 上拉输出
	GPIOB->ODR=0XFFFF;    //全部输出高
	return t;  

//  Clr_CS;
//  ili9320_WriteIndex(LCD_Reg);     
//  LCD_Reg = ili9320_ReadData();              
//  Set_CS;
//  return LCD_Reg;
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
  /**
  * LCD GPIO configuration
  *
    Control line
    PA08  ---------->  RS
    PA09  ---------->  RW
    PA10  ---------->  RD
    PA11  ---------->  CS
    PA12  ---------->  REST

    Data line
    PB00    ----------> DB0
    PB01    ----------> DB1
    PB02    ----------> DB2
    PB03    ----------> DB3
    PB04    ----------> DB4
    PB05    ----------> DB5
    PB06    ----------> DB6
    PB07    ----------> DB7
    PB08    ----------> DB10
    PB09    ----------> DB11
    PB10    ----------> DB12
    PB11    ----------> DB13
    PB12    ----------> DB14
    PB13    ----------> DB15
    PB14    ----------> DB16
    PB15    ----------> DB17
  **/    
  
  //
  // 打开控制线
  //   数据线  PB组端口(DB0 ~ DB17)  设置为2MHz开漏输出
  //   控制线  PA8 ~ PA12            设置为2MHz推挽输出
  //
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN; // | RCC_APB2ENR_AFIOEN;
  
  // 关闭JTAG-DP，释放 PB04、PB05端口
  //AFIO->MAPR &= ~AFIO_MAPR_SWJ_CFG;
  //AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
  
  SetDataOutputMode;
  GPIOA->CRH &= ~ (GPIO_CRH_MODE8 | GPIO_CRH_CNF8 |
                    GPIO_CRH_MODE9 | GPIO_CRH_CNF9 | 
                    GPIO_CRH_MODE10 | GPIO_CRH_CNF10 |
                    GPIO_CRH_MODE11 | GPIO_CRH_CNF11 |
                    GPIO_CRH_MODE12 | GPIO_CRH_CNF12);
  GPIOA->CRH |= GPIO_CRH_MODE8_1 | GPIO_CRH_MODE9_1 | GPIO_CRH_MODE10_1 | GPIO_CRH_MODE11_1 | GPIO_CRH_MODE12_1;
                  
  //GPIOA->ODR |= 0x1f00;
  GPIOB->ODR = 0xffff;
  Set_CS;
  Set_RS;
  Set_WR;
  Set_RD;
  Set_RSET;
}


