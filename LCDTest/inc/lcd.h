/**
  * @file     lcd.h
  * @author   crystalice
  * @version  v1.0
  * @date     2014-08-09
  * @brief    LCD 驱动头文件
  *
  ***************************************************************
  * LCD GPIO configuration
  * ------------------------------------
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
  ***************************************************************/


#ifndef __LCD_H__
#define __LCD_H__

#ifdef __cplusplus
  extern "C" {
#endif

#include "armsis.h"
#include "fonts.h"
    
 
/** 
  * @brief  LCD Registers  
  */ 
#define R0             0x00
#define R1             0x01
#define R2             0x02
#define R3             0x03
#define R4             0x04
#define R5             0x05
#define R6             0x06
#define R7             0x07
#define R8             0x08
#define R9             0x09
#define R10            0x0A
#define R12            0x0C
#define R13            0x0D
#define R14            0x0E
#define R15            0x0F
#define R16            0x10
#define R17            0x11
#define R18            0x12
#define R19            0x13
#define R20            0x14
#define R21            0x15
#define R22            0x16
#define R23            0x17
#define R24            0x18
#define R25            0x19
#define R26            0x1A
#define R27            0x1B
#define R28            0x1C
#define R29            0x1D
#define R30            0x1E
#define R31            0x1F
#define R32            0x20
#define R33            0x21
#define R34            0x22
#define R35            0x23
#define R36            0x24
#define R37            0x25
#define R39            0x27
#define R40            0x28
#define R41            0x29
#define R43            0x2B
#define R45            0x2D
#define R48            0x30
#define R49            0x31
#define R50            0x32
#define R51            0x33
#define R52            0x34
#define R53            0x35
#define R54            0x36
#define R55            0x37
#define R56            0x38
#define R57            0x39
#define R58            0x3A
#define R59            0x3B
#define R60            0x3C
#define R61            0x3D
#define R62            0x3E
#define R63            0x3F
#define R64            0x40
#define R65            0x41
#define R66            0x42
#define R67            0x43
#define R68            0x44
#define R69            0x45
#define R70            0x46
#define R71            0x47
#define R72            0x48
#define R73            0x49
#define R74            0x4A
#define R75            0x4B
#define R76            0x4C
#define R77            0x4D
#define R78            0x4E
#define R79            0x4F
#define R80            0x50
#define R81            0x51
#define R82            0x52
#define R83            0x53
#define R84            0x54
#define R85            0x55
#define R86            0x56
#define R87            0x57
#define R88            0x58
#define R89            0x59
#define R90            0x5A
#define R91            0x5B
#define R92            0x5C
#define R93            0x5D
#define R96            0x60
#define R97            0x61
#define R106           0x6A
#define R118           0x76
#define R128           0x80
#define R129           0x81
#define R130           0x82
#define R131           0x83
#define R132           0x84
#define R133           0x85
#define R134           0x86
#define R135           0x87
#define R136           0x88
#define R137           0x89
#define R139           0x8B
#define R140           0x8C
#define R141           0x8D
#define R143           0x8F
#define R144           0x90
#define R145           0x91
#define R146           0x92
#define R147           0x93
#define R148           0x94
#define R149           0x95
#define R150           0x96
#define R151           0x97
#define R152           0x98
#define R153           0x99
#define R154           0x9A
#define R157           0x9D
#define R192           0xC0
#define R193           0xC1
#define R229           0xE5
#define R232           0xE8
#define R233           0xE9
#define R234           0xEA
#define R235           0xEB
#define R236           0xEC
#define R237           0xED
#define R241           0xF1
#define R242           0xF2     
   

// color
#define LCD_COLOR_WHITE          0xFFFF
#define LCD_COLOR_BLACK          0x0000
#define LCD_COLOR_GREY           0xF7DE
#define LCD_COLOR_BLUE           0x001F
#define LCD_COLOR_BLUE2          0x051F
#define LCD_COLOR_RED            0xF800
#define LCD_COLOR_MAGENTA        0xF81F
#define LCD_COLOR_GREEN          0x07E0
#define LCD_COLOR_CYAN           0x7FFF
#define LCD_COLOR_YELLOW         0xFFE0

extern uint8_t   LCD_Succended;
extern uint16_t  LCD_DeviceCode;

void      LCD_Init(void);
void      LCD_DeInit(void);

void      LCD_Clear(uint16_t Color);
void      LCD_DrawChar(uint16_t X, uint16_t Y, uint8_t c);
void      LCD_TextOut(uint16_t X, uint16_t Y, uint8_t *c);
uint16_t  LCD_ReadDriverCode(void);

//// 基础函数
void      LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue);
uint16_t  LCD_ReadReg(uint8_t LCD_Reg);
void      LCD_WriteRAM_Prepare(void);
void      LCD_WriteRAM(uint16_t RGB_Code);
uint16_t  LCD_ReadRAM(void);
void      LCD_PowerOn(void);
void      LCD_DisplayOn(void);
void      LCD_DisplayOff(void);

/// 底层配置
void      LCD_CtrlLinesConfig(void);
  



#ifdef __cplusplus
  }
#endif

#endif /* __LCD_H__ */
