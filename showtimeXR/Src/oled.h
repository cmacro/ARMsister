#ifndef __OLED_H__
#define __OLED_H__		

#include "armsis.h"
#include "stdlib.h"	    	

#define OLED_MODE 0
#define SIZE 8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		32
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	32	    						  
//-----------------OLED IIC端口定义----------------  					   

#define SDA_IN()  {GPIOB->CRH &= 0XFFFF0FFF; GPIOB->CRH |= 8 << 12;}
#define SDA_OUT() {GPIOB->CRH &= 0XFFFF0FFF; GPIOB->CRH |= 3 << 12;}

#define IIC_SCL    PBout(10) //SCL
#define IIC_SDA    PBout(11) //SDA	 
#define READ_SDA   PBin(11)  //输入SDA 
 		     
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据


//OLED控制用函数
void OLED_WR_Byte(unsigned dat,unsigned cmd);  
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y, unsigned char *p,u8 Char_Size);	 
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(u8 x,u8 y,u8 no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,const unsigned char BMP[]);
void fill_picture(unsigned char fill_Data);

//void Picture(void);
void IIC_Start(void);
void IIC_Stop(void);
void Write_IIC_Command(unsigned char IIC_Command);
void Write_IIC_Data(unsigned char IIC_Data);
void Write_IIC_Byte(unsigned char txd);

uint8_t IIC_Wait_Ack(void);
#endif  /*__OLED_H__*/
	 



