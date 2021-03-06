
#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"


void IIC_Start()
{
    SDA_OUT();
    IIC_SDA = 1;
    IIC_SCL = 1; __nop(); //Delay_us(4);
    IIC_SDA = 0; __nop(); //Delay_us(4);
    IIC_SCL = 0; // 钳住总线
}

void IIC_Stop()
{
    SDA_OUT();
    IIC_SCL = 0;
    IIC_SDA = 0; __nop(); //Delay_us(4);
    IIC_SCL = 1; __nop(); //Delay_us(4);
    
    IIC_SDA = 1; // 释放总线
}

uint8_t IIC_Wait_Ack(void)
{
    u8 ucErrTime = 0;
    SDA_IN(); //SDA 设置为输入
    IIC_SDA=1;Delay_us(1);
    IIC_SCL=1;Delay_us(1);
    while(READ_SDA) {
        ucErrTime++;
        if(ucErrTime>250) { 
            IIC_Stop();
            return 1; 
        }
    }
    IIC_SCL = 0;//时钟输出 0
    return 0;
}

void Write_IIC_Byte(unsigned char txd)
{
    u8 t;
    SDA_OUT();
    IIC_SCL = 0; //拉低时钟开始数据传输
    for(t = 0; t < 8; t++) {
        IIC_SDA = (txd & 0x80) >> 7;
        txd <<= 1; __nop(); //Delay_us(2);
        IIC_SCL=1; __nop(); //Delay_us(2);
        IIC_SCL=0; __nop(); //Delay_us(2);
    }

}
/**********************************************
// IIC Write Command
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
    IIC_Start();
    Write_IIC_Byte(0x78);            //Slave address,SA0=0
    IIC_Wait_Ack();
    Write_IIC_Byte(0x00);			//write command
    IIC_Wait_Ack();
    Write_IIC_Byte(IIC_Command);
    IIC_Wait_Ack();
    IIC_Stop();
}
/**********************************************
// IIC Write Data
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{
    IIC_Start();
    Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
    IIC_Wait_Ack();
    Write_IIC_Byte(0x40);			//write data
    IIC_Wait_Ack();
    Write_IIC_Byte(IIC_Data);
    IIC_Wait_Ack();
    IIC_Stop();
}
void OLED_WR_Byte(unsigned dat, unsigned cmd)
{
    if (cmd) {
        Write_IIC_Data(dat);
    }
    else {
        Write_IIC_Command(dat);
    }
}


/********************************************
// fill_Picture
********************************************/
void fill_picture(unsigned char fill_Data)
{
    unsigned char m, n;
    for (m = 0; m < 8; m++)
    {
        OLED_WR_Byte(0xb0 + m, 0);		//page0-page1
        OLED_WR_Byte(0x00, 0);		//low column start address
        OLED_WR_Byte(0x10, 0);		//high column start address
        for (n = 0; n < 128; n++)
        {
            OLED_WR_Byte(fill_Data, 1);
        }

    }
}

//坐标设置
void OLED_Set_Pos(unsigned char x, unsigned char y)
{   OLED_WR_Byte(0xb0 + y, OLED_CMD);
    OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
    OLED_WR_Byte((x & 0x0f), OLED_CMD);
}
//开启OLED显示
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD); //SET DCDC命令
    OLED_WR_Byte(0X14, OLED_CMD); //DCDC ON
    OLED_WR_Byte(0XAF, OLED_CMD); //DISPLAY ON
}
//关闭OLED显示
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD); //SET DCDC命令
    OLED_WR_Byte(0X10, OLED_CMD); //DCDC OFF
    OLED_WR_Byte(0XAE, OLED_CMD); //DISPLAY OFF
}
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
void OLED_Clear(void)
{
    u8 i, n;
    for (i = 0; i < 8; i++)
    {
        OLED_WR_Byte (0xb0 + i, OLED_CMD); //设置页地址（0~7）
        OLED_WR_Byte (0x00, OLED_CMD);     //设置显示位置—列低地址
        OLED_WR_Byte (0x10, OLED_CMD);     //设置显示位置—列高地址
        for (n = 0; n < 128; n++)OLED_WR_Byte(0, OLED_DATA);
    } //更新显示
}
void OLED_On(void)
{
    u8 i, n;
    for (i = 0; i < 8; i++)
    {
        OLED_WR_Byte (0xb0 + i, OLED_CMD); //设置页地址（0~7）
        OLED_WR_Byte (0x00, OLED_CMD);     //设置显示位置—列低地址
        OLED_WR_Byte (0x10, OLED_CMD);     //设置显示位置—列高地址
        for (n = 0; n < 128; n++)OLED_WR_Byte(1, OLED_DATA);
    } //更新显示
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示
//size:选择字体 16/12
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 Char_Size)
{
    unsigned char c = 0, i = 0;
    c = chr - ' '; //得到偏移后的值
    if (x > Max_Column - 1) {x = 0; y = y + 2;}

    OLED_Set_Pos(x, y);
    for (i = 0; i < 6; i++)
        OLED_WR_Byte(F6x8[c][i], OLED_DATA);

}

