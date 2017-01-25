#include <reg52.h>
#include "main.h"
#include "bsp_delay.h"
#include "bsp_key.h"
#include "bsp_ds1302.h"

/* 定义控制数码管的引脚，这里使用宏定义形式是便于以后程序的移植 */
#define Segment P0
#define Digital P2

sbit beep = P3^6;

/* 定义模型变量 */
u8 location = 0;								// 记录当期是哪一位闪烁
u8 value[4] = {1, 2, 3, 4};							// 记录当前数码管显示的值
u8 alarm[4] = {1, 2, 3, 4};							// 记录闹钟响铃的时刻

enum display_mode state = ClockDisplay;
enum flicker_state flag;

u8 InterruptCount = 0;

/* 共阴极数码管段选码、位选码 '0123456789abcdef-' */
u8 SegCode[17] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x40};
u8 DigCode[4] = {0xfe, 0xfd, 0xfb, 0xf7};

int main()
{
        u8 i, j;
        u8 tmp1, tmp2;
        
        /* 这里设置的值是表示, flickerCount 次扫描全部显示, flickerCount 次扫描location位不显示 */
        u8 flickerCount = 20;
        
        IT0 = 1;
        IE = IE | 0x81;
        
        /* 定时器0工作在方式1：16位定时计数模式 */
        TMOD = 0x01;
        
        /* 这里计数初值为62500，16次就可以得到1000000次计数 */
        TH0 = 0x0b;
        TL0 = 0xdc;
        
        /* 开启总中断和定时器0中断 */
        IE = IE | 0x82;
        
        DS1302_WriteRegister(DS1302_CONTROL | DS1302_WRITE, 0x00);
        
        while(1)
        {
                switch(state)
                {
                        case (ClockDisplay):
                                tmp1 = DS1302_ReadRegister(DS1302_HR | DS1302_READ);
                                tmp2 = DS1302_ReadRegister(DS1302_MIN | DS1302_READ);
                                flag = light;
                                
                                value[0] = (tmp1 & 0xf0) >> 4;
                                value[1] = tmp1 & 0x0f;
                                value[2] = (tmp2 & 0xf0) >> 4;
                                value[3] = tmp2 & 0x0f;
                        
                                if((value[0] == alarm[0]) && (value[1] == alarm[1]) && (value[2] == alarm[2]) && (value[3] == alarm[3]))
                                {
                                        beep = 1;
                                }
                                else
                                {
                                        beep = 0;
                                }
                                break;
                        
                        case (ClockSet):
                                tmp1 = DS1302_ReadRegister(DS1302_HR | DS1302_READ);
                                tmp2 = DS1302_ReadRegister(DS1302_MIN | DS1302_READ);
                                flag = flicker;
                        
                                value[0] = (tmp1 & 0xf0) >> 4;
                                value[1] = tmp1 & 0x0f;
                                value[2] = (tmp2 & 0xf0) >> 4;
                                value[3] = tmp2 & 0x0f;
                                break;
                        
                        case (DateDisplay):
                                tmp1 = DS1302_ReadRegister(DS1302_MONTH | DS1302_READ);
                                tmp2 = DS1302_ReadRegister(DS1302_DATE | DS1302_READ);
                                flag = light;
                        
                                value[0] = (tmp1 & 0xf0) >> 4;
                                value[1] = tmp1 & 0x0f;
                                value[2] = (tmp2 & 0xf0) >> 4;
                                value[3] = tmp2 & 0x0f;
                                break;
                        
                        case (DateSet):
                                tmp1 = DS1302_ReadRegister(DS1302_MONTH | DS1302_READ);
                                tmp2 = DS1302_ReadRegister(DS1302_DATE | DS1302_READ);
                                flag = flicker;
                        
                                value[0] = (tmp1 & 0xf0) >> 4;
                                value[1] = tmp1 & 0x0f;
                                value[2] = (tmp2 & 0xf0) >> 4;
                                value[3] = tmp2 & 0x0f;
                                break;
                        
                        case (YearDisplay):
                                tmp1 = DS1302_ReadRegister(DS1302_YEAR | DS1302_READ);
                                flag = light;
                        
                                value[0] = 2;
                                value[1] = 0;
                                value[2] = (tmp1 & 0xf0) >> 4;
                                value[3] = tmp1 & 0x0f;
                                break;
                        
                        case (YearSet):
                                tmp1 = DS1302_ReadRegister(DS1302_YEAR | DS1302_READ);
                                flag = flicker;
                        
                                value[0] = 2;
                                value[1] = 0;
                                value[2] = (tmp1 & 0xf0) >> 4;
                                value[3] = tmp1 & 0x0f;
                                break;
                        
                        case (WeekDisplay):
                                tmp1 = DS1302_ReadRegister(DS1302_DAY | DS1302_READ);
                                flag = light;
                        
                                value[0] = 16;
                                value[1] = 16;
                                value[2] = tmp1 & 0x0f;
                                value[3] = 16;
                                break;
                        
                        case (WeekSet):
                                tmp1 = DS1302_ReadRegister(DS1302_DAY | DS1302_READ);
                                flag = flicker;
                        
                                value[0] = 16;
                                value[1] = 16;
                                value[2] = tmp1 & 0x0f;
                                value[3] = 16;
                                break;
                        
                        case (AlarmDisplay):
                                flag = light;
                                break;
                        
                        case (AlarmSet):
                                flag = flicker;
                                break;
                        
                        case (TemperatureDisplay):
                                flag = light;
                                break;

                }
                
                /* 这里扫描的时候是正常显示4位 */
                for(j = flickerCount; j > 0; j--)
                {
                        for(i = 0; i < 4; i++)
                        {
                                Digital = DigCode[i];
                                
                                /* 这是是在第一位位置上添加小数点 */
                                if(i == 1)
                                {
                                        /* 通过按位与操作，在不改变低7位的前提下，将位7置为高电平 */
                                        Segment = SegCode[value[i]] | 0x80;
                                }
                                else
                                {
                                        Segment = SegCode[value[i]];
                                }
                                delay_us(50);
                        }
                }
                        
                if(flicker == flag)
                {
                        /* 这里扫描的时候location对应位不显示，只显示其他位 */
                        for(j = flickerCount; j > 0; j--)
                        {
                                for(i = 0; i < 4; i++)
                                {
                                        /* 判断当期扫描的位是不是location对应位 */
                                        if(location == i)
                                        {
                                                /* 如果是的话，不选中任何位选，也就是全灭 */
                                                Digital = 0xff;
                                        }
                                        else
                                        {
                                                /* 对于其他为就正常显示就好 */
                                                Digital = DigCode[i];
                                        }
                                        
                                        /* 这里和上面是一样的，加小数点的操作 */
                                        if(i == 1)
                                        {
                                                Segment = SegCode[value[i]] | 0x80;
                                        }
                                        else
                                        {
                                                Segment = SegCode[value[i]];
                                        }
                                        delay_us(50);
                                }
                        }
                }
        }
}

/**
 * @Descroption 外部中断0服务函数
 */ 
void INT0_Handler() interrupt 0
{
        Key_Control(Key_Scan());
}

/**
 * @Descroption 定时器0中断服务函数
 */
void TIM0_Handler() interrupt 1
{
        TR0 = 0;
        
        /* 手动的装载初值 */
        TH0 = 0x0b;
        TL0 = 0xdc;
        
        TR0 = 1;
        
        InterruptCount++;
        
        /* 定时时间到了2秒之后返回时间显示界面 */
        if(InterruptCount >= 32)
        {
                InterruptCount = 0;
                TR0 = 0;
                state = ClockDisplay;
        }
}