//m^n函数
u32 oled_pow(u8 m, u8 n)
{
    u32 result = 1;
    while (n--)result *= m;
    return result;
}

//显示2个数字
//x,y :起点坐标
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size2)
{
    u8 t, temp;
    u8 enshow = 0;
    for (t = 0; t < len; t++)
    {
        temp = (num / oled_pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                OLED_ShowChar(x + (size2 / 2)*t, y, ' ', size2);
                continue;
            } else enshow = 1;

        }
        OLED_ShowChar(x + (size2 / 2)*t, y, temp + '0', size2);
    }
}
//显示一个字符号串
void OLED_ShowString(u8 x, u8 y, unsigned char *chr, u8 Char_Size)
{
    unsigned char j = 0;
    while (chr[j] != '\0'){   
        OLED_ShowChar(x, y, chr[j], Char_Size);
        x += 8;
        if (x > 120) {x = 0; y += 2;}
        j++;
    }
}

/***********功能描述：显示显示BMP图片128×32起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, const unsigned char BMP[])
{
    unsigned int j = 0;
    unsigned char x, y;

    if (y1 % 8 == 0) y = y1 / 8;
    else y = y1 / 8 + 1;
    for (y = y0; y < y1; y++){
        OLED_Set_Pos(x0, y);
        for (x = x0; x < x1; x++) {
            OLED_WR_Byte(BMP[j++], OLED_DATA);
        }
    }
}

//初始化SSD1306
void OLED_Init(void)
{

    // 使能GPIOB
    //RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    // 推挽输出 2M
    GPIOB->CRH &= ~(GPIO_CRH_MODE10_Msk | GPIO_CRH_CNF10_Msk |
                    GPIO_CRH_MODE11_Msk | GPIO_CRH_CNF11_Msk);
    GPIOB->CRH |= GPIO_CRH_MODE10_1 | GPIO_CRH_MODE11_1;

    SDA_OUT();

    Delay_ms(200);

    OLED_WR_Byte(0xAE, OLED_CMD); //关闭显示

    OLED_WR_Byte(0x40, OLED_CMD); //---set low column address
    OLED_WR_Byte(0xB0, OLED_CMD); //---set high column address

    OLED_WR_Byte(0xC8, OLED_CMD); //-not offset

    OLED_WR_Byte(0x81, OLED_CMD); //设置对比度
    OLED_WR_Byte(0xff, OLED_CMD);

    OLED_WR_Byte(0xa1, OLED_CMD); //段重定向设置

    OLED_WR_Byte(0xa6, OLED_CMD); //

    OLED_WR_Byte(0xa8, OLED_CMD); //设置驱动路数
    OLED_WR_Byte(0x1f, OLED_CMD);

    OLED_WR_Byte(0xd3, OLED_CMD);
    OLED_WR_Byte(0x00, OLED_CMD);

    OLED_WR_Byte(0xd5, OLED_CMD);
    OLED_WR_Byte(0xf0, OLED_CMD);

    OLED_WR_Byte(0xd9, OLED_CMD);
    OLED_WR_Byte(0x22, OLED_CMD);

    OLED_WR_Byte(0xda, OLED_CMD);
    OLED_WR_Byte(0x02, OLED_CMD);

    OLED_WR_Byte(0xdb, OLED_CMD);
    OLED_WR_Byte(0x49, OLED_CMD);

    OLED_WR_Byte(0x8d, OLED_CMD);
    OLED_WR_Byte(0x14, OLED_CMD);

    OLED_WR_Byte(0xaf, OLED_CMD);
    OLED_Clear();
}





























